import { MigrationInterface, QueryRunner } from "typeorm";

export class AddImportantField1697993068844 implements MigrationInterface {
    name = 'AddImportantField1697993068844'

    public async up(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`ALTER TABLE \`user\` ADD \`createdDate\` datetime(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6)`);
    }

    public async down(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`ALTER TABLE \`user\` DROP COLUMN \`createdDate\``);
    }

}
