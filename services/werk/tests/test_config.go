package tests

import "back/config"

var testPgConfig = &config.Postgres{
	Host:         "localhost",
	Port:         5432,
	Database:     "db",
	Username:     "postgres",
	Password:     "password",
	MaxIdleConns: 10,
	MaxOpenConns: 10,
}

var testUserClientConfig = &config.ClientConfig{
	Host: "localhost",
	Port: 7654,
}
