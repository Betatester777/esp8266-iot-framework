import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, TestStatus } from "./UiComponents";

export default class SetupServerTest extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;

        this.state = {

        };
    }

    onChangeValue = (newState) => {
        console.info(`Value changed: [${JSON.stringify(newState)}]`);
        this.props.context.setState(newState);
    }

    onSave=(e)=>{
        this.props.context.setState({serverTestIsComplete: 1}, this.props.context.save);
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
            let connectionStatus = null;
            let requestStatus = null;
            let responseValue = null;

            if (apiState.serverTestConnectionStatus) {
                connectionStatus = <TestStatus name="setup.server_test.control.connection_status" i18n={this.i18n} />;
                if (apiState.serverTestRequestStatus) {
                    requestStatus = <TestStatus name="setup.server_test.control.request_status" i18n={this.i18n} />;
                    if (apiState.serverTestResponseValue !== null) {
                        responseValue = <TestStatus name="setup.server_test.control.response_value" i18n={this.i18n} value={apiState.serverTestResponseValue} />;
                    }
                } else {
                    requestStatus = <TestStatus name="setup.server_test.control.request_status" i18n={this.i18n} isFailed={true} />;
                }
            } else {
                connectionStatus = <TestStatus name="setup.server_test.control.connection_status" i18n={this.i18n} isFailed={true} />;
            }

            return (
                <div>
                    <h2>{this.i18n.get("setup.server_test.content.title")}</h2>
                    <Form>
                        {connectionStatus}
                        {requestStatus}
                        {responseValue}
                        <p>
                            <Button name="buttonRepeat" title={this.i18n.get("setup.server_test.control.repeat")} onClick={this.props.context.load} isDisabled={!isSaveEnabled} marginRight={true} />
                            <Button name="buttonSave" title={this.i18n.get("setup.server_test.control.save")} onClick={this.onSave} isDisabled={!isSaveEnabled} />
                        </p>
                    </Form>
                </div>
            );
        }
    }
}
