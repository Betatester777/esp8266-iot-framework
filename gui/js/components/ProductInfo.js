import React from "react";
import PropTypes from "prop-types";
import { Form, Button, Spinner } from "./UiComponents";

export default class ProductInfo extends React.Component {
    constructor(props) {
        super(props);
        this.controls = this.props.context.controls;
        this.i18n = this.props.context.i18n;
    }

    render() {
        let apiState = this.props.context.state;

        return (
            <div>
                <h2>{this.i18n.get("common.info.content.title")}</h2>
                <p>
                    {this.i18n.get("common.info.content.text")}
                </p>
            </div>
        );
    }
}
