import {Column, CreateDateColumn, Entity, Index, PrimaryGeneratedColumn} from "typeorm";


@Entity()
export class User {
    @PrimaryGeneratedColumn()
    id!: number;

    @Index({ unique: true })
    @Column()
    username!: string;

    @Column()
    password!: string;

    @CreateDateColumn()
    createdDate!: Date
}