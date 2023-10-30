import {QueryBuilder} from "typeorm";
import {CONCAT_SQL, REPLACER, VALUES_SQL} from "../../utils/sql";
import {AppDataSource} from "../db";


async function appendOnlyQuery(query: QueryBuilder<any>, field: string){
    const [q, p] = query.getQueryAndParameters();
    const exec = q.replace(VALUES_SQL.replace(REPLACER, field), CONCAT_SQL.replace(REPLACER, field))
    await AppDataSource.manager.query(exec, p)
}

export default {appendOnlyQuery}