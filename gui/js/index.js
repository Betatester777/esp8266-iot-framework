import React, { useState, useEffect } from "react";
import ReactDOM from "react-dom";
import {BrowserRouter, Switch, Route, NavLink} from "react-router-dom";
import { Box } from "react-feather";

import Config from "./configuration.json";
import {GlobalStyle, Menu, Header, Page, Hamburger} from "./comp/UiComponents";
import { WifiPage } from "./comp/WifiPage";
import { ConfigPage } from "./comp/ConfigPage";
import { FirmwarePage } from "./comp/FirmwarePage";

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

    return <div><GlobalStyle />

        <BrowserRouter>

            <Header>
                <h1><Box style={{verticalAlign:"-0.1em"}} /> {productName}</h1>

                <Hamburger onClick={() => setMenu(!menu)} />
                <Menu className={menu ? "" : "menuHidden"}>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/">WiFi Settings</NavLink></li>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/config">Configuration</NavLink></li>
                    <li><NavLink onClick={() => setMenu(false)} exact to="/update">Update</NavLink></li>
                </Menu>
            </Header>
        
            <Page>
                <Switch>
                    <Route exact path="/config">
                        <ConfigPage API={url} />
                    </Route>
                    <Route exact path="/update">
                        <FirmwarePage API={url} />
                    </Route>
                    <Route path="/">
                        <WifiPage API={url} />
                    </Route>

                </Switch>
            </Page>

        </BrowserRouter>
    </div>;

}



ReactDOM.render(<Root />, document.getElementById("root"));