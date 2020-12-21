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

        this.controls = {
            useNTP: Config.find(obj => { return obj.name === "useNTP"; }),
            operationMode: Config.find(obj => { return obj.name === "operationMode"; }),
            serverIp: Config.find(obj => { return obj.name === "serverIp"; }),
            powerThreshold: Config.find(obj => { return obj.name === "powerThreshold"; }),
            measureInterval: Config.find(obj => { return obj.name === "measureInterval"; }),
            enableStatusLED: Config.find(obj => { return obj.name === "enableStatusLED"; }),
        }

        this.fetchData = this.fetchData.bind(this);
        this.handleSave = this.handleSave.bind(this);
    }

    fetchData() {
        this.setState({ error: null, status: null });
        fetch(`${this.props.API}/api/config/get`)
            .then(response => response.json())
            .then((configData) => {
                this.setState({
                    dataReceived: true,
                    dataSaving: false,
                    useNTP_initial: configData.useNTP,
                    operationMode_initial: configData.operationMode,
                    serverIp_initial: configData.serverIp,
                    powerThreshold_initial: configData.powerThreshold,
                    measureInterval_initial: configData.measureInterval,
                    enableStatusLED_initial: configData.enableStatusLED,
                    useNTP: configData.useNTP,
                    operationMode: configData.operationMode,
                    serverIp: configData.serverIp,
                    powerThreshold: configData.powerThreshold,
                    measureInterval: configData.measureInterval,
                    enableStatusLED: configData.enableStatusLED,
                }
                );
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
        data.append("data", JSON.stringify({
            useNTP: this.state.useNTP,
            operationMode: this.state.operationMode,
            serverIp: this.state.serverIp,
            powerThreshold: this.state.powerThreshold,
            measureInterval: this.state.measureInterval,
            enableStatusLED: this.state.enableStatusLED,

        }));

        console.log(data);

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

    render() {

        let saveEnabled = false;
        if (this.state.dataReceived) {
            saveEnabled = !(this.state.useNTP === this.state.useNTP_initial &&
                this.state.operationMode === this.state.operationMode_initial &&
                this.state.serverIp === this.state.serverIp_initial &&
                this.state.powerThreshold === this.state.powerThreshold_initial &&
                this.state.measureInterval === this.state.measureInterval_initial &&
                this.state.enableStatusLED === this.state.enableStatusLED_initial
            );
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
                        <><p>
                            <label htmlFor={this.controls.serverIp.name}>
                                <b>{this.controls.serverIp.label}</b>:
                            </label>
                            <MaskedInput id={this.controls.serverIp.name} name={this.controls.serverIp.name} className="input_small"
                                onChange={(e) => { this.setState({ serverIp: e.target.value.trim() }) }} value={this.state.serverIp} {...ipProps} />
                        </p>
                            <p>
                                <label htmlFor={this.controls.powerThreshold.name}>
                                    <b>{this.controls.powerThreshold.label}</b>:
                                </label>
                                <input id={this.controls.powerThreshold.name} name={this.controls.powerThreshold.name} type="text" className="input_small"
                                    onChange={(e) => { this.setState({ powerThreshold: Number(e.target.value) }) }} value={this.state.powerThreshold} />
                            </p>
                            <p>
                                <label htmlFor={this.controls.measureInterval.name}>
                                    <b>{this.controls.measureInterval.label}</b>:
                                </label>
                                <input id={this.controls.measureInterval.name} name={this.controls.measureInterval.name} type="text" className="input_small"
                                    onChange={(e) => { this.setState({ measureInterval: Number(e.target.value) }) }} value={this.state.measureInterval} />
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
