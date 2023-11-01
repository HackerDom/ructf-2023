import { MigrationInterface, QueryRunner } from "typeorm";

export class ConnectUser1697981359286 implements MigrationInterface {
    name = 'ConnectUser1697981359286'

    public async up(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`ALTER TABLE \`udar\` CHANGE \`teacher\` \`teacherId\` int NOT NULL`);
        await queryRunner.query(`ALTER TABLE \`udar\` CHANGE \`teacherId\` \`teacherId\` int NULL`);
        await queryRunner.query(`ALTER TABLE \`udar\` ADD CONSTRAINT \`FK_9b3495411451bc0ae1503d56afd\` FOREIGN KEY (\`teacherId\`) REFERENCES \`user\`(\`id\`) ON DELETE CASCADE ON UPDATE NO ACTION; `);
    }

    public async down(queryRunner: QueryRunner): Promise<void> {
        await queryRunner.query(`ALTER TABLE \`udar\` DROP FOREIGN KEY \`FK_9b3495411451bc0ae1503d56afd\``);
        await queryRunner.query(`ALTER TABLE \`udar\` CHANGE \`teacherId\` \`teacherId\` int NOT NULL`);
        await queryRunner.query(`ALTER TABLE \`udar\` CHANGE \`teacherId\` \`teacher\` int NOT NULL`);
    }

}
