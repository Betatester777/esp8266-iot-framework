import React from "react";
import ReactDOM from "react-dom";
import { render } from "preact";

import { BrowserRouter, Switch, Route, NavLink } from "react-router-dom";
import { Box } from "react-feather";
import { Menu, Header, Title, Hamburger, Spinner } from "./components/UiComponents";

import { ApiContextProvider, ApiContextConsumer } from "./ApiContextProvider";
import SetupLegal from "./components/SetupLegal";
import SetupWifi from "./components/SetupWifi";
import SetupServer from "./components/SetupServer";
import SetupServerTest from "./components/SetupServerTest";
import SetupSettings from "./components/SetupSettings";

import StatusPage from "./components/StatusPage";

import ProductInfo from "./components/ProductInfo";

import { WifiPage } from "./components/WifiPage";
import { ConfigPage } from "./components/ConfigPage";
import { FirmwarePage } from "./components/FirmwarePage";
import Logo from './logo.svg';
import "./styles.less";

import i18nManager from "./i18n.js";

const displayData = new Array();

const i18n = new i18nManager();

const productName = "SolarPlug";

i18n.setLocale("de-de");

class Root extends React.Component {

    constructor(props) {
        super(props);

        this.state = {
            menu: false,
            configData: null,
            setupScope: null,
        }

        //this.ws = new WebSocket(url.replace("http://", "ws://").concat("/ws"));
        //this.ws.addEventListener("message", this.onWsMessage);
    }

    componentDidMount() {

    }

    onWsMessage = (event) => {
        event.data.arrayBuffer().then((buffer) => {
            const dv = new DataView(buffer, 0);
            const timestamp = dv.getUint32(0, true);
            displayData.push([timestamp, bin2obj(buffer.slice(8, buffer.byteLength), [])]);
        });
    }

    toggleMenu = (e) => { this.setState({ menu: !this.state.menu }); }

    hideMenu = (e) => { this.setState({ menu: false }); }

    render() {
        let menuClass = this.state.menu ? "menu" : "menu menuHidden";

        let routeSwitch = null;
        let menu = null;
        let setupUI = null;

        return (
            <div>
                <BrowserRouter>
                    <Header>
                        <Title>
                            <span className="header-logo-container"><Logo className="header-logo" /></span>
                            <span className="header-text">{productName}</span>
                        </Title>
                        <ApiContextProvider>
                            <ApiContextConsumer>
                                {(context) => {
                                    if (!context.state.isConnected) {
                                        menu = <><Hamburger onClick={this.toggleMenu} />
                                        <Menu className={menuClass}>
                                            <li><NavLink onClick={this.hideMenu} exact to="/">{context.i18n.get("common.menu.status")}</NavLink></li>
                                            <li><NavLink onClick={this.hideMenu} exact to="/info">{context.i18n.get("common.menu.info")}</NavLink></li>
                                        </Menu></>;
                                    } else {
                                        if (context.state.scope === "complete") {
                                            menu = <><Hamburger onClick={this.toggleMenu} />
                                                <Menu className={menuClass}>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/">{context.i18n.get("common.menu.status")}</NavLink></li>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/config">{context.i18n.get("common.menu.config")}</NavLink></li>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/update">{context.i18n.get("common.menu.update")}</NavLink></li>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/info">{context.i18n.get("common.menu.info")}</NavLink></li>
                                                </Menu></>;
                                        } else {
                                            menu = <><Hamburger onClick={this.toggleMenu} />
                                                <Menu className={menuClass}>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/">{context.i18n.get("common.menu.setup")}</NavLink></li>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/update">{context.i18n.get("common.menu.update")}</NavLink></li>
                                                    <li><NavLink onClick={this.hideMenu} exact to="/info">{context.i18n.get("common.menu.info")}</NavLink></li>
                                                </Menu></>;
                                        }
                                    }

                                    return menu;
                                }}
                            </ApiContextConsumer>
                        </ApiContextProvider>
                    </Header>

                    <div className="page">
                        <ApiContextProvider>
                            <ApiContextConsumer>
                                {(context) => {
                                    if (!context.state.isConnected) {
                                        routeSwitch = <Switch>

                                            <Route exact path="/info"><ProductInfo context={context} /></Route>
                                            <Route path="/">
                                                <>
                                                    <Spinner />
                                                    {context.i18n.get("common.status.connecting")}
                                                </>
                                            </Route>
                                        </Switch>;
                                    } else {
                                        switch (context.state.scope) {
                                            case "legal":
                                                setupUI = <SetupLegal context={context} />;
                                                break;
                                            case "wifi":
                                                setupUI = <SetupWifi context={context} />
                                                break;
                                            case "server":
                                                setupUI = <SetupServer context={context} />
                                                break;
                                            case "server_test":
                                                setupUI = <SetupServerTest context={context} />
                                                break;
                                            case "settings":
                                                setupUI = <SetupSettings context={context} />
                                                break;
                                        }
                                        if (context.state.scope === "complete") {
                                            routeSwitch = <Switch>
                                                <Route exact path="/config">config</Route>
                                                <Route exact path="/update">update</Route>
                                                <Route exact path="/info"><ProductInfo context={context} /></Route>
                                                <Route path="/"><StatusPage context={context} /></Route>
                                            </Switch>;
                                        } else {
                                            routeSwitch = <Switch>
                                                <Route exact path="/config">config</Route>
                                                <Route exact path="/update">update</Route>
                                                <Route exact path="/info"><ProductInfo context={context} /></Route>
                                                <Route path="/">{setupUI}</Route>
                                            </Switch>;
                                        }
                                    }
                                    return routeSwitch;
                                }}
                            </ApiContextConsumer>
                        </ApiContextProvider>
                    </div>

                </BrowserRouter >
            </div>
        );
    }
}

ReactDOM.render(<Root />, document.getElementById("root"));