import { useContext, useEffect, useRef, useState } from "react";
import { Accordion, AccordionContext, Badge, Button, Card, Col, Container, Form, InputGroup, Modal, Row, Spinner, Table, useAccordionButton } from "react-bootstrap";
import UserService from "../services/UserService";
import { FaApple, FaBomb, FaLinux, FaQuestion, FaTrashAlt, FaWindows } from "react-icons/fa"

const PayloadsPage = () => {

    const userService = new UserService();
    const [payloads, setPayloads] = useState([]);
    const [loading, setLoading] = useState(true);

    // Upload payload.
    const [modalPayloadShow, setModalPayloadShow] = useState(false);
    const inputRef = useRef();
    const [uploadedFile, setUploadedFile] = useState();
    const [newPayload, setNewPayload] = useState({});
    const [countVariables, setCountVariables] = useState(0);
    const [variables, setVariables] = useState([{}]);
    const [errorText, setErrorText] = useState("");
    const [compileError, setCompileError] = useState("");

    const onFileChange = () => {
        inputRef.current?.files &&
            setUploadedFile(inputRef.current.files[0].name)
        setErrorText("");
        setCompileError("");
    }
    const handleChangeInput = (e) => {

        const { name, value } = e.target;
        setNewPayload(prevState => ({
            ...prevState,
            [name]: value
        }))
        setErrorText("");
        setCompileError("");
    }
    const handleRadioInput = (e) => {
        const { name, id } = e.target;
        setNewPayload(prevState => ({
            ...prevState,
            [name]: id
        }))
        setErrorText("");
        setCompileError("");
    }
    const handleVariableInput = (e, index, erase = false) => {
        const { name, value } = e.target;

        const newVariables = variables.map((variable, i) => {
            if (erase && index == i) {
                return null; // Check into this.
            }

            if (index == i) {
                variable[name] = value;
                return variable;
            }

            return variable;
        })

        setNewPayload(prevState => ({
            ...prevState,
            ["variables"]: newVariables
        }))
    }

    const deleteVariableItem = (index) => {

        const newVariables = variables.filter((variable, i) => {
            if (i == index) {
                return false;
            }

            return true;
        });

        setNewPayload(prevState => ({
            ...prevState,
            ["variables"]: newVariables
        }))

    }
    const addNewPayload = () => {

        userService.newPayload(newPayload, inputRef.current.files[0]).then((res) => {

            setModalPayloadShow(false);
        }).catch((err) => {

            if (err.code == "ERR_NETWORK") {
                localStorage.clear();
                window.location.replace("/?error=reset");
                return;
            }

            if (err.response.data.compileError) {
                setCompileError(err.response.data.compileError)
            }

            setErrorText("Failed to upload new payload! Reason: " + err.response.data.message);
        })
    }

    // Delete payload.
    const [modalDeletePayloadShow, setModalDeletePayloadShow] = useState(false);
    const [payloadToDelete, setPayloadToDelete] = useState({});
    const confirmDelete = (index) => {
        setPayloadToDelete(payloads[index]);
        setModalDeletePayloadShow(true);
    }
    const deletePayload = () => {
        setModalDeletePayloadShow(false);
        alert("Deleting payload with id " + payloadToDelete.id)
    }

    // Accordion magic.
    const [activeId, setActiveId] = useState(0);
    const badgeColor = (category) => {

        switch (category) {
            case 'haunted':
            case 'scary':
                return 'danger'

            case 'stealth':
                return 'secondary'

            case 'command & control':
                return "warning";

            default:
                return 'primary'
        }
    }
    const CustomAccordion = ({ children, eventKey }) => {

        const decoratedOnClick = useAccordionButton(eventKey)

        return (
            <>
                <a style={{ cursor: "pointer", textDecorationLine: "underline" }}
                    onClick={decoratedOnClick}>{children}</a>
                <FaTrashAlt style={{ float: "right", cursor: "pointer" }} onClick={() => confirmDelete(eventKey)} />
            </>
        )
    }

    const renderOSLogo = (os) => {
        switch(os) {
            case "windows":
                return <FaWindows/>
            case "linux":
                return <FaLinux/>
            case "macos":
                return <FaApple/>
            default:
                return <FaQuestion/>
        }
    }

    useEffect(() => {
        setInterval(() => {

            userService.getPayloads().then((res) => {

                setPayloads(res.data);
                setLoading(false);

            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }

                alert("Failed with code " + err.response.status);
            })
        }, 1000)
    }, [])

    return (
        <Container>
            <Modal show={modalPayloadShow} onHide={() => setModalPayloadShow(false)}>
                <Modal.Header closeButton>
                    <h3><FaBomb /> New payload</h3>
                </Modal.Header>
                <Modal.Body>
                    <InputGroup>
                        <InputGroup.Text>Name</InputGroup.Text>
                        <Form.Control name="name" onChange={handleChangeInput} placeholder="Self destruction payload"></Form.Control>
                    </InputGroup>
                    <br />
                    <Form>
                        <Form.Label>Description of payload</Form.Label>
                        <Form.Control name="description" onChange={handleChangeInput} placeholder="Does haxx" as="textarea" rows={3} />
                    </Form>
                    <br />
                    <InputGroup>
                        <InputGroup.Text>Variable</InputGroup.Text>
                        <Form.Control placeholder="TARGET_IP" name="varname" onChange={(e) => handleVariableInput(e, 0)}></Form.Control>
                        <Button variant="outline-secondary" onClick={() => { setCountVariables(prevCount => prevCount + 1); setVariables(prev => [...prev, {}]) }}>Extra variables</Button>
                        {
                            countVariables > 0 ?
                                <Button variant="outline-secondary"
                                    onClick={(e) => { deleteVariableItem(countVariables); setCountVariables(prevState => prevState - 1) }}>
                                    <FaTrashAlt style={{ color: "red" }} />
                                </Button> : <></>
                        }
                    </InputGroup>
                    <InputGroup>
                        <InputGroup.Text>Description var</InputGroup.Text>
                        <Form.Control onChange={(e) => handleVariableInput(e, 0)} name="description" placeholder="Sets the target IP to use..."></Form.Control>
                        <Form.Select name="vartype" onChange={(e) => handleVariableInput(e, 0)}>
                            <option>Variable type?</option>
                            <option value="string">string</option>
                            <option value="number">number</option>
                        </Form.Select>
                    </InputGroup>
                    {
                        countVariables > 0 ?
                            Array.from(Array(countVariables)).map((index, value) => (
                                <>
                                    <InputGroup style={{ marginTop: "10px" }}>
                                        <InputGroup.Text>Extra variable</InputGroup.Text>
                                        <Form.Control name="varname" placeholder="TARGET_IP" onChange={(e) => handleVariableInput(e, value + 1)}></Form.Control>
                                    </InputGroup>
                                    <InputGroup>
                                        <InputGroup.Text>Description</InputGroup.Text>
                                        <Form.Control name="description" placeholder="Sets the target IP to use..." onChange={(e) => handleVariableInput(e, value + 1)}></Form.Control>
                                    </InputGroup>
                                    <Form.Select name="vartype" onChange={(e) => handleVariableInput(e, value +1)}>
                                        <option>Variable type?</option>
                                        <option value="string">string</option>
                                        <option value="number">number</option>
                                    </Form.Select>
                                </>

                            )) : <></>
                    }
                    <br />
                    <Form>
                        <Form.Check inline
                            onChange={handleRadioInput}
                            type="radio"
                            id="linux"
                            label="Linux"
                            name="os" />
                        <Form.Check inline
                            onChange={handleRadioInput}
                            type="radio"
                            id="windows"
                            label="Windows"
                            name="os" />
                        <Form.Check inline
                            onChange={handleRadioInput}
                            type="radio"
                            id="macos"
                            label="MacOS"
                            name="os" />
                        <Form.Check inline
                            onChange={handleRadioInput}
                            type="radio"
                            id="android"
                            label="Android"
                            name="os" />
                        <Form.Check inline
                            onChange={handleRadioInput}
                            type="radio"
                            id="iphone"
                            label="Iphone"
                            name="os" />
                    </Form>
                    <br />
                    Choose file: <Button type="button" variant="outline-primary"
                        onClick={() => inputRef.current?.click()}>
                        {uploadedFile ? uploadedFile : "Upload"}
                    </Button>
                    <input type="file" style={{ display: "none" }} accept=".cpp,.c,.h,.hpp"
                        onChange={onFileChange} ref={inputRef}></input>
                </Modal.Body>
                <Modal.Footer>
                    {
                        errorText.length > 0 ? <><p style={{ color: "red" }}>{errorText}</p><br /></> : <></>
                    }
                    {
                        compileError.length > 0 ? <pre>{compileError}</pre> : <></>
                    }
                    <Button variant="success" onClick={() => addNewPayload()}>Upload</Button>
                    <Button variant="warning" onClick={() => setModalPayloadShow(false)}>Cancel</Button>
                </Modal.Footer>
            </Modal>

            <Modal show={modalDeletePayloadShow} onHide={() => setModalDeletePayloadShow(false)} closeButton>
                <Modal.Header>
                    Warning
                </Modal.Header>
                <Modal.Body>
                    Are you sure to delete the payload '{payloadToDelete.name}'?
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="success" onClick={() => deletePayload()}>Yes</Button>
                    <Button variant="warning" onClick={() => setModalDeletePayloadShow(false)}>No</Button>
                </Modal.Footer>
            </Modal>

            <Row>
                <Col>
                    <h1>All payloads</h1>
                </Col>
                <Col>
                    <Button variant="outline-success" onClick={() => setModalPayloadShow(true)}>Upload payload</Button>
                </Col>
            </Row>

            <Accordion defaultActiveKey="0" style={{ textAlign: "left" }}>
                {
                    payloads && payloads.length > 0 ? payloads.map((payload, index) => (
                        <Card>
                            <Card.Header>
                                {
                                    payload.os ? renderOSLogo(payload.os) : <></>
                                }
                                <CustomAccordion eventKey={index}>
                                    <a onClick={() => setActiveId(index)} style={{ margin: "10px" }}>{payload.name} </a>
                                    {
                                        payload.categories ?
                                            payload.categories.map((category) => (
                                                <Badge bg={badgeColor(category)}>{category}</Badge>))
                                            : <></>
                                    }
                                </CustomAccordion>
                            </Card.Header>
                            <Accordion.Collapse eventKey={index} style={{ textAlign: "left" }}>
                                <Card.Body>
                                    <h4>Description</h4>
                                    <pre>{payload.description}</pre>
                                    <i>Created at: {payload.createdAt}</i>
                                    {
                                        payload.variables ? 
                                        <>
                                            <h4>Variables</h4>
                                            <Table>
                                                <thead>
                                                    <th>X</th>
                                                    <th>Name</th>
                                                    <th>DataType</th>
                                                    <th>Description</th>
                                                </thead>
                                                <tbody>
                                                {
                                                    payload.variables ? payload.variables.map((v, index) => (
                                                    <tr>
                                                        <td>{index}</td>
                                                        <td>{v.varname}</td>
                                                        <td>{v.vartype}</td>
                                                        <td>{v.description}</td>
                                                    </tr>)): <></>
                                                }
                                                </tbody>
                                            </Table>
                                        </> : <></>
                                    }
                                    
                                    <h4>Payload</h4>
                                    {
                                        payload.source ? <pre>{payload.source}</pre> : <></>
                                    }
                                </Card.Body>
                            </Accordion.Collapse>
                        </Card>
                    )) : <></>
                }
            </Accordion>

            {
                loading ? <p>Loading... <Spinner /></p> : <></>
            }

        </Container>
    )

}

export default PayloadsPage;