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
                    <NavLink href="/implant">Implant generator</NavLink>
                    <NavLink href="/payloads">Payloads</NavLink>
                    <NavLink href="/">Slaves</NavLink>
                    <NavLink href="/jobs">Jobs</NavLink>
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