import {Controller, Get, Path, Post, Request, Route, Security} from "tsoa";
import {UserData, UsersService} from "./usersService";
import {omitField} from "../utils/typesHelp";
import {TokenPayload} from "../auth/tokenPayload";

@Route("users")
export class UsersController extends Controller {
    @Get("{username}")
    @Security("jwt", ["username", "exp", "userId"])
    public async getUser(
        @Path() username: string,
    ): Promise<UserData | null> {
        const service  = new UsersService();
        const user = await service.findUserByUsername(username);
        if (!user){
            this.setStatus(404);
            return null
        }
        return omitField(user, "password");
    }

    @Post("{username}")
    public async getUserId(
        @Path() username: string,
    ): Promise<number | null> {
        const service  = new UsersService();
        const user = await service.findUserByUsername(username);
        if (!user){
            this.setStatus(404);
            return null
        }
        return user.id;
    }

    @Get()
    @Security("jwt", ["username", "exp", "userId"])
    public async getUserData(
        @Request() request: any,
    ): Promise<TokenPayload | null> {
        this.setStatus(200);
        return request.user;
    }

}