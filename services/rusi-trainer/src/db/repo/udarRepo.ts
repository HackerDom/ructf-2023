import {AppDataSource} from "../db";
import {Udar} from "../entity/udar";


export const udarRepo = AppDataSource.getRepository(Udar)