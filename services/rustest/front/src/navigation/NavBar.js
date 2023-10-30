import React from "react";
import { Link, useNavigate, useLocation } from "react-router-dom";
import {Nav, Navbar, Container, NavDropdown, Button} from "react-bootstrap";

function NavBar() {
    const navigate = useNavigate();
    const location = useLocation();

    const handleLogout = () => {
        localStorage.removeItem("jwtToken");
        navigate("/login");
    };

    if (location.pathname === "/login" || location.pathname === "/register" || location.pathname === "/" || location.pathname === '/notfound') {
        return null;
    }

    return (
        <Navbar  data-bs-theme="dark" bg="primary" expand="lg">
            <Container>
                <Navbar.Brand as={Link} to="/rustesthub">Rustest Hub</Navbar.Brand>
                <Navbar.Brand as={Link} to="/rususerhub">Rususer Hub</Navbar.Brand>
                <Navbar.Brand as={Link} to="/myrustesthub">MyRustest Hub</Navbar.Brand>
                <Navbar.Brand as={Link} to="/createrustest"><Button variant="success">Create Rustest</Button></Navbar.Brand>
                <Navbar.Collapse id="navbarNav">
                    <Nav className="ml-auto">
                        <NavDropdown title="Account">
                            <NavDropdown.Item onClick={handleLogout}>Logout</NavDropdown.Item>
                        </NavDropdown>
                    </Nav>
                </Navbar.Collapse>
            </Container>
        </Navbar>
    );
}

export default NavBar;
