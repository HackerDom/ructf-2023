import {User} from "../entity/user";
import {AppDataSource} from "../db";


export const userRepo = AppDataSource.getRepository(User)