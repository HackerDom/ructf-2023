package basestore

import (
	"context"
	"database/sql"
)

type transactionKey struct{}

func withTransaction(ctx context.Context, tx *sql.Tx) context.Context {
	return context.WithValue(ctx, transactionKey{}, tx)
}

func fromContext(ctx context.Context) (*sql.Tx, bool) {
	tx, ok := ctx.Value(transactionKey{}).(*sql.Tx)
	return tx, ok
}
