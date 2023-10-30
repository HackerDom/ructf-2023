import React from "react";
import { Button, Card, Col, Row } from "react-bootstrap";
import { useNavigate } from "react-router-dom";

function RustestNode({ id, name, owner, description, isMytest }) {
    const navigate = useNavigate();

    const redirectToRustestPeview = () => {
        navigate(`/rustest/${id}/preview`);
    };

    const redirectToRustestSolve = () => {
        navigate(`/rustest/${id}/solve`);
    };

    const redirectToUserTests = () => {
        navigate(`/user_rustests/${owner}`);
    };

    return <>
        <Card className="m-4 mb-4" style={{ minWidth: '37rem', maxWidth: '37rem' }}>
            <Card.Body>
                <Row>
                    <h5>{name}</h5>
                </Row>
                <Row>
                    <Col>
                        <h6>Owner: <a href="javascript:void(0);" onClick={() => redirectToUserTests()}>@{owner}</a></h6>
                    </Col>
                </Row>
                <Row>
                    <h6>{description}</h6>
                </Row>
            </Card.Body>
            {isMytest && (
                <Button className="mx-3 mb-3" variant="primary" onClick={() => redirectToRustestPeview()}>
                    Preview Rustest
                </Button>
            )}
            {!isMytest && (
                <Button className="mx-3 mb-3" variant="danger" onClick={() => redirectToRustestSolve()}>
                    Check your RussNess
                </Button>
            )}
        </Card>
    </>
}

export default RustestNode;