import {
    Body,
    Controller,
    Get,
    Path,
    Put,
    Route, Security,
    SuccessResponse, Request
} from "tsoa";
import {UdarService} from "./udarService";
import {AuthError} from "../utils/errors";
import {UsersService} from "../users/usersService";
import {decode} from "../utils/base69";
import {ResponseWithMessage} from "../utils/responces";
import {UdarDto, UdarOutDto} from "./udar";
import {omitField} from "../utils/typesHelp";
import {TokenPayload} from "../auth/tokenPayload";


@Route("udar")
export class UdarController extends Controller {

    @Security("jwt")
    @Get("{name}")
    public async getUdar(
        @Request() request: any,
        @Path() name: string,
    ): Promise<UdarOutDto | ResponseWithMessage |null> {
        const service  = new UdarService();
        const udar = await service.get(name);
        if (!udar){
            this.setStatus(404);
            return null;
        }
        const user = request.user as TokenPayload;
        udar.trustedRusi = udar.trustedRusi.map(x => parseInt(String(x)))
        if (udar.teacher.id !== user.userId){
            if (!udar.trustedRusi.includes(user.userId)) {
                this.setStatus(404);
                return null;
            }
        }
        const myTeacherId = udar.teacher.id;
        const res: UdarOutDto = omitField(udar, 'teacher')
        res.teacherId = myTeacherId;
        return res;
    }


    @Security("jwt")
    @Get()
    public async myUdars(
        @Request() request: any,
    ): Promise<UdarDto[] | ResponseWithMessage |null> {
        const service = new UdarService();
        const user = request.user as TokenPayload;
        const udars = await service.getList(user.userId);
        return udars.map((udar) => {
            udar.trustedRusi = udar.trustedRusi.map(x => parseInt(String(x)))
            return omitField(udar, 'teacher')
        })
    }


    @SuccessResponse("201", "Ok")
    @Security("jwt")
    @Put()
    public async makeUdar(
        @Request() request: any,
        @Body() requestBody: UdarDto
    ): Promise<void> {
        const decoder = new TextDecoder();
        const service = new UdarService();
        requestBody.name = decoder.decode(decode(requestBody.name));
        const userToken = request.user as TokenPayload;
        const udar = await service.get(requestBody.name);
        if (udar && udar.teacher.id != userToken.userId)
            throw new AuthError('No access');
        const userService = new UsersService()
        const user = await userService.getUser(userToken.userId);
        if (!user)
            throw new AuthError('No access');
        this.setStatus(201);
        await service.learn({...requestBody, teacher: user});
    }
}