import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, OnOffControll , SelectDisplay, ValueDisplay} from "./UiComponents";
import { Check } from "react-feather";

export default class StatusPage extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;
    }

    onChangeValue = (newState) => {
        console.info(`Value changed: [${JSON.stringify(newState)}]`, newState["outputStatus"]!=this.props.context.state["outputStatus"]);

        if(newState.hasOwnProperty("operationMode")){
            this.props.context.setOperationMode(newState["operationMode"]);
        }else if(newState.hasOwnProperty("outputStatus")){
            this.props.context.setOutputStatus(newState["outputStatus"]);
        }
      
    }

    render() {
        console.log(this.props.context.state);
        let isSaveEnabled = true;

        let apiState = this.props.context.state;
        let content = <>
            <Spinner />
            {this.i18n.get("common.status.connecting")}
        </>;

        if (apiState.isLoaded) {

            content = <div>
                <h2>{this.i18n.get("status.content.title")}</h2>
                <SelectDisplay control={this.controls.operationMode} i18n={this.i18n} state={apiState} onChangeValue={this.onChangeValue}  /> 
                <OnOffControll control={this.controls.outputStatus} state={apiState} onChangeValue={this.onChangeValue}
                    onTranslation={this.i18n.get("common.value.on")} offTranslation={this.i18n.get("common.value.off")} isReadOnly={apiState.operationMode!==0} />
                {apiState.operationMode==1 ? 
                <ValueDisplay name="status.control.measured_power" i18n={this.i18n} value={apiState.measuredPower} isDisabled={apiState.operationMode!==1} />
                :
                null}
            </div>
        }

        return (
            <div>
                {content}
            </div>
        );
    }
}
