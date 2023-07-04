import { useContext, useEffect, useState } from "react";
import { Accordion, AccordionContext, Badge, Button, Card, Col, Container, Row, Spinner, useAccordionButton } from "react-bootstrap";
import UserService from "../services/UserService";
import {FaTrashAlt} from "react-icons/fa"

const PayloadsPage = () => {

    const userService = new UserService();
    const [payloads, setPayloads] = useState([]);
    const [loading, setLoading] = useState(true);

    // Accordion magic.
    const [activeId, setActiveId] = useState(0);

    useEffect(() => {
        setInterval(() => {

            userService.getPayloads().then((res) => {
                
                setPayloads(res.data);
                setLoading(false);
                
            }).catch((err) => {
                
                if (err.code == "ERR_NETWORK") {
                    alert("Couldn't connect to target C2 server!")
                    return;
                }
                alert("Failed with code " + err.response.status);
            })
        }, 1000)
    }, [])
    
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

    const deletePayload = (index) => {
        alert("Deleting payload with index " + index)
    }

    const CustomAccordion = ({children, eventKey}) => {

        const decoratedOnClick = useAccordionButton(eventKey)

        return (
            <>
                <a style={{cursor: "pointer", textDecorationLine: "underline"}} 
                    onClick={decoratedOnClick}>{children}</a>
                <FaTrashAlt style={{float: "right", cursor: "pointer"}} onClick={() => deletePayload(eventKey)}/>
            </>
        )
    }
    
    return (
        <Container>
            <Row>
                <Col>
            <h1>All payloads</h1> 
                </Col>
                <Col>
                <Button variant="outline-success">Upload payload</Button>
                </Col>
            </Row>

            <Accordion defaultActiveKey="0"  style={{textAlign: "left"}}>
                {
                    payloads && payloads.length > 0 ? payloads.map((payload, index) => (
                        <Card>
                            <Card.Header>
                                <CustomAccordion eventKey={index}>
                                    <a onClick={() => setActiveId(index)} style={{margin: "10px"}}>{payload.name} </a>
                                    {
                                        payload.categories ? 
                                        payload.categories.map((category) => (
                                            <Badge bg={badgeColor(category)}>{category}</Badge>)) 
                                            : <></>
                                    }
                                </CustomAccordion>
                            </Card.Header>
                            <Accordion.Collapse eventKey={index} style={{textAlign: "left"}}>
                                <Card.Body>
                                    <h4>Description</h4>
                                    <pre>{payload.description}</pre>
                                    <h4>Variables</h4>
                                    <pre>No variables</pre>
                                    <h4>Payload</h4>
                                    <pre >
                                    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
                                    <br/>eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad
                                    <br/>minim veniam, quis nostrud exercitation ullamco laboris nisi ut
                                    <br/>aliquip ex ea commodo consequat. Duis aute irure dolor in
                                    <br/>reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla
                                    <br/>pariatur. Excepteur sint occaecat cupidatat non proident, sunt in
                                    <br/>culpa qui officia deserunt mollit anim id est laborum.
                                    </pre>
                                </Card.Body>
                            </Accordion.Collapse>
                        </Card> 
                    )) : <></>
                }
            </Accordion>

            {
                loading ? <p>Loading... <Spinner/></p> : <></>
            }

        </Container>
    )

}

export default PayloadsPage;