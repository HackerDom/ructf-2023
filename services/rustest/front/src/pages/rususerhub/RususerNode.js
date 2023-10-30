import React from "react";
import {Button, Card, Col, Row} from "react-bootstrap";
import {useNavigate} from "react-router-dom";

function RususerNode({ bio, login }) {
    const navigate = useNavigate();

    const redirectToRustestsList = () => {
        // Вызываем navigate с соответствующим путем и передаем id
        navigate(`/user_rustests/${login}`);
    };

    return (
        <Card className="mx-auto mb-4" style={{ minWidth: '20rem', maxWidth: '20rem'}}>
            <Card.Body>
                <Row>
                    <Col>
                        <h6>Login:</h6>
                        <p>{login}</p>
                    </Col>
                </Row>
                <Row>
                    <h6>Bio:</h6>
                    <p>{bio}</p>
                </Row>
            </Card.Body>
            <Button className="m-3" variant="primary" onClick={() => redirectToRustestsList()}>
                ViewTests
            </Button>
        </Card>
    );
}

export default RususerNode;
