import React, { useEffect, useState } from "react";
import {Card, Col, Container, ListGroup, Row, Spinner} from "react-bootstrap";
import { useNavigate, useParams } from "react-router-dom";
import axios from "axios";

function RustestPage() {
    const navigate = useNavigate();
    const { paramId } = useParams();
    const [isLoading, setIsLoading] = useState(true);
    const [rustestData, setRustestData] = useState(null); // Стейт для хранения данных о рустесте
    const token = localStorage.getItem("jwtToken");

    useEffect(() => {
        // Выполнить запрос, но не сохранять данные в состоянии
        setIsLoading(true); // Устанавливаем состояние загрузки в true
        axios
            .get(`http://104.248.87.99:13337/rustest/${paramId}/preview`, {
                headers: {
                    Authorization: "Bearer " + token,
                },
            })
            .then((response) => {
                // Ответ от сервера без сохранения данных в состоянии
                setRustestData(response.data); // Сохраняем данные о рустесте
                setIsLoading(false); // Завершили загрузку, устанавливаем состояние загрузки в false
            })
            .catch((error) => {
                console.error("Error fetching data", error);
                setIsLoading(false); // Завершили загрузку с ошибкой, устанавливаем состояние загрузки в false
            });
    }, [paramId, token]);

    return (
        <>
            {isLoading && (
                <Spinner animation="border" role="status">
                    <span className="visually-hidden"></span>
                </Spinner>
            )}
            {!isLoading && rustestData && (
                <Container className="my-3">
                    <Card className="mb-4">
                        <Card.Body>
                            <Row>
                                <Col>
                                    <h6>ID:</h6>
                                    <p>{rustestData.id}</p>
                                </Col>
                                <Col md={3}>
                                    <h6>Name:</h6>
                                    <p>{rustestData.name}</p>
                                </Col>
                                <Col md={3}>
                                    <h6>Owner:</h6>
                                    <p>{rustestData.owner}</p>
                                </Col>
                            </Row>
                            <Row>
                                <h6>Description:</h6>
                                <p>{rustestData.description}</p>
                            </Row>
                        </Card.Body>
                    </Card>
                    {rustestData.questions && (
                    <Card>
                        <Card.Body>

                                <Row className="p-3 m-1">
                                    <h1 className="p-0">Questions:</h1>
                                    <RustestQuestionsList questions={rustestData.questions} />
                                </Row>
                        </Card.Body>
                    </Card>
                    )}
                </Container>
            )}
        </>
    );
}

function RustestQuestionsList({ questions }) {
    return (
        <ListGroup className="p-6" as="ol" numbered>
            {questions.map((question, index) => (
                <div className="mt-3" key={index} as="li">
                    <h3 className="mb-2">{question.question}</h3>
                    <ListGroup as="ul">
                        {question.allowed_answers.map((answer, answerIndex) => (
                            <ListGroup.Item
                                key={answerIndex}
                                as="li"
                                style={{
                                    backgroundColor:
                                        answerIndex === question.correct_idx - 1
                                            ? "lightgreen"
                                            : "transparent",
                                }}
                            >
                                {answer}
                            </ListGroup.Item>
                        ))}
                    </ListGroup>
                </div>
            ))}
        </ListGroup>
    );
}

export default RustestPage;
