import { MigrationInterface, QueryRunner } from "typeorm";

export class InitVoda1697982489565 implements MigrationInterface {
    name = 'InitVoda1697982489565'

    public async up(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`CREATE TABLE \`baikal\` (\`id\` int NOT NULL AUTO_INCREMENT, \`water\` int NOT NULL, PRIMARY KEY (\`id\`)) ENGINE=InnoDB`);
    }

    public async down(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`DROP TABLE \`baikal\``);
    }

}
