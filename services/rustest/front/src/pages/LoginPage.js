import React, { useState } from "react";
import {Link, useNavigate} from "react-router-dom";
import { Button, Col, Container, Form, Row } from "react-bootstrap";
import axios from "axios"; // Импорт Axios

function LoginPage() {
    const [email, setEmail] = useState("");
    const [password, setPassword] = useState("");
    const navigate = useNavigate();

    const handleLogin = () => {
        const data = {
            login: email,
            password: password,
        };

        axios.post("/api/login", data)
            .then((response) => {
                localStorage.setItem("jwtToken", response.data['token']);
                navigate('/rustesthub')
            })
            .catch((error) => {
                console.error("Ошибка при запросе на сервер", error);
            });
    };

    return (
        <Container>
            <Row className="justify-content-center mt-5">
                <Col md={6}>
                    <h2>Login</h2>
                    <Form className="mt-4">
                        <Form.Group controlId="formBasicEmail" className="m-2">
                            <Form.Label>Email address</Form.Label>
                            <Form.Control
                                type="email"
                                placeholder="Enter email"
                                value={email}
                                onChange={(e) => setEmail(e.target.value)}
                            />
                        </Form.Group>

                        <Form.Group controlId="formBasicPassword" className="m-2">
                            <Form.Label>Password</Form.Label>
                            <Form.Control
                                type="password"
                                placeholder="Password"
                                value={password}
                                onChange={(e) => setPassword(e.target.value)}
                            />
                        </Form.Group>

                        <div className="text-center">
                            <Button variant="primary" type="button" className="m-2" onClick={handleLogin}>
                                Login
                            </Button>
                            <Link to="/register">
                                <Button variant="secondary" className="m-2">
                                    Registration
                                </Button>
                            </Link>
                        </div>
                    </Form>
                </Col>
            </Row>
        </Container>
    );
}

export default LoginPage;
