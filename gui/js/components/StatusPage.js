import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, NumericInput } from "./UiComponents";
import { Wifi, CheckSquare, Server, Sliders, Zap, Check } from "react-feather";

export default class StatusPage extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;
    }

    onChangeValue = (newState) => {
        console.info(`Value changed: [${JSON.stringify(newState)}]`);
        this.props.context.setState(newState);
    }

    render() {
        console.log(this.props.context.state);
        let isSaveEnabled = true;

        let apiState = this.props.context.state;
        let content = <>
            <Spinner />
            {this.i18n.get("common.status.connecting")}
        </>;

        if (apiState.isLoaded) {

            content = <div>
                <h2>{this.i18n.get("status.content.title")}</h2>
                <div className="status-item">
                    <div>
                        <CheckSquare className="icon" />
                    </div>
                    <div>
                        Nutzungsvereinbarung
                    </div>
                    <div>
                        <Check className="icon-done" />
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Wifi className="icon" />
                    </div>
                    <div>
                        WLAN-Konfiguration
                    </div>
                    <div>
                        <Check className="icon-done" />
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Server className="icon" />
                    </div>
                    <div>
                        Inverter-Verbindung
                    </div>
                    <div>
                        <Check className="icon-done" />
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Zap className="icon" />
                    </div>
                    <div>
                        Verbindungstest
                    </div>
                    <div>
                        <Check className="icon-done" />
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Sliders className="icon" />
                    </div>
                    <div>
                        Einschaltkriterien
                    </div>
                    <div>
                        <Check className="icon-done" />
                    </div>
                </div>

            </div>
        }

        return (
            <div>
                {content}
            </div>
        );
    }
}
