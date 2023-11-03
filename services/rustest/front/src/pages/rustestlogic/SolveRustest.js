import React, { useState, useEffect } from "react";
import {Button, Card, Container, Form, Spinner} from "react-bootstrap";
import axios from "axios";
import { useParams } from "react-router-dom";

function SolveRustest() {
    const { paramId } = useParams();
    const [formData, setFormData] = useState({
        answer: -1, // -1 для обозначения не выбранного ответа
        round: 0,
    });
    const [isLoading, setIsLoading] = useState(true);

    const [answers, setAnswers] = useState([]);
    const [winState, setWinState] = useState('');
    const [testResult, setTestResult] = useState('');
    const [rewardState, setRewardState] = useState('');
    const [question, setQuestion] = useState('');
    const token = localStorage.getItem("jwtToken");

    useEffect( () => {
        axios
            .get(`/api/rustest/${paramId}/solve`, {
                headers: {
                    "Authorization": "Bearer " + token,
                },
            })
            .then((roundResponse) => {
                if (roundResponse.data.state) {
                    setWinState(roundResponse.data.state)
                }

                if (roundResponse.data.final_state) {
                    setTestResult(roundResponse.data.final_state.result)
                    if (roundResponse.data.final_state.result === "Win") {
                        setRewardState(roundResponse.data.final_state.reward)
                    }
                }

                if (roundResponse.data.round) {
                    setFormData({...formData, round: roundResponse.data.round, answer: -1});
                }

                if (roundResponse.data.question) {
                    setAnswers(roundResponse.data.question.allowed_answers)
                }

                if (roundResponse.data.question) {
                    setQuestion(roundResponse.data.question.question)
                } else {
                    setQuestion('')
                }
                setIsLoading(false);

            })
            .catch((error) => {
                console.error("Error fetching round data", error);
                setIsLoading(false);
            });
    }, [])

    const handleSubmit = () => {
        axios
            .post(`/api/rustest/${paramId}/submit`, formData, {
                headers: {
                    "Authorization": "Bearer " + token,
                },
            })
            .then((response) => {
                if (response.data.round) {
                    setFormData({...formData, round: response.data.round, answer: -1});
                }
                if (response.data.question) {
                    setAnswers(response.data.question.allowed_answers)
                }
                if (response.data.state) {
                    setWinState(response.data.state)
                }

                if (response.data.final_state) {
                    setTestResult(response.data.final_state.result)
                    if (response.data.final_state.result === "Win") {
                        setRewardState(response.data.final_state.reward)
                    }
                }

                if (response.data.round) {
                    setFormData({...formData, round: response.data.round, answer: -1});
                }

                if (response.data.question) {
                    setQuestion(response.data.question.question);
                } else {
                    setQuestion('');
                }
                setIsLoading(false);
            })
            .catch((error) => {
                console.error("Error submitting answers", error);
                setIsLoading(false);
            });
    };

    const handleAnswerChange = (event) => {
        setFormData({ ...formData, answer: parseInt(event.target.value) });
    };

    return (
        <Container className="my-3">
            {(winState === 'Final' && testResult === 'Win') && (
                <Card className="mb-4">
                    <Card.Body>
                        <h1 className="text-success text-center">Victory, here is your reward: `{rewardState}`!</h1>
                    </Card.Body>
                </Card>
            )}
            {(winState === 'Final' && testResult === 'Lose') && (
                <Card className="mb-4">
                    <Card.Body>
                        <h1 className="text-danger text-center">Loss! Better luck next time &lt;|:^)</h1>
                    </Card.Body>
                </Card>
            )}

            {question !== '' && (
                <Card className="mb-4">
                    <Card.Body>
                        <Form>
                            <h3>Question №: {formData.round + 1}</h3>
                            <p>{question}</p>
                            <Form.Group className="mb-3">
                                <Form.Label>Choose an Answer:</Form.Label>
                                {answers.map((answer, index) => (
                                    <Form.Check
                                        type="radio"
                                        key={index}
                                        name="answer"
                                        id={`answer-${index}`}
                                        label={answer}
                                        value={index}
                                        checked={formData.answer === index}
                                        onChange={handleAnswerChange}
                                    />
                                ))}
                            </Form.Group>
                            <Button variant="primary" onClick={handleSubmit} disabled={formData.answer === -1}>
                                Submit
                            </Button>
                        </Form>
                    </Card.Body>
                </Card>
            )}
            {isLoading && (
                <Spinner animation="border" role="status" className="m-3">
                    <span className="visually-hidden">Loading...</span>
                </Spinner>
            )}
        </Container>
    );

}

export default SolveRustest;
