import React, { useState, useEffect } from "react";
import axios from "axios";
import { Pagination, Spinner } from "react-bootstrap";
import RususerNode from "./RususerNode";

function RususerList() {
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
                const response = await axios.get(`/api/users?page=${currentPage}`, {
                    headers: {
                        "Authorization": "Bearer " + token
                    }
                });
                // Присвоить данные из ответа в состояние
                setRustests(response.data.users);
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
        <div className="m-5">
            <div style={{ display: 'flex', flexWrap: 'wrap' }}>
                {rustests.map((item) => (
                    <RususerNode
                        bio={item.bio}
                        login={item.login}
                    />
                ))}
            </div>

            {/* Пагинация */}
            {!isLoading && (
                <Pagination>
                    <Pagination.First
                        onClick={() => handlePageChange(0)}
                        disabled={currentPage === 0}
                    />
                    <Pagination.Prev
                        onClick={() => handlePageChange(currentPage - 1)}
                        disabled={currentPage === 0}
                    />
                    {currentPage >= 1 && (
                        <Pagination.Item onClick={() => handlePageChange(currentPage - 1)}>{currentPage}</Pagination.Item>
                    )}
                    <Pagination.Item active>{currentPage + 1}</Pagination.Item>
                    {pagesTotal >= currentPage + 2 && (
                        <Pagination.Item onClick={() => handlePageChange(currentPage + 1)}>{currentPage + 2}</Pagination.Item>
                    )}
                    {currentPage + 2 < pagesTotal && (
                        <Pagination.Ellipsis disabled/>
                    )}
                    <Pagination.Next
                        onClick={() => handlePageChange(currentPage + 1)}
                        disabled={currentPage === pagesTotal - 1}
                    />
                    <Pagination.Last
                        onClick={() => handlePageChange(pagesTotal - 1)}
                        disabled={currentPage === pagesTotal - 1}
                    />
                </Pagination>
            )}


            {/* Лоадер, который отображается во время загрузки */}
            {isLoading && (
                <Spinner animation="border" role="status" className="m-3">
                    <span className="visually-hidden"></span>
                </Spinner>
            )}
        </div>
    );
}

export default RususerList;
