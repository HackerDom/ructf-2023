import React, {useState} from "react";
import {Button, Card, Container, Form} from "react-bootstrap";


function Rustest() {
    // Состояние для хранения выбранного варианта ответа
    const [selectedOption, setSelectedOption] = useState(null);

    // Функция для обработки выбора ответа
    const handleOptionChange = (option) => {
        setSelectedOption(option);
    };

    return (
        <Container className="d-flex flex-column justify-content-center" style={{ height: '100vh' }}>
            <h1 className="mb-4">Тест: Как хорошо вы знаете Гномиков?</h1>
            <div>
                <Card className="mx-auto">
                    <Card.Body>
                        <Card.Title>Вопрос: Когда гномики какают?</Card.Title>
                        <Form.Group>
                            <Form.Check
                                type="radio"
                                label="Когда хотят"
                                name="option"
                                id="option1"
                                checked={selectedOption === 'option1'}
                                onChange={() => handleOptionChange('option1')}
                            />
                            <Form.Check
                                type="radio"
                                label="Когда нужно"
                                name="option"
                                id="option2"
                                checked={selectedOption === 'option2'}
                                onChange={() => handleOptionChange('option2')}
                            />
                            <Form.Check
                                type="radio"
                                label="Когда покушали"
                                name="option"
                                id="option3"
                                checked={selectedOption === 'option3'}
                                onChange={() => handleOptionChange('option3')}
                            />
                        </Form.Group>
                    </Card.Body>
                </Card>
            </div>
            <div className="mt-3 mx-auto">
                <Button style={{ height: '60px', width: '120px' }} variant="primary" disabled={!selectedOption}>
                    Ответить
                </Button>
            </div>
        </Container>
    );
}


export default Rustest;