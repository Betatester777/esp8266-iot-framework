import React, { Children } from 'react';
import PropTypes from 'prop-types';
import StateMachine from 'javascript-state-machine';
import i18nManager from "./i18n.js";
import Fields from "./Fields.json";

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
            wsInit: false,
            scope: null,
            isConnected: false,
            isLoaded: false,
            isModified: false,
            isSaved: false,
            statusText: null,
            statusType: StatusType.info,
        };

        this.webSocketUrl = url.replace("http://", "ws://").concat("/ws");

        this.SetupScope = Object.freeze({
            legal: "legal",
            wifi: "wifi",
            wifi_test: "wifi_test",
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
                ]
            },
            wifi_test: {
                url: `${url}/api/setup/wifi_test`,
                fields: [
                    "wifiTestIsComplete",
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
            status: {
                fields: [
                    "outputStatus",
                    "operationMode",
                    "measuredPower"
                ]
            }
        });

        this.config = null;
        this.checkConnectionInterval = null;

        this.controls = {};

        Object.keys(this.api).map((apiKey) => {
            let api = this.api[apiKey];
            api.fields.map((field) => {
                this.controls[field] = Fields.find(obj => { return obj.name === field; });
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
                { name: 'load', from: 'loading', to: 'loading' },
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
                                    //Relad status
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

    checkWsConnection = () => {
        if (!this.ws || this.ws.readyState == WebSocket.CLOSED) {
            this.wsConnect(); //check if websocket instance is closed, if so call `connect` function.
        } else if (this.ws && this.ws.readyState == WebSocket.OPEN) {
            this.lastPingValue = `${new Date().getTime()}`;
            let payload = { command: "ping", value: this.lastPingValue };
            this.wsSend(payload);

            clearTimeout(this.keepAliveCheckTimeout);

            this.keepAliveCheckTimeout = setTimeout(() => {
                if (this.lastPingValue !== this.lastPongValue) {
                    console.log("ws failed [error=missed keepalive response]");
                    this.setState({ isConnected: false });
                    this.ws.close();
                    this.machine.loadFail();
                }
            }, 500);
        }
    };

    getStatus=()=>{
        let payload = { command: "get_status" };
        this.wsSend(payload);
    }

    wsConnect = () => {
        this.ws = new WebSocket(this.webSocketUrl);

        this.ws.onopen = () => {
            console.log("ws status changed [status=connected]");
            this.machine.load();
        }

        this.ws.onmessage = this.onWsMessage;

        this.ws.onerror = (error) => {
            console.log(`ws failed [error=${error}]`);
            clearTimeout(this.keepAliveCheckTimeout);
            this.ws.close();
            this.machine.loadFail();
        }

        this.ws.onclose = (error) => {
            this.setState({ isConnected: false });
            clearTimeout(this.keepAliveCheckTimeout);
            console.error(`ws status changed [status=closed, reson=${error.reason}]`);
        }
    }

    componentDidMount() {
        console.log("didmount")
        if (!this.wsInit) {
            this.wsConnect(url.replace("http://", "ws://").concat("/ws"));
            this.checkConnectionInterval = setInterval(this.checkWsConnection, 1000);
        }
    }

    onWsMessage = (event) => {
        let lastScope=null;
        console.log(`ws receive [message=${event.data}]`);
        const data = JSON.parse(event.data);
        if (!data.hasOwnProperty("type")) {
            return;
        }

        switch (data.type) {
            case "pong":
                this.lastPongValue = data.value;
                break;
            case "status":
                if(this.machine.is("saving")) {
                    console.log("skip loading:", this.machine.state)
                    return;
                }
                let scopes = [
                    this.SetupScope.legal,
                    this.SetupScope.wifi,
                    this.SetupScope.wifi_test,
                    this.SetupScope.server,
                    this.SetupScope.server_test,
                    this.SetupScope.settings
                ];

                this.setState({deviceName: data["deviceName"]});

                for (let i = 0; i < scopes.length; i++) {
                    let scope = scopes[i];
                    if (data[scope] !== 1) {
                        console.log("######set scope:", scope);
                        lastScope=this.state.scope;
                        this.setState({ scope: scope, isConnected: true, stamp: new Date().getTime() }, () => {
                            if (this.state.scope !== lastScope) {
                                console.log("######set scope done:", this.state.scope)
                                this.config = this.api[this.state.scope];
                                this.load();
                            }
                        });
                        return;
                    }
                }
                lastScope=this.state.scope;
                this.setState({ scope: this.SetupScope.complete, isConnected: true, stamp: new Date().getTime() }, () => {
                    console.log("######set scope done:", this.state.scope)
                    this.config = this.api["status"];
                    this.machine.load();

                    let newData = {}
                    this.config.fields.map((field) => {
                        newData[field] = data[field];
                        newData[`${field}_initial`] = newData[field];
                        return;
                    });    
                    this.setState(newData);

                    this.machine.loadSuccess();
                });

                break;
        }
    }

    componentWillUnmount() {
        clearTimeout(this.keepAliveCheckTimeout);
        clearInterval(this.checkConnectionInterval);
    }

    setStateFromChild = (data, onDone) => {
        if (onDone) {
            this.setState({ ...data }, onDone);
        } else {
            this.setState({ ...data });
        }
    }

    loadPreviousScope = () => {
        this.machine.save();
        let data = new FormData();

        let payload = {};

        let previousScopeUrl = `${url}/api/setup/previous_scope`;

        data.append("data", JSON.stringify(payload));
        console.info(`Loading previous scope data [url=${previousScopeUrl}/previous_scope/set, data=${JSON.stringify(payload)}]`);
        fetch(`${previousScopeUrl}/set`,
            {
                method: "post",
                body: data
            }).then((response) => {
                console.info("status:", response.status);
                if (response.status == 200) {
                    this.machine.saveSuccess();
                    this.getStatus();
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

    load = () => {
        this.machine.load();
        console.info(`Loading data [url=${this.config.url}/get]`);
        fetch(`${this.config.url}/get`)
            .then(response => response.json())
            .then((responseData) => {
                console.info(`Loaded data [data=${JSON.stringify(responseData)}]`);
                let data = {}
                this.config.fields.map((field) => {
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
        let data = new FormData();

        let payload = {};
        this.config.fields.map((field) => {
            payload[field] = this.state[field];
            return;
        });

        data.append("data", JSON.stringify(payload));
        console.info(`Saving data [url=${this.config.url}/set, data=${JSON.stringify(payload)}]`);
        fetch(`${this.config.url}/set`,
            {
                method: "post",
                body: data
            }).then((response) => {
                console.info("status:", response.status);
                if (response.status == 200) {
                    this.machine.saveSuccess();
                    this.getStatus();
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

    setOperationMode=(operationMode)=>{
        let payload = {command: "set_operation_mode", operationMode:operationMode };
        this.wsSend(payload);
    }

    setOutputStatus=(outputStatus)=>{
        let payload = {command: "set_output_status", outputStatus: outputStatus};
        this.wsSend(payload);
    }

    wsSend=(payload)=>{
        console.log(`ws send [payload=${JSON.stringify(payload)}]`);
        this.ws.send(JSON.stringify(payload));
    }

    render() {
        console.log("#########render", this.state.isConnected)
        return (
            <ApiContext.Provider
                value={{
                    controls: this.controls,
                    state: this.state,
                    setState: this.setStateFromChild,
                    i18n: this.i18n,
                    load: this.load,
                    save: this.save,
                    setOperationMode: this.setOperationMode,
                    setOutputStatus: this.setOutputStatus,
                    loadPreviousScope: this.loadPreviousScope,
                }}
            >
                {Children.only(this.props.children)}
            </ApiContext.Provider>
        );
    }
}