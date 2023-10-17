package store

import (
	"context"
	"sqlfs/pkg/basestore"
	"sqlfs/pkg/model"

	"github.com/doug-martin/goqu/v9"

	// need to goqu dialect
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
	GetNode(_ context.Context, ino uint64) (*model.Node, error)
	GetNodeIno(_ context.Context, path string) (uint64, error)
}

func New(baseStore basestore.BaseStore) Store {
	return &store{baseStore: baseStore}
}

type store struct {
	baseStore basestore.BaseStore
}
