import React, { useEffect, useState } from "react";
import { Card, Container, ListGroup, Row, Spinner } from "react-bootstrap";
import { useParams, useNavigate } from "react-router-dom";
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
            .get(`/api/rustest/${paramId}/preview`, {
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

    const rediredctToUser = () => {
        navigate(`/user_rustests/${rustestData.owner}`);
    };
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
                                <h5>{rustestData.name}</h5>
                            </Row>
                            <Row>
                                <h6>Owner: <a href="javascript:void(0);" onClick={() => rediredctToUser()}>@{rustestData.owner}</a></h6>
                            </Row>
                            <Row>
                                <h6>{rustestData.description}</h6>
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
                                        answerIndex === question.correct_idx
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
