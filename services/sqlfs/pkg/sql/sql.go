package sql

import (
	"github.com/doug-martin/goqu/v9"
	_ "github.com/doug-martin/goqu/v9/dialect/postgres"
)

var (
	postgres    = goqu.Dialect("postgres")
	usersTable  = postgres.From("users").Prepared(true)
	nodesTable  = postgres.From("nodes").Prepared(true)
	enriesTable = postgres.From("entries").Prepared(true)
)
