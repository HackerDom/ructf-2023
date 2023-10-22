import {Controller, Get, Path, Route, Security} from "tsoa";
import {UserData, UsersService} from "./usersService";
import {omitField} from "../utils/typesHelp";

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
}