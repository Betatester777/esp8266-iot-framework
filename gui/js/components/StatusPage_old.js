import React from "react";
import PropTypes from "prop-types";
import Config from "../status.json";
import { Form, Spinner, Status, Select, NumericInput } from "./UiComponents";
import StateMachine from 'javascript-state-machine';

const StatusType = Object.freeze({
    info: "info",
    error: "error",
    success: "success"
});

export class StatusPage extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            showLoadingIndicator: null,
            statusText: null,
            statusType: StatusType.info
        };

        this.fields = Object.freeze([
            "operationMode",
            "measuredPower",
            "outputStatus"
        ]);

        this.controls = {};

        this.fields.map((field) => {
            this.controls[field] = Config.find(obj => { return obj.name === field; });
            return;
        });

        this.fetchData = this.fetchData.bind(this);
        this.handleSave = this.handleSave.bind(this);
        this.onChangeValue=this.onChangeValue.bind(this);

        this.machine = new StateMachine({
            init: 'init',
            transitions: [
                { name: 'load', from: 'init', to: 'loading' },
                { name: 'load', from: 'saved', to: 'loading' },
                { name: 'load', from: 'ready', to: 'loading' },
                { name: 'load', from: 'load_failed', to: 'loading' },
                { name: 'load_success', from: 'loading', to: 'ready' },
                { name: 'load_fail', from: 'loading', to: 'load_failed' },
                { name: 'save', from: 'ready', to: 'saving' },
                { name: 'save', from: 'save_fail', to: 'saving' },
                { name: 'save_success', from: 'saving', to: 'saved' },
                { name: 'save_fail', from: 'saving', to: 'save_failed' },
            ],
            methods: {
                onEnterState: (lifecycle)=> {
                    console.info(lifecycle.to);
                    switch (lifecycle.to) {
                        case 'loading':
                            this.setState({showLoadingIndicator: true, statusType: StatusType.info, statusText: "loading status" });
                            break;
                        case 'ready':
                            this.setState({ showLoadingIndicator: false, statusType: StatusType.info, statusText: null, isSaved: false }); 
                            break;
                        case 'load_failed':
                            this.setState({ showLoadingIndicator: true, statusType: StatusType.error, statusText: "something went wrong!" });
                            break;
                        case 'saved':
                            this.setState({ showLoadingIndicator: false, statusType: StatusType.success, statusText: "status updated!" });
                            break;
                        case 'save_failed':
                            this.setState({ showLoadingIndicator: false, statusType: StatusType.error, statusText: "status update failed" });
                            break;

                    }
                }
            }
        });
    }

    fetchData() {
        this.machine.load();
        fetch(`${this.props.API}/api/status/get`)
            .then(response => response.json())
            .then((configData) => {
                let newState={}
                this.fields.map((field) => {
                    newState[field] = configData[field];
                    newState[`${field}_initial`] = configData[field];
                    return;
                });

                this.setState(newState);
                this.machine.loadSuccess();
            })
            .catch((error) => {
                // handle your errors here
                console.error(error);
                this.machine.loadFail();
            })
    }

    handleSave(name) {
        if(this.machine.can('save')){
            this.machine.save();
        }else{
            console.warn(`No save transition is available in current state [state=${this.machine.state}]`);
            return false;
        }

        let selectedOptions=this.controls[name].options.filter((option)=>{
            return option.value===this.state[name];
        })

        console.info(`Fetch HTTP_GET [url=${this.props.API}/api${selectedOptions[0].api}]`);
        fetch(`${this.props.API}/api${selectedOptions[0].api}`,
            {
                method: "get",
                body: null
            }).then((response) => {
                console.info("status:", response.status);
                if (response.status == 200) {
                    this.machine.saveSuccess();
                    this.fetchData();
                } else {
                    console.error(`server response status: [${response.status}]`);
                    this.machine.saveFail();
                }
            }).catch((error) => {
                console.error(error);
                this.machine.saveFail();
            });
        return false;
    }

    onChangeValue(newState){ 
        let name=Object.keys(newState)[0];
        this.setState(newState, ()=>{this.handleSave(name)}); 
    }

    componentDidMount() {
        this.fetchData();
        this.timer = setInterval(()=> this.fetchData(), 2000)
    }

    render() {
        return (
            <div>
                <p>
                    <h2>Status   {this.machine.state}</h2>
                </p>
                <Form>
                    <Select control={this.controls.outputStatus} state={this.state} setState={this.onChangeValue} isReadOnly={this.state.operationMode==1}/> 
                    <Select control={this.controls.operationMode} state={this.state} setState={this.onChangeValue}/>
                    <NumericInput control={this.controls.measuredPower} state={this.state} setState={null} isReadOnly={true}/>                                      
                </Form>
                <p><Status type={this.state.statusType} text={this.state.statusText} showSpinner={this.state.showLoadingIndicator}/></p>
            </div >
        );
    }
}

Status.propTypes = {
    API: PropTypes.string,
};
