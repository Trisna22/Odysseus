import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import UserService from "../services/UserService";
import { Button, FormControl, InputGroup } from "react-bootstrap";
import css from "./ShellPage.module.css"

const ShellPage = () => {

    const userService = new UserService();

    const HELP_BUILTIN_COMMANDS = `
Available commands

Built-In commands:

ls/dir                       Lists the current working directory.
pwd/cwd                      Display the current working directory.
whoami                       Show logged in user.
sysinfo                      Display system information.
upload/put                   Uploads a file to the target implant.
download/get [path]          Downloads the file from the target implant.

`;

    const {implantId} = useParams();
    const [implantStatus, setImplantStatus] = useState("Initializing...")
    const [command, setCommand] = useState()
    const [loggedInUser, setLoggedInUser] = useState();

    const [commandOutput, setCommandOutput] = useState([])

    useEffect(() => {

        setLoggedInUser(localStorage.getItem("user") ? 
            localStorage.getItem("user") : "Anonymous")

    }, [])

    const getBeaconCommands = () => {
        return `Beacon commands:
lock                         Locks the screen.
dropransomware               Uploads ransomware. 
sleep [seconds]              Sleep for X count of seconds.

`;
    }

    const exec = () => {

        // Delete input command.
        const copyCommand = command;
        setCommand("")

        var output = "Unknown command: " + copyCommand
        
        // Check if built in command.
        if (copyCommand == "help") {
            output += HELP_BUILTIN_COMMANDS;
            output += getBeaconCommands();
        }

        // userService.launchCommand()...

        // Execute command.
        const newCommand = {
            command: copyCommand,
            data: output
        }
        
        setCommandOutput(prev => [newCommand, ...prev])
    }


    const onEnterKey = (e) => {
        console.log(e.key)
        if (e.key == 'Enter') {
            exec()
        }

    }
    
    return (

        <>
            <h1>Shell</h1> 
            <h3 style={{textAlign: "left", padding: "10px"}}>Status: {implantStatus}</h3>

            <div class={css.outputShell}>

                {
                    commandOutput.length > 0 ? commandOutput.map((output) => {

                        return (
                            <div class={css.commandOutput}>
                                <div>
                                    [{loggedInUser}] &gt; {output.command}
                                </div>
                                <pre>
                                    {output.data}
                                </pre>
                            </div>
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