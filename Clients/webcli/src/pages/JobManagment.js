import { useEffect, useState } from "react";
import UserService from "../services/UserService";
import { Accordion, AccordionBody, AccordionHeader, AccordionItem, Button, Container, Table, Toast } from "react-bootstrap";


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

    const killJob = (jobId) => {

        console.log("Killing job " + jobId)

        userService.killJob(jobId).then((res) => {

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
                                                                <td><Button variant="danger" onClick={() => killJob(job.id)}>Kill</Button></td>
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
            </Container>
        </>
    )
}

export default JobManager;