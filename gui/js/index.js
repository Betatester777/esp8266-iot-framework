import React from "react";
import ReactDOM from "react-dom";
import { render } from "preact";

import { HashRouter, Switch, Route, NavLink } from "react-router-dom";
import { Box } from "react-feather";
import { Menu, Header, Title, Hamburger, Spinner } from "./components/UiComponents";

import { ApiContextProvider, ApiContextConsumer } from "./ApiContextProvider";
import SetupLegal from "./components/SetupLegal";
import SetupWifi from "./components/SetupWifi";
import SetupWifiTest from "./components/SetupWifiTest";
import SetupServer from "./components/SetupServer";
import SetupServerTest from "./components/SetupServerTest";
import SetupSettings from "./components/SetupSettings";
import ProductInfo from "./components/ProductInfo";
import ConfigPage from "./components/ConfigPage";
import StatusPage from "./components/StatusPage";

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
    }

    componentDidMount() {

    }

    toggleMenu = (e) => { this.setState({ menu: !this.state.menu }); }

    hideMenu = (e) => { this.setState({ menu: false }); }

    render() {
        let menuClass = this.state.menu ? "menu" : "menu menuHidden";

        return (
            <div>
                <HashRouter>
                    <ApiContextProvider>
                        <ApiContextConsumer>
                            {(context) => {
                                let content = null;
                                let setupUI = null;
                                console.log("!!!!!!render", context.state.isConnected)
                                if (!context.state.isConnected) {
                                    content = (
                                        <div>
                                            <Header>
                                                <Title>
                                                    <span className="header-logo-container"><Logo className="header-logo" /></span>
                                                    <span className="header-text">{productName}</span>
                                                </Title>
                                                <>
                                                    <Hamburger onClick={this.toggleMenu} />
                                                    <Menu className={menuClass}>
                                                        <li><NavLink onClick={this.hideMenu} exact to="/">{context.i18n.get("common.menu.status")}</NavLink></li>
                                                        <li><NavLink onClick={this.hideMenu} exact to="/info">{context.i18n.get("common.menu.info")}</NavLink></li>
                                                    </Menu>
                                                </>
                                            </Header>
                                            <div className="page">
                                                <Switch>
                                                    <Route exact path="/info"><ProductInfo context={context} /></Route>
                                                    <Route path="/">
                                                        <div className="hv-center">
                                                            <div className="h-center-align"><Spinner /></div>
                                                            <div className="h-center-align">{context.i18n.get("common.status.connecting")}</div>
                                                        </div>
                                                    </Route>
                                                </Switch>
                                            </div>
                                        </div>);
                                } else {
                                    if (context.state.scope === "complete") {
                                        content = (
                                            <div>
                                                <Header>
                                                    <Title>
                                                        <span className="header-logo-container"><Logo className="header-logo" /></span>
                                                        <span className="header-text">{productName}</span>
                                                    </Title>
                                                    <><Hamburger onClick={this.toggleMenu} />
                                                        <Menu className={menuClass}>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/">{context.i18n.get("common.menu.status")}</NavLink></li>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/config">{context.i18n.get("common.menu.config")}</NavLink></li>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/update">{context.i18n.get("common.menu.update")}</NavLink></li>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/info">{context.i18n.get("common.menu.info")}</NavLink></li>
                                                        </Menu></>
                                                </Header>
                                                <div className="page">
                                                    <Switch>
                                                        <Route exact path="/config"><ConfigPage context={context} /></Route>
                                                        <Route exact path="/update">update </Route>
                                                        <Route exact path="/info"><ProductInfo context={context} /></Route>
                                                        <Route path="/"><StatusPage context={context} /></Route>
                                                    </Switch>
                                                </div>
                                            </div>);
                                    } else {
                                        switch (context.state.scope) {
                                            case "legal":
                                                setupUI = <SetupLegal context={context} />;
                                                break;
                                            case "wifi":
                                                setupUI = <SetupWifi context={context} />
                                                break;
                                            case "wifi_test":
                                                setupUI = <SetupWifiTest context={context} />
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
                                            default:
                                                setupUI = context.i18n.get("setup.error.invalid_scope");
                                                break;
                                        }
                                        content = (
                                            <div>
                                                <Header>
                                                    <Title>
                                                        <span className="header-logo-container"><Logo className="header-logo" /></span>
                                                        <span className="header-text">{productName}</span>
                                                    </Title>
                                                    <><Hamburger onClick={this.toggleMenu} />
                                                        <Menu className={menuClass}>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/">{context.i18n.get("common.menu.setup")}</NavLink></li>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/update">{context.i18n.get("common.menu.update")}</NavLink></li>
                                                            <li><NavLink onClick={this.hideMenu} exact to="/info">{context.i18n.get("common.menu.info")}</NavLink></li>
                                                        </Menu></>
                                                </Header>
                                                <div className="page">
                                                    <Switch>
                                                        <Route exact path="/update">update</Route>
                                                        <Route exact path="/info"><ProductInfo context={context} /></Route>
                                                        <Route path="/">{setupUI}</Route>
                                                    </Switch>
                                                </div>
                                            </div>);
                                    }
                                }
                                return content;                              

                            }}

                        </ApiContextConsumer>
                    </ApiContextProvider>
                </HashRouter>
            </div>
        );
    }
}

ReactDOM.render(<Root />, document.getElementById("root"));