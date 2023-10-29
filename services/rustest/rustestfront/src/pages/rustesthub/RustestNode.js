import React from "react";
import { Button, Card, Col, Row } from "react-bootstrap";
import {useNavigate} from "react-router-dom";

function RustestNode({ id, name, owner, description, isMytest }) {
    const navigate = useNavigate();

    const redirectToRustestPeview = () => {
        navigate(`/rustest/${id}/preview`);
    };

    const redirectToRustestSolve = () => {
        navigate(`/rustest/${id}/solve`);
    };

    return <>
        <Card className="m-4 mb-4" style={{ minWidth: '37rem', maxWidth: '37rem'}}>
            <Card.Body>
                <Row>
                    <h6>ID:</h6>
                    <p>{id}</p>
                </Row>
                <Row>
                    <Col>
                        <h6>Name:</h6>
                        <p>{name}</p>
                    </Col>
                    <Col>
                        <h6>Owner:</h6>
                        <p>{owner}</p>
                    </Col>
                </Row>
                <Row>
                    <h6>Description:</h6>
                    <p>{description}</p>
                </Row>
            </Card.Body>
            {isMytest && (
                <Button className="mx-3 mb-3" variant="primary" onClick={() => redirectToRustestPeview()}>
                    ViewTestPreview
                </Button>
                )}
            {!isMytest && (
            <Button className="mx-3 mb-3" variant="danger" onClick={() => redirectToRustestSolve()}>
                SolveTest
            </Button>
                )}
        </Card>
    </>
}

export default RustestNode;