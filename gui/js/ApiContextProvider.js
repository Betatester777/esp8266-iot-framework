import React, { Children } from 'react';
import PropTypes from 'prop-types';
import Setup from "./setup.json";
import StateMachine from 'javascript-state-machine';
import i18nManager from "./i18n.js";

export const ApiContext = React.createContext();
export const ApiContextConsumer = ApiContext.Consumer;

const StatusType = Object.freeze({
    info: "info",
    error: "error",
    success: "success"
});

let url = "http://192.168.4.1";
url = "http://192.168.178.39";

if (process.env.NODE_ENV === "production") { url = window.location.origin; }

if (process.env.NODE_ENV === "development") {
    require("preact/debug");
}

export class ApiContextProvider extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            scope: null,
            isConnected: false,
            isLoaded: false,
            isModified: false,
            isSaved: false,
            statusText: null,
            statusType: StatusType.info,
        };

        this.SetupScope = Object.freeze({
            legal: "legal",
            wifi: "wifi",
            server: "server",
            server_test: "server_test",
            time: "time",
            timer: "timer",
            settings: "settings",
            complete: "complete",
        });

        this.productName = "WiFiSolarPlug";
        this.i18n = new i18nManager();

        this.url = url;

        this.api = Object.freeze({
            legal: {
                url: `${url}/api/setup/legal`,
                fields: [
                    "userAgreementAccepted"
                ]
            },
            wifi: {
                url: `${url}/api/setup/wifi`,
                fields: [
                    "ssid",
                    "password",
                    "useDHCP",
                    "fixedIp",
                    "subnetMask",
                    "dnsServerIp",
                    "gatewayIp",
                    "mdnsName",
                ]
            },
            server: {
                url: `${url}/api/setup/server`,
                fields: [
                    "serverProductId",
                    "serverHost",
                    "serverPort",
                    "measureInterval",
                ]
            },
            server_test: {
                url: `${url}/api/setup/server_test`,
                fields: [
                    "serverTestIsComplete",
                    "serverTestConnectionStatus",
                    "serverTestRequestStatus",
                    "serverTestResponseValue"
                ]
            },
            settings: {
                url: `${url}/api/setup/settings`,
                fields: [
                    "powerThresholdHigh",
                    "powerThresholdLow"
                ]
            },
            complete: {
                url: `${url}/api/setup/complete`,
                fields: []
            }
        });

        this.controls = {};

        Object.keys(this.api).map((apiKey) => {
            let api = this.api[apiKey];
            api.fields.map((field) => {
                this.controls[field] = Setup.find(obj => { return obj.name === field; });
                if (this.controls[field]) {
                    this.controls[field].translation = this.i18n.get(this.controls[field].label);
                    this.controls[field].translation_info = this.i18n.get(this.controls[field].label + ".info");
                }
                return;
            });
        });

        this.machine = new StateMachine({
            init: 'init',
            transitions: [
                { name: 'load', from: 'init', to: 'loading' },
                { name: 'load', from: 'saved', to: 'loading' },
                { name: 'load', from: 'load_failed', to: 'loading' },
                { name: 'load', from: 'ready', to: 'loading' },
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
                                isLoaded: false,
                                isModified: false,
                                isSaved: false,
                                statusType: StatusType.info,
                                statusText: "setup.status.loading",
                            });
                            break;
                        case 'ready':
                            if (this.state.isSaved) {
                                this.setState({
                                    isLoaded: true,
                                    statusType: StatusType.success,
                                    statusText: "setup.status.save.success",
                                });
                            } else {
                                this.setState({
                                    isLoaded: true,
                                    statusType: StatusType.info,
                                    statusText: null,
                                });
                            }
                            break;
                        case 'load_failed':
                            this.setState({
                                statusType: StatusType.error,
                                statusText: "setup.status.load.failed"
                            });
                            break;
                        case 'saved':
                            this.setState({
                                statusType: StatusType.success,
                                statusText: "setup.status.save.success",
                                isSaved: true,
                            }, () => {
                                if (this.state.scope !== this.SetupScope.wifi) {
                                    //Skip on WIFI edit because of reconnect
                                    this.loadStatus();
                                }
                            });
                            break;
                        case 'save_failed':
                            this.setState({
                                statusType: StatusType.error,
                                statusText: "setup.status.save.failed"
                            });
                            break;
                    }
                }
            }
        });
    }

    componentDidMount() {
        this.loadStatus();
    }

    loadStatus = () => {
        let statusUrl = `${url}/api/setup/status/get`;
        console.info(`Loading status [url=${statusUrl}]`);
        fetch(statusUrl)
            .then(response => response.json())
            .then((setupStatus) => {
                console.info(`Loaded status [status=${JSON.stringify(setupStatus)}]`);
                if (!setupStatus.legal) {
                    this.setState({ scope: this.SetupScope.legal }, () => {
                        this.load();
                    });
                } else if (!setupStatus.wifi) {
                    this.setState({ scope: this.SetupScope.wifi }, () => {
                        this.load();
                    });
                } else if (!setupStatus.server) {
                    this.setState({ scope: this.SetupScope.server }, () => {
                        this.load();
                    });
                } else if (!setupStatus.server_test) {
                    this.setState({ scope: this.SetupScope.server_test }, () => {
                        this.load();
                    });
                } else if (!setupStatus.settings) {
                    this.setState({ scope: this.SetupScope.settings }, () => {
                        this.load();
                    });
                }else{
                    this.setState({ scope: this.SetupScope.complete }, () => {
                        this.load();
                    });
                }

                this.setState({isConnected : true});
            })
            .catch((error) => {
                // handle your errors here
                console.error(error);
            });
    }

    setStateExtern = (data, onDone) => {
        if (onDone) {
            this.setState({ ...data }, onDone);
        } else {
            this.setState({ ...data });
        }

    }

    load = () => {
        this.machine.load();
        let scopeConfig = this.api[this.state.scope];
        console.info(`Loading data [url=${scopeConfig.url}/get]`);
        fetch(`${scopeConfig.url}/get`)
            .then(response => response.json())
            .then((responseData) => {
                console.info(`Loaded data [data=${JSON.stringify(responseData)}]`);
                let data = {}
                scopeConfig.fields.map((field) => {
                    data[field] = responseData[field];
                    data[`${field}_initial`] = data[field];
                    return;
                });

                this.setState(data);
                this.machine.loadSuccess();
            })
            .catch((error) => {
                // handle your errors here
                console.error(error);
                this.machine.loadFail();
            });
    }

    save = () => {
        this.machine.save();
        let scopeConfig = this.api[this.state.scope];
        let data = new FormData();

        let payload = {};
        scopeConfig.fields.map((field) => {
            payload[field] = this.state[field];
            return;
        });

        data.append("data", JSON.stringify(payload));
        console.info(`Saving data [url=${scopeConfig.url}/set, data=${JSON.stringify(payload)}]`);
        fetch(`${scopeConfig.url}/set`,
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

    render() {
        return (
            <ApiContext.Provider
                value={{
                    controls: this.controls,
                    state: this.state,
                    setState: this.setStateExtern,
                    i18n: this.i18n,
                    load: this.load,
                    save: this.save,
                }}
            >
                {Children.only(this.props.children)}
            </ApiContext.Provider>
        );
    }
}