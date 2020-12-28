import React from "react";
import PropTypes from "prop-types";
import Config from "../configuration.json";
import { Form, Button, Spinner, Status, NumericInput, IPv4, DNSName, Select, OptGroupSelect } from "./UiComponents";
import StateMachine from 'javascript-state-machine';

const StatusType = Object.freeze({
    info: "info",
    error: "error",
    success: "success"
});

const ValidHostnameRegex = "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$";
//var regexObj = new RegExp(ValidHostnameRegex);

export class ConfigPage extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            showLoadingIndicator: null,
            statusText: null,
            statusType: StatusType.info,
            saveButtonEnabled: false,
        };

        this.fields = Object.freeze([
            "useNTP",
            "operationMode",
            "serverProductId",
            "serverAddressType",
            "serverIp",
            "serverDNS",
            "serverPort",
            "powerThresholdHigh",
            "powerThresholdLow",
            "measureInterval",
            "enableStatusLED",
        ]);

        this.controls = {};

        this.fields.map((field) => {
            this.controls[field] = Config.find(obj => { return obj.name === field; });
            return;
        });

        this.fetchData = this.fetchData.bind(this);
        this.handleSave = this.handleSave.bind(this);
        this.checkModified=this.checkModified.bind(this);
        this.onChangeValue=this.onChangeValue.bind(this);

        this.machine = new StateMachine({
            init: 'init',
            transitions: [
                { name: 'load', from: 'init', to: 'loading' },
                { name: 'load', from: 'saved', to: 'loading' },
                { name: 'load', from: 'load_failed', to: 'loading' },
                { name: 'load_success', from: 'loading', to: 'ready' },
                { name: 'load_fail', from: 'loading', to: 'load_failed' },
                { name: 'modify', from: 'ready', to: 'modified' },
                { name: 'modify', from: 'saved', to: 'modified' },
                { name: 'modify', from: 'save_failed', to: 'modified' },
                { name: 'save', from: 'modified', to: 'saving' },
                { name: 'revert', from: 'modified', to: 'ready' },
                { name: 'save', from: 'save_failed', to: 'saving' },
                { name: 'save_success', from: 'saving', to: 'saved' },
                { name: 'save_fail', from: 'saving', to: 'save_failed' },
            ],
            methods: {
                onEnterState: (lifecycle)=> {
                    console.info(lifecycle.to);
                    switch (lifecycle.to) {
                        case 'loading':
                            this.setState({ saveButtonEnabled: false, showLoadingIndicator: true, statusType: StatusType.info, statusText: "loading settings" });
                            break;
                        case 'ready':
                            if(this.state.isSaved){
                                this.setState({ saveButtonEnabled: false, showLoadingIndicator: false, statusType: StatusType.success, statusText: "configuration was saved!", isSaved: false });
                            }else{
                                this.setState({ saveButtonEnabled: false, showLoadingIndicator: false, statusType: StatusType.info, statusText: null, isSaved: false }); 
                            }                           
                            break;
                        case 'load_failed':
                            this.setState({ saveButtonEnabled: false, showLoadingIndicator: true, statusType: StatusType.error, statusText: "something went wrong!" });
                            break;
                        case 'modified':
                            this.setState({ saveButtonEnabled: true, showLoadingIndicator: false, statusType: StatusType.info, statusText: "" });
                            break;
                        case 'saved':
                            this.setState({ saveButtonEnabled: false, showLoadingIndicator: false, statusType: StatusType.success, statusText: "saved!" , isSaved: true });
                            break;
                        case 'save_failed':
                            this.setState({ saveButtonEnabled: true, showLoadingIndicator: false, statusType: StatusType.error, statusText: "save changes failed" });
                            break;

                    }
                }
            }
        });
    }

    fetchData() {
        this.machine.load();
        fetch(`${this.props.API}/api/config/get`)
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

    handleSave(event) {
        event.preventDefault();

        this.machine.save();
        let data = new FormData();

        let payload = {};
        this.fields.map((field) => {
            payload[field] = this.state[field];
            return;
        });

        data.append("data", JSON.stringify(payload));

        fetch(`${this.props.API}/api/config/set`,
            {
                method: "post",
                body: data
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

    checkModified(){
        let isModified=false;
        this.fields.map((field) => {
            if (this.state[field] !== this.state[`${field}_initial`]) {
                isModified=true;
                return;
            }
        });

        if(isModified){
            if(this.machine.can("modify")){
                this.machine.modify();
            }            
        }else{
            if(this.machine.can("revert")){
                this.machine.revert();
            }
        }
    }

    onChangeValue(newState){ 
        this.setState(newState, this.checkModified); 
    }

    componentDidMount() {
        this.fetchData();
    }

    render() {
        return (
            <div>
                <p>
                    <h2>Configuration   {this.machine.state}</h2>
                    <Status type={this.state.statusType} text={this.state.statusText} showSpinner={this.state.showLoadingIndicator}/>
                </p>
                <Form>
                    <Select control={this.controls.useNTP} state={this.state} setState={this.onChangeValue} />
                    <Select control={this.controls.enableStatusLED} state={this.state} setState={this.onChangeValue} />
                    <Select control={this.controls.operationMode} state={this.state} setState={this.onChangeValue} />
                    <OptGroupSelect control={this.controls.serverProductId} state={this.state} setState={this.onChangeValue} />
                    <Select control={this.controls.serverAddressType} state={this.state} setState={this.onChangeValue} />
                    <IPv4 control={this.controls.serverIp} state={this.state} setState={this.onChangeValue} />
                    <DNSName control={this.controls.serverDNS} state={this.state} setState={this.onChangeValue} />
                    <NumericInput control={this.controls.serverPort} state={this.state} setState={this.onChangeValue} />
                    <NumericInput control={this.controls.powerThresholdHigh} state={this.state} setState={this.onChangeValue} />
                    <NumericInput control={this.controls.powerThresholdLow} state={this.state} setState={this.onChangeValue} />
                    <Select control={this.controls.measureInterval} state={this.state} setState={this.onChangeValue} />                    
                    <p>
                        <Button name="buttonSave" title="save" onClick={this.handleSave} isDisabled={!this.state.saveButtonEnabled} />
                    </p>
                </Form>
            </div >
        );
    }
}

ConfigPage.propTypes = {
    API: PropTypes.string,
};
