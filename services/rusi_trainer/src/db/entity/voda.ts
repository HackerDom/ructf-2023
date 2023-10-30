import {Column, Entity, PrimaryGeneratedColumn} from "typeorm";


@Entity()
export class Baikal {
    @PrimaryGeneratedColumn()
    id!: number;

    @Column()
    water: number = 1337993;
}