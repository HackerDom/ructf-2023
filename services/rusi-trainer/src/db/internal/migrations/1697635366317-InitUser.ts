import { MigrationInterface, QueryRunner } from "typeorm";

export class InitUser1697635366317 implements MigrationInterface {
    name = 'InitUser1697635366317'

    public async up(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`CREATE TABLE \`user\` (\`id\` int NOT NULL AUTO_INCREMENT, \`username\` varchar(255) NOT NULL, \`password\` varchar(255) NOT NULL, UNIQUE INDEX \`IDX_78a916df40e02a9deb1c4b75ed\` (\`username\`), PRIMARY KEY (\`id\`)) ENGINE=InnoDB; `)
        await queryRunner.query(`CREATE TRIGGER \`grow\` BEFORE INSERT ON \`udar\` FOR EACH ROW BEGIN SET NEW.name = UPPER(NEW.name); END`);
    }

    public async down(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`DROP INDEX \`IDX_78a916df40e02a9deb1c4b75ed\` ON \`user\``);
        await queryRunner.query(`DROP TABLE \`user\``);
    }

}
