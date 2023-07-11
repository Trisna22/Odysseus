import { useEffect, useState } from "react";
import { Button, Container, Form, InputGroup } from "react-bootstrap";
import UserService from "../services/UserService";
import { useParams } from "react-router-dom";

const LoginPage = () => {

    const [useAuth, setUseAuth] = useState(false); 
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [targetURL, setTargetURL] = useState("http://localhost:8080"); // Test
    const [errorText, setErrorText] = useState("");

    const query = new URLSearchParams(window.location.search)
    
    useEffect(() => {
        if (query.get("error")) {
            switch (query.get("error")) {
                case 'reset':
                    setErrorText("Lost connection with C2 server!");
                    return;
                case 'logout':
                    setErrorText("Succesfully logged out!")
                    return;

                default:
                    setErrorText("Somethings bad happend, unknown error '" + query.get("error") + "'!");
                    return;
            }
        }
    },[])

    const onKeyEnter = (event) => {
        if (event.key === "Enter") {
            connectToServer()
        }
    }
    
    const connectToServer = () => {

        localStorage.setItem("baseURL", targetURL);
        const userService = new UserService();

        if (useAuth) {
            userService.connectServer(username, password).then((res) => {

                localStorage.setItem("authToken", res.data.token);
                localStorage.setItem("user", username);
                window.location.reload(false); // Refresh

            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    setErrorText("Couldn't connect to target C2 server!")
                    return;
                }
    
                if (err.response.status == 403) {
                    setErrorText("Authentication credentails invalid!");
                }
                else {
                    setErrorText("Got status " + err.response.status + " as error response!");
                }
            })
        }
        else {
            userService.connectServer().then((res) => {

                localStorage.setItem("authToken", res.data.token);
                localStorage.setItem("user", username);
                window.location.reload(false); // Refresh
                
            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    setErrorText("Couldn't connect to target C2 server!")
                    return;
                }

                if (err.response.status == 403) {
                    setUseAuth(true);
                    setErrorText("This server has authentication!");
                } else {
                    setErrorText("Got status " + err.response.status + " as error response!");
                }
            })
        }
    }

    return (
        <Container style={{maxWidth: "800px"}}>
            <h1>Odysseus C2 launch party</h1>
            <p>Connecting to the Odysseus C2 server.</p>

            <InputGroup>
                <InputGroup.Text>Target C2 server</InputGroup.Text>
                <Form.Control placeholder="http://example.org:666" onChange={(e) => setTargetURL(e.target.value)} onKeyDown={onKeyEnter}></Form.Control>
                <InputGroup.Text>Use authentication?</InputGroup.Text>
                <InputGroup.Checkbox onChange={() => setUseAuth(!useAuth)}></InputGroup.Checkbox>
            </InputGroup>

            {
                useAuth ? 
                    <InputGroup>
                        <InputGroup.Text>Username</InputGroup.Text>
                        <Form.Control placeholder="john" onChange={(e) => setUsername(e.target.value)}></Form.Control>
                        <InputGroup.Text>Password</InputGroup.Text>
                        <Form.Control placeholder="****" onChange={(e) => setPassword(e.target.value)} onKeyDown={onKeyEnter}></Form.Control>
                    </InputGroup>
                    : <></>
            }
            
            <br/>
            <Button variant="outline-primary" onClick={() => connectToServer()}>Connect</Button>
            {
                errorText.length > 0 ? 
                    <p style={{color: "red"}}>{errorText}</p> : <></>
            }
        </Container>
    )
}

export default LoginPage;