import * as express from "express";
import * as jwt from "jsonwebtoken";
import {AuthError} from "../utils/errors";
import {JWT_SECRET} from "../config";
import {TokenPayload} from "../auth/tokenPayload";

export function expressAuthentication(
    request: express.Request,
    securityName: string,
    scopes?: string[]
): Promise<TokenPayload> {
    if (securityName === "jwt") {
        const token =
            request.body.token ||
            request.query.token ||
            request.headers["x-access-token"];

        return new Promise((resolve, reject) => {
            if (!token) {
                reject(new AuthError("No token provided"));
            }
            jwt.verify(token, JWT_SECRET, function (err: any, decoded: any) {
                if (err) {
                    reject(new AuthError("Not valid token"));
                } else {
                    const expired: number = decoded.exp;
                    if (!expired){
                        reject(new AuthError("No expire."))
                    }
                    if (expired < new Date().getTime()){
                        reject(new AuthError("Token expired."))
                    }
                    // Check if JWT contains all required scopes
                    if (!scopes){
                        resolve(decoded)
                    } else {
                        let fields = Object.keys(decoded);
                        for (let scope of scopes) {
                            if (!fields.includes(scope)) {
                                reject(new AuthError("JWT does not contain required scope."));
                            }
                        }
                        decoded = decoded as TokenPayload
                        resolve(decoded);
                    }
                }
            });
        });
    }
    throw Error("Not Implemented Auth Sec")
}
