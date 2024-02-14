import { useEffect, useState } from "react";
import UserService from "../services/UserService";
import { Accordion, AccordionBody, AccordionHeader, AccordionItem, Button, Container, Table } from "react-bootstrap";


const JobManager = () => {


    const userService = new UserService();
    const [activeImplants, setActiveImplants] = useState([]);
    const [loading, setLoading] = useState(false);

    useEffect(() => {

        setInterval(() => {

            userService.getJobList().then((res) => {
                setActiveImplants(res.data);
                setLoading(false);
            }).catch((err) => {
                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset")
                    return;
                }
            })
            
        }, 1000)
    }, [])


    return (
        <>
            <h1>Job Manager</h1>
            <Container>
                {
                    activeImplants ? <>
                        <Accordion defaultActiveKey="0" flush>
                            {
                                activeImplants.map((implant, i) => {
                                    return (
                                        <AccordionItem eventKey={i}>
                                            <AccordionHeader>{implant.slave.ip} {implant.slave.computername} ({implant.slave.nickname}) </AccordionHeader>
                                            <AccordionBody >
                                                <Table  hover striped bordered>
                                                <thead>
                                                    <th>X</th>
                                                    <th>JobID</th>
                                                    <th>Name</th>
                                                    <th>Description</th>
                                                    <th></th>
                                                </thead>
                                                <tbody>    
                                                    {
                                                        implant.activeJobs.map((job, index) => (
                                                            <tr>
                                                                <td>{index}</td>
                                                                <td><a href="/jobs">{job.id}</a></td>
                                                                <td><a href="/payloads">{job.name}</a></td>
                                                                <td>{job.description}</td>
                                                                <td><Button variant="danger">Kill</Button></td>
                                                            </tr>
                                                        ))
                                                    }
                                                </tbody>
                                                </Table>                                        
                                                
                                            </AccordionBody>
                                        </AccordionItem>
                                    )
                                })
                            }
                        </Accordion>
                    </> : <>No active jobs currently from any implant...</>
                }
                    {/* <Accordion defaultActiveKey="0" flush>
                    <AccordionItem eventKey="0">
                        <AccordionHeader>12.34.45.67 LAPTOP-Sidney (Sidney's Laptop)</AccordionHeader>
                        <AccordionBody style={{textAlign: "left"}}>
                            <h3>Active jobs</h3>
                            <Table  hover striped bordered>
                                <thead>
                                    <th>JobID</th>
                                    <th>Name</th>
                                    <th>Description</th>
                                    <th></th>
                                </thead>
                                <tbody>
                                <tr>
                                    <td><a href="/payloads">97c873ff-b4df-433f-9428-2fde0cf06789</a></td>
                                    <td>Sleepy hellow</td>
                                    <td>Sleeps for a X count of seconds.</td>
                                    <td><Button variant="danger">Kill</Button></td>
                                </tr>
                                <tr>
                                    <td>6d20acf1-6d1c-4af7-83ac-4be40ef593dd</td>
                                    <td>Reverse shell</td>
                                    <td>Reverse shell to attacker</td>
                                    <td><Button variant="danger">Kill</Button></td>
                                </tr>
                                </tbody>
                            </Table>
                        </AccordionBody>
                    </AccordionItem>
                    <AccordionItem eventKey="1">
                        <AccordionHeader>67.23.12.55 LAPTOP-Bram (Bram's Laptop)</AccordionHeader>
                        <AccordionBody style={{textAlign: "left"}}>
                        <h3>Active jobs</h3>
                            <Table  hover striped bordered>
                                <thead>
                                    <th>JobID</th>
                                    <th>Name</th>
                                    <th>Description</th>
                                    <th></th>
                                </thead>
                                <tbody>
                                <tr>
                                    <td>97c873ff-b4df-433f-9428-2fde0cf06789</td>
                                    <td>Sleepy hellow</td>
                                    <td>Sleeps for a X count of seconds.</td>
                                    <td><Button variant="danger">Kill</Button></td>
                                </tr>
                                <tr>
                                    <td>6d20acf1-6d1c-4af7-83ac-4be40ef593dd</td>
                                    <td>Reverse shell</td>
                                    <td>Reverse shell to attacker</td>
                                    <td><Button variant="danger">Kill</Button></td>
                                </tr>
                                </tbody>
                            </Table>
                        </AccordionBody>
                    </AccordionItem>
                </Accordion> */}
                
            </Container>
        </>
    )
}

export default JobManager;