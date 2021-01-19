import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, TextInput, NumericInput, OptGroupSelect } from "./UiComponents";

export default class SetupServer extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;

        this.state = {
            showCustomPort: false
        };
    }

    toggleExtendedConfigurations = () => {
        let showCustomPort = !this.state.showCustomPort;
        if (!showCustomPort) {
            this.setState({ showCustomPort: showCustomPort });
            this.props.context.setState({ serverPort: 502 });
        } else {
            this.setState({ showCustomPort: showCustomPort });
        }
    }

    onChangeValue = (newState) => {
        console.info(`Value changed: [${JSON.stringify(newState)}]`);
        this.props.context.setState(newState);
    }

    render() {
        console.log(this.props.context.state);
        let isSaveEnabled = false;

        let apiState = this.props.context.state;
        if(apiState.isLoaded){
            if (apiState.serverHost.trim().length > 0 && apiState.serverPort > 0 && apiState.serverPort <= 65535) {
                isSaveEnabled = true;
            }
        }

        return (
            <div>
                <div>
                    <h2>{this.i18n.get("setup.server.content.title")}</h2>
                    <Form>
                        <OptGroupSelect control={this.controls.serverProductId} state={apiState} onChangeValue={this.onChangeValue} />
                        <TextInput control={this.controls.serverHost} state={apiState} onChangeValue={this.onChangeValue} />
                        <p>
                            <label for="showCustomPort">
                                <b>{this.i18n.get("setup.server.control.custom_port")}</b>
                            </label>
                            <input id="showCustomPort" name="showCustomPort" type="checkbox" onChange={this.toggleExtendedConfigurations} value={0} checked={this.state.showCustomPort} />
                        </p>
                        {
                            this.state.showCustomPort ?
                                <NumericInput control={this.controls.serverPort} state={apiState} onChangeValue={this.onChangeValue} />
                                : null
                        }
                        <p>
                            <Button name="buttonSave" title={this.i18n.get("setup.server.control.save")} onClick={this.props.context.save} isDisabled={!isSaveEnabled} />
                        </p>
                    </Form>
                </div>
            </div>
        );
    }
}
