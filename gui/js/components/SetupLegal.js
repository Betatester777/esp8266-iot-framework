import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner, Checkbox, } from "./UiComponents";

export default class SetupLegal extends React.Component {
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
        let isSaveEnabled = false;
        let userAgreementAccepted = this.props.context.state.userAgreementAccepted;

        if (userAgreementAccepted) {
            isSaveEnabled = true;
        } else {
            isSaveEnabled = false;
        }
        return (

            <div>
                <div>
                    <h2>{this.i18n.get("setup.legal.content.title")}</h2>
                    <div className="scrollable-container">{this.i18n.get("setup.legal.content.agreement")}</div>
                    <Form>
                        <Checkbox control={this.controls.userAgreementAccepted} state={this.props.context.state} onChangeValue={this.onChangeValue} />
                        <p>
                            <Button name="buttonSave" title={this.i18n.get("setup.legal.control.save")} onClick={this.props.context.save} isDisabled={!isSaveEnabled} />
                        </p>
                    </Form>
                </div>
            </div >
        );
    }
}
