import {NextFunction, Request, Response} from "express";
import {BadRequestError} from "../utils/errors";
import {BestChars} from "../utils/base69";


export function hasAccess(req: Request, _: Response, next: NextFunction): void {
    const q =  req.query;
    const lake = q['lake'];
    if (!lake){
        throw new BadRequestError('DRINK WATER FROM WHAT???')
    }
    if (typeof lake !== 'string') {
        throw new BadRequestError('GO AWAY LIZARD!!!')
    }
    for (const lakeElement of lake) {
        if (!BestChars.includes(lakeElement)){
            throw new BadRequestError('GO AWAY LIZARD!!!')
        }
    }
    next();
}