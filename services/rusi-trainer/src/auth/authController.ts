import {Body, Controller, Post, Route, SuccessResponse} from "tsoa";
import { UsersService} from "../users/usersService";
import {AuthService} from "./authService";
import {ResponseWithMessage} from "../utils/responces";
import {AuthError} from "../utils/errors";
import {RegisterParams} from "./registerParams";
import {TokenResponse} from "./tokenResponse";


@Route("auth")
export class AuthController extends Controller {
    @Post("login")
    public async login(
        @Body() requestBody: RegisterParams
    ): Promise<TokenResponse> {
        const service = new UsersService();
        const user = await service.findUserByUsername(requestBody.username);
        if (!user) {
            this.setStatus(401);
            throw new AuthError("User not found");
        }
        const authService = new AuthService();
        const isPasswordValid = await authService.comparePassword(requestBody.password, user.password);
        if (!isPasswordValid) {
            this.setStatus(401);
            throw new AuthError("Invalid password");
        }
        const token = authService.generateToken(user);
        return {accessToken: token};
    }

    @SuccessResponse("201", "Created")
    @Post("register")
    public async register(
        @Body() requestBody: RegisterParams
    ): Promise<ResponseWithMessage> {
        const service = new UsersService();
        if (await service.findUserByUsername(requestBody.username)) {
            this.setStatus(400);
            return {message: "User already exists"};
        }
        const authService = new AuthService();
        const hashedPassword = await authService.hashPassword(requestBody.password);
        await service.createUser({username: requestBody.username, hashedPassword});
        this.setStatus(201);
        return {message: "Done"};
    }
}