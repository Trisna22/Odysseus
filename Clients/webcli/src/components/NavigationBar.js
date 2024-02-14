import {Container, Nav, NavDropdown, NavLink, Navbar } from "react-bootstrap";
import { useNavigate } from "react-router-dom";
import {GiAngelOutfit} from "react-icons/gi";
import { FaUserSecret, FaUserTie } from "react-icons/fa";
import UserService from "../services/UserService";

const NavigationBar = () => {
    
    const userService = new UserService();
    
    const userLogout = () => {

        localStorage.clear();
        window.location.reload(false);
    }

    const isAuth = () => {        

        if (localStorage.getItem("authToken")) {
            userService.checkToken().catch((err) => {
                userLogout(); // If not logged in, go to login page.
            }); 
            return true;
        }     
        else {
            return false;
        }   
    }    

    const hasUser = () => {
        return localStorage.getItem("user");
    }
    
    return (
        <Navbar>
            <Container>
            <GiAngelOutfit size={30}/>
            <Navbar.Brand href="/">Odysseus client</Navbar.Brand>

            {
                isAuth() ? 
                <>
                    <NavLink href="/">Implant generator</NavLink>
                    <NavLink href="/implants">Implants</NavLink>
                    <NavLink href="/payloads">Payloads</NavLink>
                    <NavDropdown title="Jobs" id="jobsDropdown">
                        <NavDropdown.Item href="/jobs">All jobs</NavDropdown.Item>
                        <NavDropdown.Item href="/job-manager">Job manager</NavDropdown.Item>
                    </NavDropdown>
                    <Navbar.Text>
                        Signed in as:
                    </Navbar.Text>
                    {
                        hasUser() ?
                        <FaUserTie/> :
                        <FaUserSecret/>
                    }
                    <NavDropdown 
                        title={hasUser() ? localStorage.getItem("user") : "Anonymous"}>
                        <NavDropdown.Item onClick={() => userLogout()}>
                            Logout
                        </NavDropdown.Item> 
                    </NavDropdown>
                </> : <></>
            }
            </Container>
        </Navbar>
    )
}

export default NavigationBar;