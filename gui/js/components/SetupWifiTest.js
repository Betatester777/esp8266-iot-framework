import React from "react";
import PropTypes from "prop-types";
import Setup from "../setup.json";
import { Form, Button, Spinner, Checkbox, TextInput, PasswordInput, IPv4, Status } from "./UiComponents";
import StateMachine from 'javascript-state-machine';

const StatusType = Object.freeze({
    info: "info",
    error: "error",
    success: "success"
});

const ScopeType = Object.freeze({
    legal: "legal",
    wifi: "wifi",
    wifi_status: "wifi_status"
});

let isAndroid = () => {
    return navigator.userAgent.match(/Android/i);
 }

function validateIPaddress(ipaddress) {
    if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) {
        return true;
    }
    return false;
}

export class SetupWifi extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            showLoadingIndicator: null,
            scope: ScopeType.legal,
            statusText: null,
            statusType: StatusType.info,
            buttonApplyLegalEnabled: false,
            buttonApplyWifiEnabled: false,
            buttonCompleteSetup: false,
        };

        this.fields = Object.freeze([
            "userAgreementAccepted",
            "ssid",
            "password",
            "useDHCP",
            "fixedIp",
            "subnetMask",
            "dnsServerIp",
            "gatewayIp",
            "mdnsName",
        ]);

        this.controls = {};

        this.fields.map((field) => {
            this.controls[field] = Setup.find(obj => { return obj.name === field; });
            return;
        });

        this.onChangeValue=this.onChangeValue.bind(this);

        this.machine = new StateMachine({
            init: 'init',
            transitions: [
                { name: 'load', from: 'init', to: 'loading' },
                { name: 'load', from: 'saved', to: 'loading' },
                { name: 'load', from: 'load_failed', to: 'loading' },
                { name: 'load_success', from: 'loading', to: 'ready' },
                { name: 'load_fail', from: 'loading', to: 'load_failed' },
                { name: 'save', from: 'ready', to: 'saving' },
                { name: 'save', from: 'save_failed', to: 'saving' },
                { name: 'save_success', from: 'saving', to: 'saved' },
                { name: 'save_fail', from: 'saving', to: 'save_failed' },
            ],
            methods: {
                onEnterState: (lifecycle) => {
                    console.info(lifecycle.to);
                    switch (lifecycle.to) {
                        case 'loading':
                            this.setState({
                                showLoadingIndicator: true,
                                statusType: StatusType.info,
                                statusText: "loading settings"
                            });
                            break;
                        case 'ready':
                            if (this.state.isSaved) {
                                this.setState({
                                    showLoadingIndicator: false,
                                    statusType: StatusType.success,
                                    statusText: "configuration was saved!",
                                    isSaved: false
                                });
                            } else {
                                this.setState({
                                    showLoadingIndicator: false,
                                    statusType:
                                        StatusType.info,
                                    statusText: null,
                                    isSaved: false
                                });
                            }
                            break;
                        case 'load_failed':
                            this.setState({
                                showLoadingIndicator: true,
                                statusType: StatusType.error,
                                statusText: "something went wrong!"
                            });
                            break;
                        case 'saved':
                            this.setState({
                                showLoadingIndicator: false,
                                statusType: StatusType.success,
                                statusText: "saved!",
                                isSaved: true,
                                scope: ScopeType.wifi_status,
                                buttonCompleteSetup: true
                            });
                            break;
                        case 'save_failed':
                            this.setState({
                                showLoadingIndicator: false,
                                statusType: StatusType.error,
                                statusText: "save changes failed"
                            });
                            break;
                    }
                }
            }
        });
    }

    back = (scope) => {
        this.setState({ scope: scope })
    }

    applyLegal = () => {
        this.setState({ scope: ScopeType.wifi })
    }

    openDeviceUrl = () => {
        if(isAndroid){
            window.location.replace(`http://${this.state.mdnsName}/`);
        }else{
            window.location.replace(`http://${this.state.mdnsName}.local/`);
        }
    }

    fetchData=()=> {
        this.machine.load();
        fetch(`${this.props.API}/get`)
            .then(response => response.json())
            .then((configData) => {
                let newState = {}
                this.fields.map((field) => {
                    newState[field] = configData[field];
                    newState[`${field}_initial`] = configData[field];
                    return;
                });

                this.onChangeValue(newState);
                this.machine.loadSuccess();
            })
            .catch((error) => {
                // handle your errors here
                console.error(error);
                this.machine.loadFail();
            })
    }

    save=(event)=>{
        event.preventDefault();

        this.machine.save();
        let data = new FormData();

        let payload = {};
        this.fields.map((field) => {
            payload[field] = this.state[field];
            return;
        });

        data.append("data", JSON.stringify(payload));

        fetch(`${this.props.API}/set`,
            {
                method: "post",
                body: data
            }).then((response) => {
                console.info("status:", response.status);
                if (response.status == 200) {
                    this.machine.saveSuccess();
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

    onChangeValue(newState) {
        this.setState(newState,
            () => {
                let enabled = false;
                switch (this.state.scope) {
                    case ScopeType.legal:
                        this.setState({ buttonApplyLegalEnabled: this.state.userAgreementAccepted });
                        break;
                    case ScopeType.wifi:
                        let ssid = this.state.ssid.trim();
                        let password = this.state.password.trim();
                        if (ssid.length > 3 && ssid.length < 255 &&
                            password.length > 3 && password.length < 255
                        ) {
                            if (this.state.useDHCP) {
                                enabled = true;
                            } else {
                                if (validateIPaddress(this.state.fixedIp) && validateIPaddress(this.state.subnetMask)) {
                                    enabled = true;
                                }
                            }

                        }
                        this.setState({ buttonApplyWifiEnabled: enabled });
                        break;
                }
            });
    }

    componentDidMount() {
        this.fetchData();
    }

    render() {
        return (

            <div>
                <Status type={this.state.statusType} text={this.state.statusText} showSpinner={this.state.showLoadingIndicator} />
                {this.state.scope === ScopeType.legal ? this.renderLegal() : null}
                {this.state.scope === ScopeType.wifi ? this.renderWifi() : null}
                {this.state.scope === ScopeType.wifi_status ? this.renderWifiStatus() : null}
            </div >
        );
    }


    renderLegal = () => {
        return (
            <div>
                <h2>{this.props.i18n.get("setup.legal.content.title")}</h2>
                <div className="scrollable-container">{this.props.i18n.get("setup.legal.content.agreement")}</div>
                <Form>
                    <Checkbox control={this.controls.userAgreementAccepted} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} />
                    <p>
                        <Button name="buttonApplyLegal" title={this.props.i18n.get("setup.legal.control.apply")} onClick={this.applyLegal} isDisabled={!this.state.buttonApplyLegalEnabled} />
                    </p>
                </Form>
            </div>
        );
    }

    renderWifi = () => {
        return (
            <div>
                <h2>{this.props.i18n.get("setup.wifi.content.title")}</h2>
                <Form>
                    <TextInput control={this.controls.ssid} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} showInfo={true} />
                    <PasswordInput control={this.controls.password} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} showInfo={true} />
                    <Checkbox control={this.controls.useDHCP} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} />
                    <IPv4 control={this.controls.fixedIp} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} />
                    <IPv4 control={this.controls.subnetMask} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} />
                    <IPv4 control={this.controls.dnsServerIp} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} />
                    <IPv4 control={this.controls.gatewayIp} i18n={this.props.i18n} state={this.state} setState={this.onChangeValue} />
                    <p>
                        <Button name="buttonBack" title={this.props.i18n.get("setup.wifi.control.back")} onClick={this.back} onClick={(e) => { this.back(ScopeType.legal) }} marginRight={true} />
                        <Button name="buttonApplyWifi" title={this.props.i18n.get("setup.wifi.control.apply")} onClick={this.save} isDisabled={!this.state.buttonApplyWifiEnabled} />
                    </p>
                </Form>
            </div>
        );
    }

    renderWifiStatus = () => {
        let pcLink = `<a href="http://${this.state.mdnsName}.local/">http://${this.state.mdnsName}.local/</a>`;
        let androidLink = `<a href="http://${this.state.mdnsName}/">http://${this.state.mdnsName}</a>`;
        return (
            <div>
                <h2>{this.props.i18n.get("setup.wifi_status.content.title")}</h2>
                <p>{this.props.i18n.get("setup.wifi_status.content.text", [["{url}", pcLink], ["{android_url}", androidLink]])}</p>
                <Form>
                    <p>
                        <Button name="buttonCompleteSetup" title={this.props.i18n.get("setup.wifi_status.control.apply")} onClick={this.openDeviceUrl} isDisabled={!this.state.buttonCompleteSetup} />
                    </p>
                </Form>
            </div>
        );
    }
}

SetupWifi.propTypes = {
    i18n: PropTypes.object,
    API: PropTypes.string,
};
