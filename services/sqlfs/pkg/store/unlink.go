package store

import (
	"context"
	"database/sql"
	"errors"
	"syscall"

	"github.com/doug-martin/goqu/v9"
)

// remove users
func Unlink(ctx context.Context, path, name string, deleteNlink int, db *sql.DB) syscall.Errno {
	tx, err := db.BeginTx(ctx, nil)
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if unlinkErr := unlink(ctx, path, name, deleteNlink, tx); unlinkErr != syscall.F_OK {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	return syscall.F_OK
}

func unlink(ctx context.Context, path, name string, deleteNlink int, tx *sql.Tx) syscall.Errno {
	query, args, err := enriesTable.Delete().Where(goqu.Ex{
		"path":     path,
		"filename": name,
	}).Returning("ino").ToSQL()
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	var ino int
	row := tx.QueryRowContext(ctx, query, args...)
	err = row.Scan(&ino)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		_ = tx.Rollback()
		return syscall.ENOENT
	case err == nil:
	default:
		_ = tx.Rollback()
		return syscall.EIO
	}

	query, args, err = nodesTable.Update().Set(
		goqu.Record{"nlink": goqu.L("nlink - 1")},
	).Where(goqu.C("ino").Eq(ino)).
		Returning("nlink").ToSQL()
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	var nlink int
	row = tx.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&nlink); err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if nlink != deleteNlink {
		if err := tx.Commit(); err != nil {
			_ = tx.Rollback()
			return syscall.EIO
		}

		return syscall.F_OK
	}

	query, args, err = nodesTable.Delete().
		Where(goqu.C("ino").Eq(ino)).ToSQL()
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	r, err := tx.ExecContext(ctx, query, args...)
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	deleted, err := r.RowsAffected()
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if deleted != 1 {
		_ = tx.Rollback()
		return syscall.EIO
	}

	return syscall.F_OK
}
