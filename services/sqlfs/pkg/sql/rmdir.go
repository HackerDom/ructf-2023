package sql

import (
	"context"
	"database/sql"
	xpath "path"
	"syscall"

	"github.com/doug-martin/goqu/v9"
)

func Rmdir(ctx context.Context, path, name string, db *sql.DB) syscall.Errno {
	tx, err := db.BeginTx(ctx, nil)
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	p := xpath.Join(path, name)
	query, args, err := enriesTable.Select(goqu.COUNT("*")).
		Where(goqu.C("path").Eq(p)).
		ToSQL()

	var count int
	row := tx.QueryRowContext(ctx, query, args...)
	if err = row.Scan(&count); err != nil {
		_ = tx.Rollback()
	}

	if count > 0 {
		_ = tx.Rollback()
		return syscall.ENOTEMPTY
	}

	if unlinkErr := unlink(ctx, path, name, 1, tx); unlinkErr != syscall.F_OK {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	return syscall.F_OK
}
