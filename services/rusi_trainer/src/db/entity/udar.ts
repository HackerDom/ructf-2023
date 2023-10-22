import {Entity, Column, PrimaryColumn, ManyToOne} from "typeorm"
import {User} from "./user";

@Entity()
export class Udar {
    @PrimaryColumn()
    name!: string

    @Column()
    phonk!: string

    @Column()
    description!: string

    @Column("simple-array")
    trustedRusi!: number[]

    @ManyToOne(() => User, {onDelete: "CASCADE"})
    teacher!: User

    @Column()
    map!: number

}