import "reflect-metadata"
import { RegisterRoutes } from "../build/routes";
import express, {
    Response as ExResponse,
    Request as ExRequest,
    json, urlencoded,
    NextFunction,
} from "express";
import { ValidateError } from "tsoa";
import {AuthError, BadRequestError} from "./utils/errors";
export const app = express();


app.use(
    urlencoded({
        extended: true,
    })
);
app.use(json());

RegisterRoutes(app);

app.use(function errorHandler(
    err: unknown,
    req: ExRequest,
    res: ExResponse,
    next: NextFunction
): ExResponse | void {
    if (err instanceof ValidateError) {
        return res.status(422).json({
            message: "Validation Failed",
            details: err?.fields,
        });
    }
    if (err instanceof AuthError) {
        return res.status(401).json({
            message: "Auth error",
            details: err.message,
        });
    }
    if (err instanceof BadRequestError) {
        return res.status(400).json({
            message: "Bad request error",
            details: err.message,
        });
    }
    if (err instanceof Error) {
        console.warn(`Caught Internal Error for ${req.path}:`);
        console.log(err)
        return res.status(500).json({
            message: "Internal Server Error",
        });
    }

    next();
});

app.use(function notFoundHandler(_req, res: ExResponse) {
    res.status(404).send({
        message: "Not Found",
    });
});
