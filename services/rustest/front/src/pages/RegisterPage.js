import React, { useState } from "react";
import {Link, useNavigate} from "react-router-dom";
import { Button, Col, Container, Form, Row } from "react-bootstrap";
import axios from "axios";

function RegisterPage() {
    const [email, setEmail] = useState("");
    const [password, setPassword] = useState("");
    const [bio, setBio] = useState("");
    const navigate = useNavigate();

    const handleRegister = () => {
        const data = {
            login: email,
            password: password,
            bio: bio,
        };

        axios.post("/api/register", data)
            .then((response) => {
                console.log("Registration successful", response.data);
                localStorage.setItem("jwtToken", response.data['token']);
                navigate("/login");
            })
            .catch((error) => {
                console.error("Registration error", error);
            });
    };

    return (
        <Container>
            <Row className="justify-content-center mt-5">
                <Col md={6}>
                    <h2>Registration</h2>
                    <Form className="mt-4">
                        <Form.Group controlId="formBasicEmail" className="m-2">
                            <Form.Label>Email address</Form.Label>
                            <Form.Control
                                type="email"
                                placeholder="Enter your email"
                                value={email}
                                onChange={(e) => setEmail(e.target.value)}
                            />
                        </Form.Group>

                        <Form.Group controlId="formBasicPassword" className="m-2">
                            <Form.Label>Password</Form.Label>
                            <Form.Control
                                type="password"
                                placeholder="Choose a password"
                                value={password}
                                onChange={(e) => setPassword(e.target.value)}
                            />
                        </Form.Group>

                        <Form.Group controlId="formBasicBio" className="m-2">
                            <Form.Label>Bio</Form.Label>
                            <Form.Control
                                as="textarea"
                                rows={4}
                                placeholder="Tell us about yourself"
                                value={bio}
                                onChange={(e) => setBio(e.target.value)}
                            />
                        </Form.Group>

                        <div className="text-center">
                            <Button variant="primary" type="button" className="m-2" onClick={handleRegister}>
                                Register
                            </Button>
                            <Link to="/login">
                                <Button variant="secondary" className="m-2">
                                    Login
                                </Button>
                            </Link>
                        </div>
                    </Form>
                </Col>
            </Row>
        </Container>
    );
}

export default RegisterPage;
