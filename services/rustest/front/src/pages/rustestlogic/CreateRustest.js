import React, { useEffect, useState } from "react";
import { Button, Card, Col, Container, Form, Row, Alert } from "react-bootstrap";
import axios from "axios";
import { useNavigate } from "react-router-dom";

function CreateRustest() {
    const navigate = useNavigate();
    const [token, setToken] = useState(localStorage.getItem("jwtToken"));
    const [error, setError] = useState("");

    useEffect(() => {
        setToken(localStorage.getItem("jwtToken"));

        if (window.location.pathname !== "/register") {
            if (!token) {
                navigate("/login");
            }
        }
    }, [navigate, token]);

    const [formData, setFormData] = useState({
        name: "",
        description: "",
        reward: "",
        questions: [
            {
                question: "",
                allowed_answers: ["", ""],
                correct_idx: "",
            },
        ],
    });

    const handleQuestionChange = (event, questionIndex) => {
        const { name, value } = event.target;
        const updatedQuestions = [...formData.questions];
        updatedQuestions[questionIndex][name] = value;
        setFormData({ ...formData, questions: updatedQuestions });
    };

    const addQuestion = () => {
        setFormData({
            ...formData,
            questions: [
                ...formData.questions,
                {
                    question: "",
                    answers: [{ answer: "", isCorrect: false }],
                    correct_idx: "",
                },
            ],
        });
    };

    const removeQuestion = (questionIndex) => {
        const updatedQuestions = formData.questions.filter((_, index) => index !== questionIndex);
        setFormData({ ...formData, questions: updatedQuestions });
    };

    const handleAnswerChange = (questionIndex, answerIndex, value) => {
        const updatedQuestions = [...formData.questions];
        updatedQuestions[questionIndex].allowed_answers[answerIndex] = value;
        setFormData({ ...formData, questions: updatedQuestions });
    };

    const addAnswer = (questionIndex) => {
        const updatedQuestions = [...formData.questions];
        updatedQuestions[questionIndex].allowed_answers.push("");
        setFormData({ ...formData, questions: updatedQuestions });
    };

    const handleCorrectAnswerChange = (questionIndex, answerIndex) => {
        const updatedQuestions = [...formData.questions];
        updatedQuestions[questionIndex].correct_idx = answerIndex;
        setFormData({ ...formData, questions: updatedQuestions });
    };

    const handleSubmit = (event) => {
        event.preventDefault();
        axios
            .post("/api/rustest", formData, {
                headers: {
                    Authorization: "Bearer " + token,
                },
            })
            .then((response) => {
                navigate(`/rustest/${response.data.id}/preview`)
            })
            .catch((responseError) => {
                if (responseError.response) {
                    setError(responseError.response.data);
                } else {
                    setError("An error occurred.");
                }
            });
    };

    return (
        <Container className="my-3">
            <Card className="mb-4">
                <Card.Body>
                    <Form onSubmit={handleSubmit}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm="2">
                                Name:
                            </Form.Label>
                            <Col sm="10">
                                <Form.Control
                                    type="text"
                                    name="name"
                                    value={formData.name}
                                    onChange={(e) => setFormData({ ...formData, name: e.target.value })}
                                />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm="2">
                                Description:
                            </Form.Label>
                            <Col sm="10">
                                <Form.Control
                                    as="textarea"
                                    rows={3}
                                    name="description"
                                    value={formData.description}
                                    onChange={(e) => setFormData({ ...formData, description: e.target.value })}
                                />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm="2">
                                Reward:
                            </Form.Label>
                            <Col sm="10">
                                <Form.Control
                                    type="text"
                                    name="reward"
                                    value={formData.reward}
                                    onChange={(e) => setFormData({ ...formData, reward: e.target.value })}
                                />
                            </Col>
                        </Form.Group>
                        {formData.questions.map((question, questionIndex) => (
                            <Card key={questionIndex} className="mb-3">
                                <Card.Body>
                                    <Button
                                        variant="danger"
                                        size="sm"
                                        onClick={() => removeQuestion(questionIndex)}
                                    >
                                        Remove Question
                                    </Button>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm="2">
                                            Question:
                                        </Form.Label>
                                        <Col sm="10">
                                            <Form.Control
                                                type="text"
                                                name="question"
                                                value={question.question}
                                                onChange={(e) => handleQuestionChange(e, questionIndex)}
                                            />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm="2">
                                            Answers:
                                        </Form.Label>
                                        <Col sm="10">
                                            {question.allowed_answers.map((answer, answerIndex) => (
                                                <div key={answerIndex} className="mb-2">
                                                    <Form.Check
                                                        type="radio"
                                                        name={`correctAnswer_${questionIndex}`}
                                                        label={`Answer ${answerIndex}`}
                                                        id={`answer_${questionIndex}_${answerIndex}`}
                                                        checked={answerIndex === question.correct_idx}
                                                        onChange={() => handleCorrectAnswerChange(questionIndex, answerIndex)}
                                                    />
                                                    <Form.Control
                                                        type="text"
                                                        name={`answers_${questionIndex}`}
                                                        value={answer}
                                                        onChange={(e) => handleAnswerChange(questionIndex, answerIndex, e.target.value)}
                                                    />
                                                </div>
                                            ))}
                                            <Button
                                                variant="secondary"
                                                size="sm"
                                                onClick={() => addAnswer(questionIndex)}
                                            >
                                                Add Answer
                                            </Button>
                                        </Col>
                                    </Form.Group>
                                </Card.Body>
                            </Card>
                        ))}
                        <Button variant="primary" onClick={addQuestion}>
                            Add Question
                        </Button>
                        <Button type="submit" variant="success">
                            Create Rustest
                        </Button>
                    </Form>
                </Card.Body>
            </Card>
            {error && (
                <Alert variant="danger">
                    {error}
                </Alert>
            )}
        </Container>
    );
}

export default CreateRustest;
