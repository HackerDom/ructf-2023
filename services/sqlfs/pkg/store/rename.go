package store

import (
	"context"
	"database/sql"
	"errors"
	"syscall"

	"github.com/doug-martin/goqu/v9"
	"github.com/doug-martin/goqu/v9/exp"
)

func Rename(ctx context.Context, path, name, newPath, newName string, db *sql.DB) syscall.Errno {
	query, args, err := enriesTable.ForUpdate(exp.Wait, goqu.T("entries")).
		Select(goqu.L("1 = 1")).
		Where(goqu.Ex{"path": newPath, "filename": newName}).ToSQL()
	if err != nil {
		return syscall.EIO
	}

	tx, err := db.BeginTx(ctx, nil)
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	row := tx.QueryRowContext(ctx, query, args...)
	var a bool
	err = row.Scan(&a)
	switch {
	case err == nil:
		_ = tx.Rollback()
		return syscall.EEXIST
	case errors.Is(err, sql.ErrNoRows):
	default:
		_ = tx.Rollback()
		return syscall.EIO
	}

	query, args, err = enriesTable.Update().Set(goqu.Record{
		"path":     newPath,
		"filename": newName,
	}).Where(goqu.Ex{
		"path":     path,
		"filename": name,
	}).ToSQL()
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if _, err := tx.ExecContext(ctx, query, args...); err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	return syscall.F_OK
}
