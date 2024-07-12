import { useEffect, useState } from "react";
import UserService from "../services/UserService";
import { Accordion, Button, Container, Modal, Spinner, Table } from "react-bootstrap";
import AccordionItem from "react-bootstrap/esm/AccordionItem";
import AccordionHeader from "react-bootstrap/esm/AccordionHeader";
import AccordionBody from "react-bootstrap/esm/AccordionBody";


const JobManager = () => {


    const userService = new UserService();
    const [activeImplants, setActiveImplants] = useState([]);
    const [loading, setLoading] = useState(true);

    const [modalDeleteJob, setModalDeleteJob] = useState(false);
    const [jobToKill, setJobToKill] = useState("");
    const [fromSlave, setFromSlave] = useState("");

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

    const killJob = (jobId, slaveId) => {

        userService.killJob(jobId, slaveId).then((res) => {

        }).catch((err) => {
            if (err.code == "ERR_NETWORK") {
                localStorage.clear();
                window.location.replace("/?error=reset")
                return;
            } 

            console.error("Failed to kill the job!")
            alert("Failed to kill the job!");
       })
    }

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
                                                                <td><Button variant="danger" onClick={() => {setModalDeleteJob(true);setJobToKill(job.id);setFromSlave(implant.slave.id)}}>Kill</Button></td>
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

                {
                    loading ? <p>Loading... <Spinner/></p> : <></>
                }
            </Container>
            <Modal show={modalDeleteJob} onHide={() => setModalDeleteJob(false)} closeButton>
                <Modal.Header>
                    Warning
                </Modal.Header>
                <Modal.Body>
                    Are you sure to kill this job?
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="success" onClick={() => {killJob(jobToKill, fromSlave);setModalDeleteJob(false)}}>Yes</Button>
                    <Button variant="danger" onClick={() => setModalDeleteJob(false)}>No</Button>
                </Modal.Footer>
            </Modal>
        </>
    )
}

export default JobManager;