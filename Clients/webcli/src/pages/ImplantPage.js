import { useEffect, useState } from "react";
import { Button, Container, Form, InputGroup } from "react-bootstrap";
import UserService from "../services/UserService";


const ImlantPage = () => {

    const userService = new UserService()
    const [payloads, setPayloads] = useState([]);
    const [errorText, setErrorText] = useState();
    const [nickname, setNickname] = useState("");
    const [os, setOS] = useState("");
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
        
        userService.generateSlave(nickname, os, initPayload).then((res) => {

            setImplantLink({
                link: "/malwaresample",
                name: os
            });
        }).catch((err) => {
            
            setErrorText("Failed to generate new slave!")
            console.error(err);
        })
    }
    
    return (
        <Container>
            <h1>Implant page</h1>
            <div>
                <InputGroup>
                    <InputGroup.Text>Nickname (optional)</InputGroup.Text>
                    <Form.Control placeholder="Trump's computer" onChange={(e) => setNickname(e.target.value)}></Form.Control>
                </InputGroup>
                <InputGroup>
                    <InputGroup.Text>Target OS</InputGroup.Text>
                    <Form.Select onChange={(e) => setOS(e.target.value)}>
                        <option>Select OS</option>
                        <option value="linux">Linux</option>
                        <option value="windows">Windows</option>
                        <option value="macos">MacOS</option>
                    </Form.Select>
                </InputGroup>
                <InputGroup>
                    <InputGroup.Text>Init payload</InputGroup.Text>
                    <Form.Select onChange={(e) => setInitPayload(e.target.value)}>
                        <option value="none">No init payload</option>
                        {
                            payloads ? payloads.map((payload) => (
                                <option value={payload.id}>{payload.name}</option>
                            )) : <></>
                        }
                    </Form.Select>
                </InputGroup>
                <br/>
                {
                    errorText ? <p style={{color: "red"}}>{errorText}</p> : <></>
                }
                <br/>
                <Button variant="outline-primary" onClick={() => generateSlave()}>Generate implant</Button>
                {
                    implantLink ? <a href={implantLink.url}>{implantLink.name}</a> : <></>
                }
            </div>
        </Container>
    )
}

export default ImlantPage;