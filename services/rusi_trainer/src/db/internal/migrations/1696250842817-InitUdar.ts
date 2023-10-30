import { MigrationInterface, QueryRunner } from "typeorm";

export class InitUdar1696250842817 implements MigrationInterface {
    name = 'InitUdar1696250842817'

    public async up(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`CREATE TABLE \`udar\` (\`name\` varchar(255) NOT NULL, \`phonk\` varchar(255) NOT NULL, \`description\` varchar(255) NOT NULL, \`trustedRusi\` text NOT NULL, \`teacher\` int NOT NULL, \`map\` int NOT NULL, PRIMARY KEY (\`name\`)) ENGINE=InnoDB;`);
    }

    public async down(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`DROP TABLE \`udar\``);
    }

}
