import { useEffect, useState } from "react";
import UserService from "../services/UserService";
import { Modal, Spinner, Table } from "react-bootstrap";


const JobsPage = () => {

    const userService = new UserService();
    const [jobs, setJobs] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {

        setInterval(() => {
            userService.getJobs().then((res) => {

                setJobs(res.data);
                setLoading(false);

            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }
            })
            
        }, 1000)
    }, [])

    // Modal magic
    const [modalShow, setModalShow] = useState(false);
    const [detailsJob, setDetailsJob] = useState({});
    const showModal = (job) => {
        setDetailsJob(job);
        setModalShow(true);
    }

    const decideColSpan = () => {
        let catLen = detailsJob.payload.categories.length;
        let varLen = detailsJob.variables.length;
        return catLen > varLen ? catLen : varLen;
    }

    return (
        <>
            <Modal show={modalShow} onHide={() => setModalShow(false)}>
                <Modal.Header closeButton>
                    <h3>Details</h3>
                </Modal.Header>
                <Modal.Body>
                    {
                        detailsJob ? 
                        <>
                            <>
                                {
                                    detailsJob ? 
                                    <>
                                    <h4>Job </h4>
                                    <Table bordered>
                                        <tbody>
                                            <tr>
                                                <th>Status</th>
                                                <td>{detailsJob.status}</td>
                                            </tr>
                                            <tr>
                                                <th>Object size</th>
                                                <td>{detailsJob.objectSize} bytes</td>
                                            </tr>
                                            <tr>
                                                <th>Created at</th>
                                                <td>{detailsJob.createdAt}</td>
                                            </tr>
                                            <tr>
                                                <th>Finished at</th>
                                                <td>{detailsJob.finishedAt ? detailsJob.finishedAt : "Still running"}</td>
                                            </tr>
                                            <tr>
                                                <th>Object location</th>
                                                <td>{detailsJob.location}</td>
                                            </tr>
                                        </tbody>


                                    </Table>
                                    </> : <></>
                                }
                            </>
                            <>
                            {
                                detailsJob.slave ? 
                                <>
                                <h4>Slave</h4>
                                <Table bordered>
                                    <tbody>
                                    <tr>
                                        <th>Nickname</th>
                                        <td>{detailsJob.slave.nickname ? detailsJob.slave.nickname : <>No nickname</>}</td>
                                    </tr>
                                    <tr>
                                        <th>IP</th>
                                        <td>{detailsJob.slave.ip}</td>
                                    </tr>
                                    <tr>
                                        <th>Computername</th>
                                        <td>{detailsJob.slave.computername}</td>
                                    </tr>
                                    <tr>
                                        <th>Username</th>
                                        <td>{detailsJob.slave.username}</td>
                                    </tr>
                                    <tr>
                                        <th>Operating system</th>
                                        <td>{detailsJob.slave.os}</td>
                                    </tr>
                                    </tbody>
                                </Table>
                                </> : <>No slave data available</>
                            }
                            </>
                            <>
                            {
                                detailsJob.payload ? 
                                <>
                                <h4>Payload</h4>
                                <Table bordered>
                                    <tbody>
                                        <tr>
                                            <th>Name</th>
                                            <td colSpan={detailsJob.payload.categories.length}>{detailsJob.payload.name}</td>
                                        </tr>
                                        <tr>
                                            <th>Description</th>
                                            <td colSpan={detailsJob.payload.categories.length}>{detailsJob.payload.description}</td>
                                        </tr>
                                        <tr>
                                            <th>Categories</th>
                                            {
                                                detailsJob.payload.categories ? detailsJob.payload.categories.map((cat) => (
                                                    <td>{cat}</td>
                                                )): <></>
                                            }
                                        </tr>
                                        <tr>
                                            <th>Created at</th>
                                            <td colSpan={detailsJob.payload.categories.length}>{detailsJob.payload.createdAt}</td>
                                        </tr>

                                        <tr>
                                            <th>Variables</th>
                                            {
                                                detailsJob.payload.variables ? <td colSpan={detailsJob.payload.categories.length}>{detailsJob.payload.variables.length} variables to set.</td> : <></>
                                            }
                                        </tr>
                                        {
                                            detailsJob.payload.variables ? detailsJob.payload.variables.map((v, i) => (
                                                <tr>
                                                    <td>{v.varname} ({v.vartype})</td>
                                                    <td colSpan={detailsJob.payload.variables.length}><b>{detailsJob.variables[i].value}</b>: <i>{v.description}</i></td>
                                                </tr>
                                            )) : <></>
                                        }
                                        <tr>
                                            <th>Payload location</th>
                                            <td colSpan={detailsJob.payload.categories.length}>{detailsJob.payload.location}</td>
                                        </tr>
                                    
                                    </tbody>
                                </Table>
                                </> : <></>
                            }
                            </>
                        </> : <></>
                    }
                </Modal.Body>

            </Modal>
            <h1>Jobs page</h1>
            <Table bordered hover>
                <thead>
                    <th>Slave</th>
                    <th>Payload</th>
                    <th>Status</th>
                    <th>Created At</th>
                    <th>Finished At</th>
                </thead>
            <tbody>
            {
                jobs ? jobs.map((job) => (
                    
                <tr onClick={() => showModal(job)} style={{cursor: "pointer"}}>
                    <td>{job.slave.ip} {job.slave.nickname ? "(" + job.slave.nickname + ")" : <></>}</td>
                    <td>{job.payload.name}</td>
                    <td>{job.status}</td>
                    <td>{job.createdAt}</td>
                    <td>{job.finishedAt ? job.finishedAt : <>Still running</>}</td>
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

export default JobsPage;