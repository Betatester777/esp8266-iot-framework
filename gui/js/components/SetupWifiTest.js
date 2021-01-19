import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, NumericInput } from "./UiComponents";

let isAndroid = () => {
    return navigator.userAgent.match(/Android/i);
}
export default class SetupWifiTest extends React.Component {
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

    onSave = (e) => {
        this.props.context.setState({ wifiTestIsComplete: 1 }, ()=>{
            this.props.context.save();
        
            if (isAndroid) {
                window.location.replace(`http://${this.props.context.state.deviceName}/`);
            } else {
                window.location.replace(`http://${this.props.context.state.deviceName}.local/`);
            }
        });
    }

    render() {
        console.log(this.props.context.state);
        let isSaveEnabled = false;

        let apiState = this.props.context.state;

        let pcLink = `<a href="http://${apiState.deviceName}.local/">http://${apiState.deviceName}.local/</a>`;
        let androidLink = `<a href="http://${apiState.deviceName}/">http://${apiState.deviceName}</a>`;

        return (
            <div>
                <div>
                    <h2>{this.i18n.get("setup.wifi_test.content.title")}</h2>
                    <p>{this.i18n.get("setup.wifi_test.content.text", [["{url}", pcLink], ["{android_url}", androidLink]])}</p>
                    <Form>
                        <p>
                            <Button name="buttonSave" title={this.i18n.get("setup.wifi_test.control.save")} onClick={this.onSave} isDisabled={false} />
                        </p>
                    </Form>
                </div>
            </div>
        );
    }
}
