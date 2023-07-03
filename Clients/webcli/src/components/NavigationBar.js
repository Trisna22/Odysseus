import {Container, Nav, NavDropdown, NavLink, Navbar } from "react-bootstrap";
import { useNavigate } from "react-router-dom";

const NavigationBar = () => {
    
    const userLogout = () => {

        localStorage.clear();
        window.location.reload(false);
    }
    
    return (
        <Navbar>
            <Container>

            <Navbar.Brand href="/">Odysseus client</Navbar.Brand>

            {
                localStorage.getItem("authToken") ? 
                <>
                    <NavLink href="/payloads">Payloads</NavLink>
                    <NavLink href="/slaves">Slaves</NavLink>
                    <NavLink href="/jobs">Jobs</NavLink>
                    <Navbar.Text>
                        Signed in as:
                    </Navbar.Text>
                    <NavDropdown 
                        title={localStorage.getItem("user") ? localStorage.getItem("user") : "Anonymous"}>
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