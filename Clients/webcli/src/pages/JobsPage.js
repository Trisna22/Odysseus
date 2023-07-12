import { useEffect, useState } from "react";
import UserService from "../services/UserService";
import { Spinner, Table } from "react-bootstrap";


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
            
        }, 10000)
    }, [])

    return (
        <>
            <h1>Jobs page</h1>
            {
                loading ? <p>Loading... <Spinner/></p> : <></>
            }
            <Table striped bordered>
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
                    
                <tr>
                    <td>{job.slave.ip} {job.slave.nickname ? "(" + job.slave.nickname + ")" : <></>}</td>
                    <td>{job.payload.name}</td>
                    <td>{job.status}</td>
                    <td>{job.createdAt}</td>
                    <td>{job.finishedAt ? job.finishedAt : <>Not finished yet!</>}</td>
                </tr>
                )) : <></>
            } 
            </tbody>
            </Table>
        </>
    )
}

export default JobsPage;