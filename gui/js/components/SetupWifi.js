import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, Checkbox, IPv4, TextInput, PasswordInput } from "./UiComponents";

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

    render() {
        console.log(this.props.context.state);
        let isSaveEnabled = false;

        let apiState = this.props.context.state;

        if(apiState.isLoaded){
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
        }


        return (
            <div>
                <div>
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
            </div>
        );
    }
}
