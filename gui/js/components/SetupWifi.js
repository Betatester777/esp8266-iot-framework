import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, Checkbox, IPv4, TextInput, PasswordInput } from "./UiComponents";

let isAndroid = () => {
    return navigator.userAgent.match(/Android/i);
}

function validateIPaddress(ipaddress) {
    if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) {
        return true;
    }
    return false;
}

export default class SetupWifi extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;
        this.state = {
            isSaveEnabled: false
        };
    }

    onChangeValue = (newState) => {
        console.info(`Value changed: [${JSON.stringify(newState)}]`);
        this.props.context.setState(newState);
    }

    openDeviceUrl = () => {
        if (isAndroid) {
            window.location.replace(`http://${this.props.context.state.mdnsName}/`);
        } else {
            window.location.replace(`http://${this.props.context.state.mdnsName}.local/`);
        }
    }

    render() {
        console.log(this.props.context.state);
        let isSaveEnabled = false;

        let apiState = this.props.context.state;
        let content = <>
            <Spinner />
            {this.i18n.get("common.status.connecting")}
        </>;

        if (apiState.isSaved) {
            let pcLink = `<a href="http://${apiState.mdnsName}.local/">http://${apiState.mdnsName}.local/</a>`;
            let androidLink = `<a href="http://${apiState.mdnsName}/">http://${apiState.mdnsName}</a>`;

            content = <div>
                <h2>{this.i18n.get("setup.wifi_status.content.title")}</h2>
                <p>{this.i18n.get("setup.wifi_status.content.text", [["{url}", pcLink], ["{android_url}", androidLink]])}</p>
                <Form>
                    <p>
                        <Button name="buttonReconnect" title={this.i18n.get("setup.wifi_status.control.apply")} onClick={this.openDeviceUrl} isDisabled={false} />
                    </p>
                </Form>
            </div>;
        } else if (apiState.isLoaded) {
            if (apiState.ssid.length > 3 && apiState.ssid.length < 255 &&
                apiState.password.length > 3 && apiState.password.length < 255
            ) {
                if (apiState.useDHCP) {
                    isSaveEnabled = true;
                } else {
                    if (validateIPaddress(apiState.fixedIp) && validateIPaddress(apiState.subnetMask)) {
                        isSaveEnabled = true;
                    }
                }
            }

            content = <div>
                <h2>{this.i18n.get("setup.wifi.content.title")}</h2>
                <Form>
                    <TextInput control={this.controls.ssid} state={apiState} onChangeValue={this.onChangeValue} showInfo={true} />
                    <PasswordInput control={this.controls.password} state={apiState} onChangeValue={this.onChangeValue} showInfo={true} />
                    <Checkbox control={this.controls.useDHCP} state={apiState} onChangeValue={this.onChangeValue} />
                    <IPv4 control={this.controls.fixedIp} state={apiState} onChangeValue={this.onChangeValue} />
                    <IPv4 control={this.controls.subnetMask} state={apiState} onChangeValue={this.onChangeValue} />
                    <IPv4 control={this.controls.dnsServerIp} state={apiState} onChangeValue={this.onChangeValue} />
                    <IPv4 control={this.controls.gatewayIp} state={apiState} onChangeValue={this.onChangeValue} />
                    <p>
                        <Button name="buttonSave" title={this.i18n.get("setup.wifi.control.save")} onClick={this.props.context.save} isDisabled={!isSaveEnabled} />
                    </p>
                </Form>
            </div>
        }

        return (
            <div>
                {content}
            </div>
        );
    }
}
