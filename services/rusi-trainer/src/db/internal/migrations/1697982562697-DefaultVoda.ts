import { MigrationInterface, QueryRunner } from "typeorm"

export class DefaultVoda1697982562697 implements MigrationInterface {

    public async up(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`INSERT INTO \`baikal\` VALUES (1, 13337993)`);
    }

    public async down(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`DELETE FROM \`baikal\` WHERE id=1`);
    }

}
