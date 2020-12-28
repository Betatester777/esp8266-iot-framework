import React, { useState, useEffect } from "react";
import ReactDOM from "react-dom";
import {BrowserRouter, Switch, Route, NavLink} from "react-router-dom";
import { Box } from "react-feather";
import {Menu, Header, Title, Hamburger} from "./components/UiComponents";
import { WifiPage } from "./components/WifiPage";
import { ConfigPage } from "./components/ConfigPage";
import { StatusPage } from "./components/StatusPage";
import { FirmwarePage } from "./components/FirmwarePage";
import Logo from './logo.svg';
import "./styles.less";

let url = "http://192.168.1.54";

const productName = "WiFiSolarPlug";

if (process.env.NODE_ENV === "production") {url = window.location.origin;}

if (process.env.NODE_ENV === "development") {require("preact/debug"); url= "http://192.168.178.39";}

const displayData = new Array();

export function wsMessage(event) {
    event.data.arrayBuffer().then((buffer) => {                
        const dv = new DataView(buffer, 0);
        const timestamp = dv.getUint32(0, true);
        displayData.push([timestamp, bin2obj(buffer.slice(8,buffer.byteLength), [])]);     
    });        
}

function Root() {
    
    const [menu, setMenu] = useState(false);
    const [configData, setConfigData] = useState(null);
    const [socket, setSocket] = useState({});

    useEffect(() => {
        const ws = new WebSocket(url.replace("http://","ws://").concat("/ws"));
        ws.addEventListener("message", wsMessage);
        setSocket(ws);       
    }, []);

    let menuClass=menu ? "menu" : "menu menuHidden";
    console.log(menu)

    return <div>

        <BrowserRouter>
            <Header>
                <Title>
                    <span className="header-logo-container"><Logo className="header-logo" /></span>
                    <span className="header-text">{productName}</span>
                </Title>

                <Hamburger onClick={() => setMenu(!menu)} />
                <Menu className={menuClass}>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/">WiFi Settings</NavLink></li>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/config">Configuration</NavLink></li>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/status">Status</NavLink></li>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/update">Update</NavLink></li>
                </Menu>
            </Header>
        
            <div className="page">
                <Switch>
                    <Route exact path="/config">
                        <ConfigPage API={url} />
                    </Route>
                    <Route exact path="/status">
                        <StatusPage API={url} />
                    </Route>
                    <Route exact path="/update">
                        <FirmwarePage API={url} />
                    </Route>
                    <Route path="/">
                        <WifiPage API={url} />
                    </Route>

                </Switch>
            </div>

        </BrowserRouter>
    </div>;
}


ReactDOM.render(<Root />, document.getElementById("root"));