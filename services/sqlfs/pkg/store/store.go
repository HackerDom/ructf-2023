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
	RunInTransaction(_ context.Context, f func(context.Context) error) error

	GetNodeCount(context.Context) (uint64, error)
	GetNodesTotalSize(context.Context) (uint64, error)
	GetNode(_ context.Context, ino uint64) (*model.Node, error)
	GetNodeIno(_ context.Context, path string) (uint64, error)
	CreateNode(_ context.Context, mode uint32, size uint64, nlink uint64) (*model.Node, error)
	CreateEntry(_ context.Context, path string, filename string, ino uint64) error
	GetEntriesCount(_ context.Context, options ...Option) (uint64, error)
	DeleteEntries(_ context.Context, path string, name string) (ino uint64, _ error)
	DecrementNodeNlink(_ context.Context, ino uint64) (nlink uint64, _ error)
	DeleteNode(_ context.Context, ino uint64) error
	UpdateEntries(_ context.Context, path string, name string, newPath string, newName string) error
}

func New(baseStore basestore.BaseStore) Store {
	return &store{BaseStore: baseStore}
}

type store struct {
	basestore.BaseStore
}
