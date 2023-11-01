import {AppDataSource} from "../db/db";

export class VodaService {
    public async drink(name: string): Promise<number | null> {
        await AppDataSource.manager.query(`UPDATE ${name} SET water = water - 1  WHERE id = 1;`);
        return AppDataSource.manager.query(`SELECT water FROM ${name} WHERE id = 1`);
    }

}