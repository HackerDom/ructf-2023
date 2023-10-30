import React, {useEffect} from "react";
import { Route, Routes, useNavigate } from "react-router-dom";
import Loginpage from "./pages/LoginPage";
import RegistrationPage from "./pages/RegisterPage";
import NotfoundPage from "./pages/NotfoundPage";
import NavBar from "./navigation/NavBar";
import RustestList from "./pages/rustesthub/RustestList";
import RususerList from "./pages/rususerhub/RususerList";
import MyRustestList from "./pages/rustesthub/MyRustestList";
import UserRustestList from "./pages/rustesthub/UserRustestList";
import RustestPage from "./pages/rustestsinglelogic/RustestPage";
import CreateRustest from "./pages/rustestlogic/CreateRustest";
import SolveRustest from "./pages/rustestlogic/SolveRustest";

function App() {
    const navigate = useNavigate();

    useEffect(() => {
        const token = localStorage.getItem("jwtToken");

        if (window.location.pathname !== "/register") {
            if (!token) {
                navigate("/login");
            }
        }
    }, [navigate]);

    return (
        <>
            <NavBar />
            <Routes>
                <Route path="/" element={<Loginpage />} />
                <Route path="/register" element={<RegistrationPage />} />
                <Route path="/login" element={<Loginpage />} />
                <Route path="*" element={<NotfoundPage />} />
                <Route path="/rustesthub" element={<RustestList />} />
                <Route path="/rususerhub" element={<RususerList />} />
                <Route path="/myrustesthub" element={<MyRustestList />} />
                <Route path="/user_rustests/:userId" element={<UserRustestList />} />
                <Route path="/rustest/:paramId/preview" element={<RustestPage />} />
                <Route path="/createrustest" element={<CreateRustest />} />
                <Route path="/rustest/:paramId/solve" element={<SolveRustest />} />
            </Routes>
        </>
    );
}

export default App;
