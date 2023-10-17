package store

import (
	"context"
	"database/sql"

	"github.com/doug-martin/goqu/v9"
	_ "github.com/doug-martin/goqu/v9/dialect/postgres"
)

var (
	postgres    = goqu.Dialect("postgres")
	usersTable  = postgres.From("users").Prepared(true)
	nodesTable  = postgres.From("nodes").Prepared(true)
	enriesTable = postgres.From("entries").Prepared(true)
)

type Store interface {
	GetNodeCount(context.Context) (uint64, error)
	GetNodesTotalSize(context.Context) (uint64, error)
}

type store struct{
	db *sql.DB
}
