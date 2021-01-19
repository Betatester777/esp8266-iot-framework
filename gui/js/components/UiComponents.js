import React, { useState } from "react";
import styled, { createGlobalStyle, css } from "styled-components";
import { normalize } from "styled-normalize";
import { Loader, Menu as MenuIcon, Info, Eye, EyeOff, Repeat, Check } from "react-feather";
import PropTypes from "prop-types";
import MaskedInput from "react-text-mask";


export const cPrimary = "#0055ff";
export const cPrimaryHover = "#0066ee";
export const cHeader = "#111";
export const cHeaderHover = "#333";
export const cSecondary = "#ff00cc";
export const cSecondaryHover = "#cc0099";

export const Hamburger = ({ onClick }) => (
    <a onClick={onClick} className="hamburger">
        <MenuIcon />
    </a>
);

Hamburger.propTypes = {
    onClick: PropTypes.func,
};

export const Header = ({ children }) => (
    <div className="header sticky">
        <div>{children}</div>
    </div>
);

Header.propTypes = {
    children: PropTypes.any
};

export const Title = ({ children }) => (
    <div className="title">
        {children}
    </div>
);

Title.propTypes = {
    children: PropTypes.any
};

export const Card = ({ children }) => (
    <div className="card">
        {children}
    </div>
);

Card.propTypes = {
    children: PropTypes.any,
};

export const Menu = ({ className, children }) => (
    <ul className={className}>{children}</ul>
);

Menu.propTypes = {
    className: PropTypes.string,
    children: PropTypes.any
};

export const Confirmation = ({ active, confirm, cancel, className, children }) => (
    active ? <div className="modal"
        onClick={() => cancel()}>
        <div onClick={(e) => e.stopPropagation()}><p>{children}</p>
            <div>
                <CancelButton title="Cancel" onClick={() => cancel()} />
                <Button title="Continue" onClick={() => confirm()} />
            </div>
        </div>
    </div> : null
);

Confirmation.propTypes = {
    active: PropTypes.bool,
    confirm: PropTypes.func,
    cancel: PropTypes.func,
    className: PropTypes.string,
    children: PropTypes.any,
};

export const Alert = ({ active, confirm, children }) => (
    active ? <div className="alert"
        onClick={() => confirm()}>
        <div onClick={(e) => e.stopPropagation()}><p>{children}</p>
            <div>
                <Button onClick={() => confirm()}>OK</Button>
            </div>
        </div>
    </div> : ""
);

Alert.propTypes = {
    active: PropTypes.bool,
    confirm: PropTypes.func,
    children: PropTypes.any,
};

export const Button = ({ name, title, onClick, isSubmit, isReset, isCancel, isDisabled, isRed, marginRight }) => {
    let className = "button";
    let type = "button";
    if (isSubmit) {
        type = "submit";
    } else if (isReset) {
        type = "reset";
    }

    if (isDisabled) {
        className = "button-disabled";
        onClick = null;
    }

    if (isCancel) {
        className = "button-cancel";
    }

    if (isRed) {
        className = "button-red";
    }

    if (marginRight) {
        className += " multibutton";
    }

    return (<button name={name} className={className} type={type} onClick={onClick}>{title}</button>);
}

export const IconButton = ({ name, icon, title, onClick, isSubmit, isReset, isCancel, isDisabled, isRed, marginRight }) => {
    let className = "button";
    let type = "button";

    if (isSubmit) {
        type = "submit";
    } else if (isReset) {
        type = "reset";
    }

    if (isDisabled) {
        className = "button-disabled";
        onClick = null;
    }

    if (isCancel) {
        className = "button-cancel";
    }

    if (isRed) {
        className = "button-red";
    }

    if (marginRight) {
        className += " multibutton";
    }

    return (<button name={name} className={className} type={type} onClick={onClick} style={{ minWidth: "1em" }}>{icon}{title}</button>);
}

export const SubmitButton = ({ title, onClick, isDisabled = false }) => {
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isSubmit={true} />;
}

export const ResetButton = ({ title, onClick, isDisabled = false }) => {
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isReset={true} />;
}


export const CancelButton = ({ title, onClick, isDisabled = false }) => {
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isCancel={true} />;
}


export const RedButton = ({ title, onClick, isDisabled = false }) => {
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isRed={true} />;
}

export const Spinner = () => (
    <span className="spinner"><Loader size={48} /></span>
);

export const Status = ({ type, text, showSpinner }) => (
    <div className="status">
        {showSpinner ?
            <div className="spinner-container" >
                <Spinner />
            </div> : null}
        <div >
            <span className={`status-${type}`}>{text}</span>
        </div>
    </div>
)

