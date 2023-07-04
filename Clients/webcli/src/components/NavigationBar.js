import {Container, Nav, NavDropdown, NavLink, Navbar } from "react-bootstrap";
import { useNavigate } from "react-router-dom";
import {GiAngelOutfit} from "react-icons/gi";
import { FaUserSecret, FaUserTie } from "react-icons/fa";

const NavigationBar = () => {
    
    const userLogout = () => {

        localStorage.clear();
        window.location.reload(false);
    }

    const isAuth = () => {

        return localStorage.getItem("authToken");
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