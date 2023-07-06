import { useEffect, useState } from "react";
import UserService from "../services/UserService";
import { Spinner } from "react-bootstrap";
import {useNavigate} from "react-router-dom"


const JobsPage = () => {

    const userService = new UserService();
    const [jobs, setJobs] = useState([]);
    const [loading, setLoading] = useState(true);

    const navigate = useNavigate();

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

                alert("Failed with code " + err.response.status);
            })
            
        }, 1000)
    })

    return (
        <>
            <h1>Jobs page</h1>
            {
                loading ? <p>Loading... <Spinner/></p> : <></>
            }
        </>
    )
}

export default JobsPage;