export function Fetch(props) {
    return <span onClick={(e) => {
        e.stopPropagation();
        if (typeof props.POST !== undefined) {
            fetch(props.href,
                {
                    method: "POST",
                })
                .then((response) => { return response.status; })
                .then((status) => {
                    if (status == 200) {
                        if (typeof props.onFinished === "function") { props.onFinished(); }
                    }
                });
        } else {
            fetch(props.href)
                .then((response) => { return response.status; })
                .then((status) => {
                    if (status == 200) {
                        if (typeof props.onFinished === "function") { props.onFinished(); }
                    }
                });
        }
    }}>{props.children}</span>;
}

Fetch.propTypes = {
    href: PropTypes.string,
    POST: PropTypes.bool,
    onFinished: PropTypes.func,
    children: PropTypes.any,
};

var checkControlVisible = (control, state) => {
    if (control.hasOwnProperty("display_filter")) {
        return eval(control.display_filter);
    }
    return true;
}

export const IPv4 = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    const ipProps = {
        guide: false,
        placeholderChar: '\u2000',
        mask: value => Array(value.length).fill(/[\d.]/),
        pipe: value => {
            if (value === '.' || value.endsWith('..')) return false;

            const parts = value.split('.');

            if (
                parts.length > 4 ||
                parts.some(part => part === '00' || part < 0 || part > 255)
            ) {
                return false;
            }

            return value;
        },
    };

    let onChangeHandler = function (event) {
        let { value } = event.target;
        let newState = {};
        newState[props.control.name] = value.trim();
        props.onChangeValue(newState);
    };

    let name = props.control.name;
    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.translation}</b>:
            </label>
            <MaskedInput id={name} name={name} type="text" {...ipProps} onChange={onChangeHandler} value={props.state[name]} />
        </p>
    );
}

export const Checkbox = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let onChangeHandler = function (event) {
        let { checked } = event.target;
        let newState = {};
        newState[props.control.name] = Number(checked);
        props.onChangeValue(newState);
    };

    let name = props.control.name;
    let isReadOnly = false;
    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }
    return (
        <p>
            <label for={name}>
                <b>{props.control.translation}</b>
            </label>
            <input id={name} name={name} type="checkbox" onChange={onChangeHandler} value={1} checked={props.state[name]} readOnly={false} />
        </p>
    );
}

export const NumericInput = (props) => {
    const regexp = new RegExp(`^-?[0-9]*$`);

    let name = props.control.name;
    let min = props.control.min;
    let max = props.control.max;
    let initValue = props.state[name];

    const isValid = (value, min, max) => {
        let ret = (!isNaN(Number(value)) && value !== '' && value !== '-' &&
            (min === undefined || value >= min) &&
            (max === undefined || value <= max));
        return ret;
    }

    const [internalValue, setInternalValue] = useState(initValue);
    const [valid, setValid] = useState(isValid(initValue, min, max));

    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let isReadOnly = false;

    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }

    let onChange = (event) => {
        const newValue = event.target.value.trim();

        if (newValue === '') {
            setValid(false);
            setInternalValue(newValue);
            return;
        } else if (regexp.test(newValue)) {
            setInternalValue(newValue);
            let newValid = isValid(newValue, min, max);
            console.log(newValue, newValid)
            setValid(newValid);
            if (newValid) {
                let newState = {};
                newState[props.control.name] = Number(newValue);
                props.onChangeValue(newState);
            }
        } else {
            setValid(false);
        }
    }

    let onBlur = () => {
        if (internalValue < min) {
            setInternalValue(min);
        } else if (internalValue > max) {
            setInternalValue(max);
        } else {
            setInternalValue(initValue);
        }
        setValid(true);
        let newState = {};
        newState[props.control.name] = Number(internalValue);
        props.onChangeValue(newState);
    }

    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.translation}</b>:
            </label>
            <input id={name} name={name}
                className={valid ? '' : 'invalid'}
                type="text" onChange={onChange}
                onBlur={onBlur}
                value={internalValue}
                readOnly={isReadOnly} />
        </p>
    );
}

export const DNSName = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let onChangeHandler = function (event) {
        let { value } = event.target;
        let newState = {};
        newState[props.control.name] = value.trim();
        props.onChangeValue(newState);
    };

    let name = props.control.name;
    let isReadOnly = false;
    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }
    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.translation}</b>:
            </label>
            <input id={name} name={name} type="text" onChange={onChangeHandler} value={props.state[name]} readOnly={isReadOnly} />
        </p>
    );
}

