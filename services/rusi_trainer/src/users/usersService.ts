import {userRepo} from "../db/repo/userRepo";
import {User} from "../db/entity/user";
import {AppDataSource} from "../db/db";


export interface UserCreateParams {
    username: string
    hashedPassword: string
}

export type UserData = Omit<User, 'password'>


export class UsersService {
    findUserByUsername(username: string) : Promise<User | null> {
        return userRepo.findOne({where: {username: username}})
    }

    getUser(userId: number) : Promise<User | null> {
        return userRepo.findOne({where: {id: userId}})
    }

    async createUser(params: UserCreateParams) : Promise<User> {
        let newUser = new User()
        newUser.username = params.username;
        newUser.password = params.hashedPassword;
        return await AppDataSource.manager.save(newUser)
    }

}