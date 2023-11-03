package store

import (
	"context"
	"database/sql"
	"fmt"
	"sqlfs/pkg/model"

	"github.com/doug-martin/goqu/v9"
)

type EntriesIter interface {
	Init(_ context.Context, path string) error
	HasNext() bool
	Next() (*model.Entry, error)
	Close() error
}

func (s *store) CreateEntriesIter() EntriesIter {
	return &entriesIter{store: s}
}

type entriesIter struct {
	store   *store
	rows    *sql.Rows
	hasNext bool
}

func (iter *entriesIter) Init(ctx context.Context, path string) error {
	query, args, err := enriesTable.
		Join(goqu.T("nodes"), goqu.Using("ino")).
		Select("filename", "mode", "ino").
		Where(goqu.C("path").Eq(path)).ToSQL()
	if err != nil {
		return fmt.Errorf("failed to build query: %w", err)
	}

	rows, err := iter.store.QueryContext(ctx, query, args...)
	if err != nil {
		return fmt.Errorf("failed to exec query %s: %w", query, err)
	}
	iter.rows = rows
	iter.hasNext = rows.Next()

	return nil
}

func (iter *entriesIter) HasNext() bool {
	return iter.hasNext
}

func (iter *entriesIter) Next() (*model.Entry, error) {
	if !iter.hasNext {
		return nil, sql.ErrNoRows
	}

	var entry model.Entry
	if err := iter.rows.Scan(&entry.Name, &entry.Mode, &entry.Ino); err != nil {
		return nil, fmt.Errorf("failed to scan row: %w", err)
	}
	iter.hasNext = iter.rows.Next()

	return &entry, nil
}

func (iter *entriesIter) Close() error {
	return iter.rows.Close()
}
