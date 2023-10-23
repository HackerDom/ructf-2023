package basestore

import (
	"context"
	"database/sql"
	"fmt"

	"github.com/rs/zerolog"
)

type BaseStore interface {
	RunInTransaction(_ context.Context, f func(context.Context) error) error
	QueryRowContext(_ context.Context, query string, args ...any) *sql.Row
	ExecContext(_ context.Context, query string, args ...any) (sql.Result, error)
}

func New(db *sql.DB) BaseStore {
	return &baseStore{db: db}
}

type baseStore struct {
	db *sql.DB
}

func (bs *baseStore) RunInTransaction(ctx context.Context, f func(context.Context) error) error {
	tx, err := bs.db.Begin()
	if err != nil {
		return fmt.Errorf("failed to begin transaction: %w", err)
	}

	ctx = withTransaction(ctx, tx)
	if err := f(ctx); err != nil {
		if rollbackErr := tx.Rollback(); rollbackErr != nil {
			zerolog.Ctx(ctx).Err(err).Msg("failed to rollback transaction")
		}

		return err
	}

	if err := tx.Commit(); err != nil {
		return fmt.Errorf("failed to commit transaction: %w", err)
	}

	return nil
}

func (bs *baseStore) QueryRowContext(ctx context.Context, query string, args ...any) *sql.Row {
	tx, ok := fromContext(ctx)
	if ok {
		return tx.QueryRowContext(ctx, query, args...)
	}

	return bs.db.QueryRowContext(ctx, query, args...)
}

func (bs *baseStore) ExecContext(ctx context.Context, query string, args ...any) (sql.Result, error) {
	tx, ok := fromContext(ctx)
	if ok {
		return tx.ExecContext(ctx, query, args...)
	}

	return bs.db.ExecContext(ctx, query, args...)
}
