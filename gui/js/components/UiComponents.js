import React from "react";
import styled, { createGlobalStyle, css } from "styled-components";
import { normalize } from "styled-normalize";
import { Loader, Menu as MenuIcon } from "react-feather";
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
    <div className="header">
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

export const Menu = ({className,  children })=>(
    <ul className={className}>{ children }</ul>
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
                <CancelButton title="Cancel" onClick={() => cancel()}/>
                <Button title="Continue" onClick={() => confirm()}/>
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

export const Button=({name, title, onClick, isSubmit, isReset, isCancel, isDisabled, isRed})=>{
    let className="button";
    let type="button";
    if(isSubmit){
        type="submit";
    }else if(isReset){
        type="reset";
    }

    if(isDisabled){
        className="button-disabled";
        onClick=null;
    }

    if(isCancel){
        className="button-cancel";        
    }

    if(isRed){
        className="button-red"; 
    }
    return (<button name={name} className={className} type={type} onClick={onClick}>{title}</button>);
}

export const SubmitButton=({title, onClick, isDisabled=false})=>{
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isSubmit={true}/>;
}

export const ResetButton=({title, onClick, isDisabled=false})=>{
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isReset={true}/>;
}


export const CancelButton=({title, onClick, isDisabled=false})=>{
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isCancel={true} />;
}


export const RedButton=({title, onClick, isDisabled=false})=>{
    return <Button title={title} onClick={onClick} isDisabled={isDisabled} isRed={true}/>;
}
    
export const Spinner = ()=> (
    <span className="spinner"><Loader/></span>
);

export const Status=({type, text, showSpinner})=>(
    <div className="status">
        {showSpinner?
            <div className="spinner-container" >
                <Spinner/> 
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

export const IPv4 = (props) => { 
    if(props.control.hasOwnProperty("display_filter_value") && props.control.hasOwnProperty("display_filter")){
        if(props.control.display_filter_value!==props.state[props.control.display_filter]){
            return null;
        }
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

    let onChangeHandler=function(event){ 
        let { value} = event.target;
        let newState={};
        newState[props.control.name]=value.trim();        
        props.setState(newState);
    };

    let name=props.control.name;
    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.label}</b>:
            </label>
            <MaskedInput id={name} name={name} type="text" className="input_small"  {...ipProps}
                                onChange={onChangeHandler} value={props.state[name]} />
        </p>
    );
}

export const NumericInput = (props) => { 
    if(props.control.hasOwnProperty("display_filter_value") && props.control.hasOwnProperty("display_filter")){
        if(props.control.display_filter_value!==props.state[props.control.display_filter]){
            return null;
        }
    }

    let onChangeHandler=function(event){ 
        let { value} = event.target;
        let newState={};
        newState[props.control.name]=Number(value);     
        props.setState(newState);
    };

    let name=props.control.name;
    let isReadOnly=false;
    if(props.control.hasOwnProperty("read_only")){
        isReadOnly=props.control.read_only;
    }
    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.label}</b>:
            </label>
            <input id={name} name={name} type="text" className="input_small" onChange={onChangeHandler} value={props.state[name]} readOnly={isReadOnly} />
        </p>
    );
}

export const Select = (props ) => {
    if(props.control.hasOwnProperty("display_filter_value") && props.control.hasOwnProperty("display_filter")){
        if(props.control.display_filter_value!==props.state[props.control.display_filter]){
            return null;
        }
    }
    let onChangeHandler=function(event){ 
        let { value} = event.target;
        let newState={};
        newState[props.control.name]=Number(value);        
        props.setState(newState);
    };

    let isReadOnly=false;
    if(props.hasOwnProperty("isReadOnly")){
        isReadOnly=props.isReadOnly;
    }

    let name=props.control.name;
    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.label}</b>:
            </label>
            <select id={name} name={name} onChange={onChangeHandler} >
                {
                    props.control.options.map((option) => {
                        let selected = (option.value === props.state[name]) ? "selected" : "";
                        if(isReadOnly){
                            return selected ? <option value={option.value} selected={selected}>{option.text}</option> : null;
                        }else{
                            return <option value={option.value} selected={selected}>{option.text}</option>;
                        }
                    })
                }
            </select>
        </p>
    );
}

export const OptGroupSelect = (props ) => {
    if(props.control.hasOwnProperty("display_filter_value") && props.control.hasOwnProperty("display_filter")){
        if(props.control.display_filter_value!==props.state[props.control.display_filter]){
            return null;
        }
    }
    let onChangeHandler=function(event){ 
        let { value} = event.target;
        let newState={};
        newState[props.control.name]=Number(value);        
        props.setState(newState);
    };

    let name=props.control.name;
    return (
        <p>
            <label htmlFor={name}>
                <b>{props.control.label}</b>:
            </label>
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

export const WizardBox = ({ children })=>(
    <div className={{textAlign: "center"}}>{ children }</div>
)

export const Wizard = ({ children }) => (
    <div className="wizard flex">{ children }</div>
)

export const Flex=({ children })=>(
    <div className="flex">{ children }</div>
)


export const Form=({ children })=>(
    <form>{ children }</form>
)