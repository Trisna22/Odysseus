import { useEffect, useState } from "react";
import { useParams } from "react-router-dom";



const ShellPage = () => {

    const {implantId} = useParams();
    const [implantStatus, setImplantStatus] = useState("Initializing...")

    useEffect(() => {

        
        
    }, [])
    
    return (

        <>
            <h1>Shell</h1> 

            <h3>Status: {implantStatus}</h3>

        </>
    )

}

export default ShellPage;