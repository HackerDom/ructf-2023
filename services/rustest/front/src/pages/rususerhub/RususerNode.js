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
                        <h6>@{login}</h6>
                    </Col>
                </Row>
                <Row>
                    <h6>Bio: {bio}</h6>
                </Row>
            </Card.Body>
            <Button className="m-3" variant="primary" onClick={() => redirectToRustestsList()}>
                View Rustests of user
            </Button>
        </Card>
    );
}

export default RususerNode;