export const TextInput = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let infoKeyName = `${name}_info`;

    let onChangeHandler = function (event) {
        let { value } = event.target;
        let newState = {};
        newState[props.control.name] = value.trim();
        props.onChangeValue(newState);
    };

    let onInfo = function (event) {
        let newState = {};
        let isVisible = true;

        if (props.state.hasOwnProperty(infoKeyName)) {
            isVisible = !props.state[infoKeyName];
        }
        newState[infoKeyName] = isVisible;
        props.onChangeValue(newState);
    };

    let name = props.control.name;
    let isReadOnly = false;


    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }

    let labelContent = <b>{props.control.translation}</b>;

    let infoText = props.state[infoKeyName] ? <div>{props.control.translation_info}</div> : null;

    if (props.showInfo) {
        labelContent = <b>{props.control.translation} <Info onClick={onInfo} /></b>;
    }

    return (
        <p>
            <label htmlFor={name}>{labelContent}{infoText}</label>
            <input id={name} name={name} type="text" onChange={onChangeHandler} value={props.state[name]} readOnly={isReadOnly} />
        </p>
    );
}

export const PasswordInput = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let name = props.control.name;
    let isReadOnly = false;

    let passwordKeyName = `${name}_password`;
    let infoKeyName = `${name}_info`;

    let inputType = "password";

    let onChangeHandler = function (event) {
        let { value } = event.target;
        let newState = {};
        newState[props.control.name] = value.trim();
        props.onChangeValue(newState);
    };

    let onInfo = function (event) {
        let newState = {};
        let isVisible = true;

        if (props.state.hasOwnProperty(infoKeyName)) {
            isVisible = !props.state[infoKeyName];
        }
        newState[infoKeyName] = isVisible;
        props.onChangeValue(newState);
    };

    let onTogglePassword = function (event) {
        let newState = {};
        let isVisible = true;

        if (props.state.hasOwnProperty(passwordKeyName)) {
            isVisible = !props.state[passwordKeyName];
        }
        newState[passwordKeyName] = isVisible;
        props.onChangeValue(newState);
    };

    let eyeButton = <Eye onClick={onTogglePassword} className="password-button" />;

    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }

    let labelContent = <b>{props.control.translation}</b>;

    let infoText = props.state[infoKeyName] ? <div>{props.control.translation_info}</div> : null;

    if (props.showInfo) {
        labelContent = <b>{props.control.translation} <Info onClick={onInfo} /></b>;
    }

    if (props.state[passwordKeyName]) {
        inputType = "text";
        eyeButton = <EyeOff onClick={onTogglePassword} className="password-button" />;
    }

    return (
        <p>
            <label htmlFor={name}>{labelContent}{infoText}</label>
            <input id={name} name={name} type={inputType} className="input_password" onChange={onChangeHandler} value={props.state[name]} readOnly={isReadOnly} />
            {eyeButton}
        </p>
    );
}

export const Select = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let name = props.control.name;
    let isReadOnly = false;
    let infoKeyName = `${name}_info`;

    let onChangeHandler = function (event) {
        let { value } = event.target;
        let newState = {};
        newState[props.control.name] = Number(value);
        props.onChangeValue(newState);
    };

    let onInfo = function (event) {
        let newState = {};
        let isVisible = true;

        if (props.state.hasOwnProperty(infoKeyName)) {
            isVisible = !props.state[infoKeyName];
        }
        newState[infoKeyName] = isVisible;
        props.onChangeValue(newState);
    };

    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }

    let labelContent = <b>{props.i18n.get(props.control.label)}</b>;

    let infoText = props.state[infoKeyName] ? <div>{props.control.translation_info}</div> : null;

    if (props.showInfo) {
        labelContent = <b>{props.control.translation} <Info onClick={onInfo} /></b>;
    }

    return (
        <p>
            <label htmlFor={name}>{labelContent}{infoText}</label>
            <select id={name} name={name} onChange={onChangeHandler} >
                {
                    props.control.options.map((option) => {
                        let selected = (option.value === props.state[name]) ? "selected" : "";
                        if (isReadOnly) {
                            return selected ? <option value={option.value} selected={selected}>{props.i18n.get(option.text)}</option> : null;
                        } else {
                            return <option value={option.value} selected={selected}>{props.i18n.get(option.text)}</option>;
                        }
                    })
                }
            </select>
        </p>
    );
}

export const OptGroupSelect = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let name = props.control.name;
    let isReadOnly = false;
    let infoKeyName = `${name}_info`;

    let onChangeHandler = function (event) {
        let { value } = event.target;
        let newState = {};
        newState[props.control.name] = Number(value);
        props.onChangeValue(newState);
    };

    let onInfo = function (event) {
        let newState = {};
        let isVisible = true;

        if (props.state.hasOwnProperty(infoKeyName)) {
            isVisible = !props.state[infoKeyName];
        }
        newState[infoKeyName] = isVisible;
        props.onChangeValue(newState);
    };

    if (props.control.hasOwnProperty("read_only")) {
        isReadOnly = props.control.read_only;
    }

    let labelContent = <b>{props.control.translation}</b>;

    let infoText = props.state[infoKeyName] ? <div>{props.control.translation_info}</div> : null;

    if (props.showInfo) {
        labelContent = <b>{props.control.translation} <Info onClick={onInfo} /></b>;
    }

    return (
        <p>
            <label htmlFor={name}>{labelContent}{infoText}</label>
            <select id={name} name={name} onChange={onChangeHandler} >
                {
                    props.control.optgroups.map((optgroup) => {
                        return (
                            <optgroup label={optgroup.text}>
                                {
                                    optgroup.options.map((option) => {
                                        let selected = (option.value === props.state[name]) ? "selected" : "";
                                        return <option value={option.value} selected={selected}>{option.text}</option>;
                                    })
                                }
                            </optgroup>
                        );
                    })
                }
            </select>
        </p>
    );
}

