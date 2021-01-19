import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, NumericInput } from "./UiComponents";
import { Wifi, CheckSquare, Server, Sliders, Zap, Check, Edit, Play } from "react-feather";

export default class ConfigPage extends React.Component {
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
                <h2>{this.i18n.get("config.content.title")}</h2>
                <div className="status-item">
                    <div>
                        <Wifi className="icon" />
                    </div>
                    <div>
                        {this.i18n.get("config.scope.wifi")}
                    </div>
                    <div>
                        <div className="icon-edit-button" onClick={() => alert(123)}><Edit className="icon-edit" /></div>
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Server className="icon" />
                    </div>
                    <div>
                        {this.i18n.get("config.scope.server")}
                    </div>
                    <div>
                        <div className="icon-edit-button" onClick={() => alert(123)}><Edit className="icon-edit" /></div>
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Zap className="icon" />
                    </div>
                    <div>
                        {this.i18n.get("config.scope.server_test")}
                    </div>
                    <div>
                        <div className="icon-edit-button" onClick={() => alert(123)}><Play className="icon-edit" /></div>
                    </div>
                </div>
                <div className="status-item">
                    <div>
                        <Sliders className="icon" />
                    </div>
                    <div>
                        {this.i18n.get("config.scope.settings")}
                    </div>
                    <div>
                        <div className="icon-edit-button" onClick={() => alert(123)}><Edit className="icon-edit" /></div>
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
