import {Udar} from "../db/entity/udar";
import {udarRepo} from "../db/repo/udarRepo";
import {AppDataSource} from "../db/db";
import typeormppend from "../db/extension/typeormppend";


export class UdarService {
    public get(name: string): Promise<Udar | null> {
        return udarRepo.findOne({where:{name}, relations: {teacher: true}})
    }

    public async make(params: Udar): Promise<void> {
      await typeormppend.appendOnlyQuery(AppDataSource
      .createQueryBuilder()
      .insert()
      .into(Udar)
      .values(params)
      .orUpdate(['description', 'trustedRusi', 'map'], ['name']), 'trustedRusi')
    }
}