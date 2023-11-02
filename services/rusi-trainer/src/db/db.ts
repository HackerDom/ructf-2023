import { DataSource } from "typeorm"

const AppDataSource = new DataSource({
    type: "mysql",
    host: "rusi-db",
    port: 3306,
    username: "root",
    password: "password",
    database: "mydatabase",
    entities: ["build/src/db/entity/*{.js,.ts}"],
    migrations: ["build/src/db/internal/migrations/*{.js,.ts}"]
})

AppDataSource.initialize()
    .then(() => {
        console.log("Data Source has been initialized!")
    })
    .catch((err) => {
        console.error("Error during Data Source initialization", err)
    })

export {AppDataSource}