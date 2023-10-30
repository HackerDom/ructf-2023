import React, { useState, useEffect } from "react";
import axios from "axios";
import RustestNode from "./RustestNode";
import {Container, Pagination, Spinner} from "react-bootstrap";

function MyRustestList() {
    const [currentPage, setCurrentPage] = useState(() => {
        const searchParams = new URLSearchParams(window.location.search);
        return Number(searchParams.get("page")) || 0;
    });

    const [rustests, setRustests] = useState([]); // Данные о рустестах
    const [pagesTotal, setPagesTotal] = useState(0);
    const [isLoading, setIsLoading] = useState(false); // Состояние загрузки
    const token = localStorage.getItem('jwtToken');

    useEffect(() => {
        // Функция для загрузки данных по заданной странице
        const fetchData = async (currentPage) => {
            setIsLoading(true); // Устанавливаем состояние загрузки в true
            try {
                const response = await axios.get(`/api/my_rustests?page=${currentPage}`, {
                    headers: {
                        "Authorization": "Bearer " + token
                    }
                });
                // Присвоить данные из ответа в состояние
                setRustests(response.data.rustests);
                setPagesTotal(response.data.pages_total);
                setIsLoading(false); // Завершили загрузку, устанавливаем состояние загрузки в false
            } catch (error) {
                console.error("Error fetching data", error);
                setIsLoading(false); // Завершили загрузку с ошибкой, устанавливаем состояние загрузки в false
            }
        };

        fetchData(currentPage); // Загрузить данные для текущей страницы
    }, [currentPage, token]);

    const handlePageChange = (newPage) => {
        // Изменяем GET параметры в URL при переключении страницы
        const searchParams = new URLSearchParams(window.location.search);
        searchParams.set("page", newPage);
        const newURL = `${window.location.pathname}?${searchParams.toString()}`;
        window.history.pushState({ path: newURL }, "", newURL);

        setCurrentPage(newPage); // Обновить текущую страницу
    };

    return (
        <Container style={{ display: 'flex', flexWrap: 'wrap' }}>
            {rustests.map((item) => (
                <RustestNode
                    key={item.id}
                    id={item.id}
                    name={item.name}
                    owner={item.owner}
                    description={item.description}
                    isMytest={true}
                />
            ))}

            {/* Лоадер, который отображается во время загрузки */}
            {isLoading && (
                <Spinner animation="border" role="status" className="m-3">
                    <span className="visually-hidden">Loading...</span>
                </Spinner>
            )}
        </Container>
    );
}

export default MyRustestList;
