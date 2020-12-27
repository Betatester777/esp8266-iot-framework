import React, { useState } from "react";
import PropTypes from "prop-types";

import styled from "styled-components";

import { FileListing } from "./FileListing";
import { Card, Button, Confirmation, Alert, Spinner, Wizard, WizardBox } from "./UiComponents";
import { Zap, Power } from "react-feather";

export function FirmwarePage(props) {

    const [state, setState] = useState(1);
    const [filename, setFilename] = useState("");
    const [modal, setModal] = useState(false);
    const [restart, setRestart] = useState(false);
    const [failed, setFailed] = useState(false);
    const [busy, setBusy] = useState(false);

    function startFlashing() {
        fetch(`${props.API}/api/update?filename=${filename}`, { method: "POST" })
            .then((response) => { return response.status; })
            .then((status) => {
                if (status == 200) {
                    setBusy(true);
                    pollStatus();
                }
            });
    }

    function pollStatus() {
        fetch(`${props.API}/api/update-status`)
            .then((response) => {
                return response.json();
            })
            .then((data) => {
                if (data.status == 1) {
                    setBusy(false);
                    setState(3);
                } else if (data.status == 254) {
                    setTimeout(pollStatus, 2000);
                } else {
                    setBusy(false);
                    setState(1);
                    setFailed(true);
                }
            });
    }

    let step;
    let buttons;
    if (state == 2) {
        if (!busy) {
            buttons = <Flex>
                <Button name="back" title="Back" onClick={() => setState(1)} />
                <Button name="update" title="Update Firmware" onClick={() => setModal(true)} />
            </Flex>;
            step = <WizardBox>
                <h1><Zap /></h1>
                <p>You have selected <b>{filename}</b> to be flashed</p>
            </WizardBox>;
        } else {
            buttons = <Flex>
                <Button name="back" title="Back" isDisabled={true} />
                <Button name="update" title="Update Firmware" isDisabled={true} />
            </Flex>;
            step = <WizardBox>
                <h1><Spinner /></h1>
                <p>The firmware <b>{filename}</b> is being flashed</p>
                <p><small>Please be patient, this can take a few minutes. Do not turn off the device!</small></p>
            </WizardBox>;
        }
    } else if (state == 3) {
        step = <WizardBox>
            <h1><Power /></h1>
            <p>The firmware <b>{filename}</b> has been flashed successfully.</p>
            <p>Please restart the device to boot from the new software version.</p>
            <p>
                <Button name="restart" title="Restart Now" onClick={() => { fetch(`${props.API}/api/restart`, { method: "POST" }); setRestart(true); }} />
            </p>
        </WizardBox>;
        buttons = <Button name="back" title="Back" onClick={() => setState(1)} />;
    } else {
        step = <FileListing API={props.API} selectable={true} onSelect={(name) => { setFilename(name); setState(2); }} filter="bin" />;
    }

    return (
        <>
            <h2>Firmware Update</h2>
            <Wizard>
                <h3 className={state == 1 ? "active" : ""}>1. Select</h3>
                <h3 className={state == 2 ? "active" : ""}>2. Flash</h3>
                <h3 className={state == 3 ? "active" : ""}>3. Reboot</h3>
            </Wizard>

            <Card>
                {step}
            </Card>

            {buttons}

            <Confirmation active={modal}
                confirm={() => { setModal(false); startFlashing(); }}
                cancel={() => setModal(false)}>Are you sure? If you continue, the current software version will be overwritten.</Confirmation>

            <Alert active={failed} confirm={() => setFailed(false)}>
                The firmware update has failed.
        </Alert>

            <Alert active={restart} confirm={() => setRestart(false)}>
                The device is restarting. Please wait a few seconds and refresh this page
        </Alert>
        </>
    );
}

FirmwarePage.propTypes = {
    API: PropTypes.string,
};