export const TestStatus = (props) => {
    let status = "success";
    if (props.isFailed) {
        status = "failed";
    }

    let statusValue = props.i18n.get(`${props.name}.${status}`);
    if (props.value !== null) {
        statusValue = props.i18n.get(`${props.name}.${status}`, [["{value}", props.value]]);
    }

    return (
        <p>
            <div className="status-label">{props.i18n.get(props.name)}</div>
            <div className={`status ${status}`}>{statusValue}</div>
            {props.isFailed ?
                <div className="status-hint">{props.i18n.get(`${props.name}.${status}.hint`)}</div>
                : null
            }

        </p>
    );
}

export const ValueDisplay = (props) => {
    let value = props.i18n.get(`${props.name}.value.unknown`);

    let label = props.i18n.get(props.name);
    let valueStyle = "value-display";
    if (props.isDisabled) {
        valueStyle = "value-display-disabled";
        value = props.i18n.get(`${props.name}.value.disabled`)
    } else if (props.value !== null) {
        value = props.i18n.get(`${props.name}.value`, [["{value}", props.value]]);
    }

    return (
        <p className="status-row">
            <div className="status-display-label">{label}</div>
            <div>
                <div className={valueStyle}>{value}</div>
            </div>

        </p>
    );
}

export const SelectDisplay = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let name = props.control.name;
    let isReadOnly = false;
    let infoKeyName = `${name}_info`;

    let onChangeHandler = function (value) {
        let newState = {};
        newState[props.control.name] = value;
        props.onChangeValue(newState);
    };

    let labelContent = <>{props.i18n.get(props.control.label)}</>;

    let infoText = props.state[infoKeyName] ? <div>{props.control.translation_info}</div> : null;

    return (
        <p className="status-row">
            <div className="status-label">{labelContent}{infoText}</div>
            <div style={{ display: "grid" }}>
                {
                    props.control.options.map((option) => {
                        let style = "select-display-button";
                        let isDisabled = false;
                        let text = option.text;
                        if (option.value === props.state[name]) {
                            style = "select-display-button-active";
                            isDisabled = true;
                            text = `${option.text}.active`;
                        }
                        return <button className={style} onClick={() => { onChangeHandler(option.value) }} disabled={isDisabled}>{props.i18n.get(text)}</button>;
                    })
                }
            </div>
        </p>
    );
}

export const OnOffControll = (props) => {
    if (!checkControlVisible(props.control, props.state)) {
        return null;
    }

    let onOnHandler = function (event) {
        let newState = {};
        newState[props.control.name] = 1;
        props.onChangeValue(newState);
    };

    let onOffHandler = function (event) {
        let newState = {};
        newState[props.control.name] = 0;
        props.onChangeValue(newState);
    };

    let name = props.control.name;

    let statusIndicatorStyle = "status-indicator-off";
    let onStyle = "status-button-on";
    let offStyle = "status-button-off";

    if (props.state[name] == 1) {
        statusIndicatorStyle = "status-indicator-on";
        onStyle = "status-button-disabled";
    } else {
        offStyle = "status-button-disabled";
    }

    return (
        <p className="status-row">
            <div className="status-display-label">
                {props.control.translation}
            </div>
            <div style={{ height: "3em" }}>
                <div className={statusIndicatorStyle}></div>
                {!props.isReadOnly ?
                    <>
                        <button className={onStyle} style={{ marginRight: "1em" }} onClick={onOnHandler} disabled={props.state[name] == 1}>{props.onTranslation}</button>
                        <button className={offStyle} onClick={onOffHandler} disabled={props.state[name] == 0}>{props.offTranslation}</button>
                    </>
                    : null}

            </div>
        </p>
    );
}

export const WizardBox = ({ children }) => (
    <div className={{ textAlign: "center" }}>{children}</div>
)

export const Wizard = ({ children }) => (
    <div className="wizard flex">{children}</div>
)

export const Flex = ({ children }) => (
    <div className="flex">{children}</div>
)


export const Form = ({ children }) => (
    <form>{children}</form>
)