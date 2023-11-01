import {Controller, Middlewares, Post, Query, Route} from "tsoa";
import {VodaService} from "./vodaService";
import {hasAccess} from "../security/validator";

@Route("voda")
export class VodaController extends Controller {
    @Post("drink")
    @Middlewares(hasAccess)
    public async drinkVoda(
        @Query() lake: string,
    ): Promise<number | null> {
        const service  = new VodaService();
        const water = await service.drink(lake);
        this.setStatus(201);
        return water;
    }
}