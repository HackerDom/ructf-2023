import bcrypt from 'bcrypt';
import {User} from "../db/entity/user";
import {JWT_MINUTES_EXPIRES_IN, JWT_SECRET} from "../config";
import jwt from 'jsonwebtoken';
import {TokenPayload} from "./tokenPayload";


export class AuthService {
    hashPassword(password: string) : Promise<string> {
        return bcrypt.hash(password, 10)
    }

    comparePassword(password: string, hash: string) : Promise<boolean> {
        return bcrypt.compare(password, hash)
    }

    generateToken(user: User){
        const payload: TokenPayload = {
            userId: user.id,
            username: user.username,
            exp: Date.now() + JWT_MINUTES_EXPIRES_IN * 60 * 1000
        }
        return jwt.sign(payload, JWT_SECRET)
    }
}