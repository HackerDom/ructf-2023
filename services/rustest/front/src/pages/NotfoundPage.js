import React from "react";
import { Link, Navigate } from 'react-router-dom';

function NotfoundPage() {
    return (
        <div>
            This page doesn't exist. Go <Link to="/rustesthub">home</Link> or use the back button.
            <Navigate to="/notfound" replace />
        </div>
    )
}

export default NotfoundPage;
