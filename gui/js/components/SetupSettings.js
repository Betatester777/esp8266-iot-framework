import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, NumericInput } from "./UiComponents";

export default class SetupSettings extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;

        this.state = {

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

        if (apiState.powerThresholdLow >= 20 && apiState.powerThresholdLow <= 20000 &&
            apiState.powerThresholdHigh >= 20 && apiState.powerThresholdHigh <= 20000 &&
            apiState.powerThresholdLow <= apiState.powerThresholdHigh) {
            isSaveEnabled = true;
        }

        return (
            <div>
                <div>
                    <h2>{this.i18n.get("setup.settings.content.title")}</h2>
                    <Form>
                        <NumericInput control={this.controls.powerThresholdHigh} state={apiState} onChangeValue={this.onChangeValue} />
                        <NumericInput control={this.controls.powerThresholdLow} state={apiState} onChangeValue={this.onChangeValue} />
                        <p>
                            <Button name="buttonSave" title={this.i18n.get("setup.settings.control.save")} onClick={this.props.context.save} isDisabled={!isSaveEnabled} />
                        </p>
                    </Form>
                </div>
            </div>
        );
    }
}
