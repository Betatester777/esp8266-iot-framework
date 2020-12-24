import React from "react";
import PropTypes from "prop-types";
import Config from "../configuration.json";
import { Form, Button, DisabledButton, Spinner } from "./UiComponents";
import MaskedInput from 'react-text-mask'

const ipProps = {
    guide: false,
    placeholderChar: '\u2000',
    mask: value => Array(value.length).fill(/[\d.]/),
    pipe: value => {
        if (value === '.' || value.endsWith('..')) return false;

        const parts = value.split('.');

        if (
            parts.length > 4 ||
            parts.some(part => part === '00' || part < 0 || part > 255)
        ) {
            return false;
        }

        return value;
    },
};

export class ConfigPage extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            error: null,
            status: null,
            dataReceived: false,
            dataSaving: false
        };

        this.fields = Object.freeze([
            "useNTP",
            "operationMode",
            "serverIp", "serverPort",
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
        this.onNumericInput=this.onNumericInput.bind(this);
        this.onNumericChange=this.onNumericChange.bind(this);
    }

    fetchData() {
        this.setState({ error: null, status: null });
        fetch(`${this.props.API}/api/config/get`)
            .then(response => response.json())
            .then((configData) => {
                let newState={  dataReceived: true,dataSaving: false};

                this.fields.map((field) => {
                    newState[field]= configData[field];
                    newState[`${field}_initial`]= configData[field];
                    return;
                });

                this.setState(newState);
            })
            .catch((error) => {
                // handle your errors here
                console.error(error);
                this.setState({ dataReceived: false, dataSaving: false, error: "failed to load configuration", status: "" });
            })
    }

    handleSave(event) {
        event.preventDefault();
        let data = new FormData();

        let payload={};
        this.fields.map((field) => {
            payload[field]= this.state[field];
            return;
        });

        data.append("data", JSON.stringify(payload));

        this.setState({ error: null, status: null, dataReceived: true, dataSaving: true });
        fetch(`${this.props.API}/api/config/set`,
            {
                method: "post",
                body: data
            }).then((response) => {
                console.info("status:", response.status);
                if (response.status == 200) {
                    this.setState({ status: "configuration was saved successfully" });
                    setTimeout(() => { this.setState({ status: null }) }, 5000);
                    this.fetchData();
                } else {
                    this.setState({ error: "failed to save configuration", dataSaving: false });
                    setTimeout(() => { this.setState({ error: null }) }, 5000);
                }
            }).catch((ex) => {
                this.setState({ error: "failed to save configuration", dataSaving: false });
                setTimeout(() => { this.setState({ error: null }) }, 5000);
            });
        return false;
    }

    componentDidMount() {
        this.fetchData();
    }

    onNumericChange(event){
        let { value, min } = event.target;
        let name=event.target.name;

        let newState={};
        newState[name]=Number(value);        
        this.setState(newState);
    }

    onNumericInput(event){
        let { value, min, max } = event.target;
        let name=event.target.name;

        //Allow backspace
        if([8, 37, 39].indexOf(event.keyCode)>=0){
            return;
        }else if((!(event.keyCode >= 48 && event.keyCode <= 57) || (event.keyCode >= 96 && event.keyCode <= 105))){
            event.preventDefault();
        }

        let numeticValue = Number(value);
        let numericMax=Number(max);
        console.log(numeticValue, numericMax,  numeticValue>numericMax);

        if(numeticValue>numericMax){
            event.preventDefault();
        }
    }

    render() {

        let saveEnabled = false;
        if (this.state.dataReceived) {

            this.fields.map((field)=>{
                if(this.state[field] !== this.state[`${field}_initial`]){
                    saveEnabled=true;
                    return;
                }
            })
        }

        return (
            <div>
                <p>
                    <h2>Configuration</h2>
                    {(!this.state.dataReceived) ? <><Spinner /> loading contigurations...</> : null}
                    {(this.state.dataSaving) ? <><Spinner /> saving contigurations...</> : null}
                    {(this.state.status) ? <h4 >{this.state.status}</h4> : null}
                    {(this.state.error) ? <h4 style={{ "fontColor": "red" }}>{this.state.error}</h4> : null}
                </p>
                <Form>
                    <p>
                        <label htmlFor={this.controls.useNTP.name}>
                            <b>{this.controls.useNTP.label}</b>:
                                </label>
                        <select id={this.controls.useNTP.name} name={this.controls.useNTP.name}
                            onChange={(e) => { this.setState({ useNTP: Number(e.target.value) }) }} >
                            {
                                this.controls.useNTP.options.map((option) => {
                                    let selected = (option.value === this.state.useNTP) ? "selected" : "";
                                    return <option value={option.value} selected={selected}>{option.text}</option>;
                                })
                            }
                        </select>
                    </p>
                    <p>
                        <label htmlFor={this.controls.operationMode.name}>
                            <b>{this.controls.operationMode.label}</b>:
                                </label>
                        <select id={this.controls.operationMode.name} name={this.controls.operationMode.name}
                            onChange={(e) => { this.setState({ operationMode: Number(e.target.value) }) }} >
                            {
                                this.controls.operationMode.options.map((option) => {
                                    let selected = (option.value === this.state.operationMode) ? "selected" : "";
                                    return <option value={option.value} selected={selected}>{option.text}</option>;
                                })
                            }
                        </select>
                    </p>
                    {this.state.operationMode === 1 ?
                        <>
                            <p>
                                <label htmlFor={this.controls.serverIp.name}>
                                    <b>{this.controls.serverIp.label}</b>:
                                </label>
                                <MaskedInput id={this.controls.serverIp.name} name={this.controls.serverIp.name} type="text" className="input_small"
                                    onChange={(e) => { this.setState({ serverIp: e.target.value.trim() }) }} value={this.state.serverIp} {...ipProps} />
                            </p>
                            <p>
                                <label htmlFor={this.controls.serverPort.name}>
                                    <b>{this.controls.serverPort.label}</b>:
                                </label>
                                <input id={this.controls.serverPort.name} name={this.controls.serverPort.name} type="text" className="input_small"
                                    onChange={(e) => { this.setState({ serverPort: Number(e.target.value) }) }} value={this.state.serverPort} />
                            </p>
                            <p>
                                <label htmlFor={this.controls.powerThresholdHigh.name}>
                                    <b>{this.controls.powerThresholdHigh.label}</b>:
                                </label>
                                <input id={this.controls.powerThresholdHigh.name} name={this.controls.powerThresholdHigh.name} type="text" className="input_small"
                                    onChange={(e) => { this.setState({ powerThresholdHigh: Number(e.target.value) }) }} value={this.state.powerThresholdHigh} />
                            </p>

                            <p>
                                <label htmlFor={this.controls.powerThresholdLow.name}>
                                    <b>{this.controls.powerThresholdLow.label}</b>:
                                </label>
                                <input id={this.controls.powerThresholdLow.name} name={this.controls.powerThresholdLow.name} type="text" className="input_small"
                                    onChange={(e) => { this.setState({ powerThresholdLow: Number(e.target.value) }) }} value={this.state.powerThresholdLow} />
                            </p>
                            <p>
                                <label htmlFor={this.controls.measureInterval.name}>
                                    <b>{this.controls.measureInterval.label}</b>:
                                </label>
                                <input id={this.controls.measureInterval.name} name={this.controls.measureInterval.name} type="text" className="input_small" min="5" max="600"
                                    onChange={this.onNumericChange} onKeyDown={this.onNumericInput} value={this.state.measureInterval} />
                            </p>
                        </> : null}

                    <p>
                        <label htmlFor={this.controls.enableStatusLED.name}>
                            <b>{this.controls.enableStatusLED.label}</b>:
                                </label>
                        <select id={this.controls.enableStatusLED.name} name={this.controls.enableStatusLED.name}
                            onChange={(e) => { this.setState({ enableStatusLED: Number(e.target.value) }) }} >
                            {
                                this.controls.enableStatusLED.options.map((option) => {
                                    let selected = (option.value === this.state.enableStatusLED) ? "selected" : "";
                                    return <option value={option.value} selected={selected}>{option.text}</option>;
                                })
                            }
                        </select>
                    </p>
                    <p>
                        {saveEnabled ?
                            <Button id="saveButton" onClick={this.handleSave} >SAVE</Button>
                            :
                            <DisabledButton id="saveButton">SAVE</DisabledButton>
                        }
                    </p>
                </Form>
            </div >
        );
    }
}

ConfigPage.propTypes = {
    API: PropTypes.string,
};
