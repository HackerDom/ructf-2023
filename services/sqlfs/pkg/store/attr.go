package store

import (
	"context"
	"database/sql"
	"errors"
	"syscall"

	"github.com/doug-martin/goqu/v9"
)

func UpdateTimes(ctx context.Context, path string, times []syscall.Timespec, db *sql.DB) syscall.Errno {
	tx, err := db.Begin()
	if err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	ino, getInoErr := getIno(ctx, path, tx)
	if getInoErr != syscall.F_OK {
		return getInoErr
	}

	if err := updateTimes(ctx, ino, times, tx); err != syscall.F_OK {
		_ = tx.Rollback()
		return err
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return syscall.EIO
	}

	return syscall.F_OK
}

// lstat
func GetStat(ctx context.Context, path string, db *sql.DB) (*syscall.Stat_t, syscall.Errno) {
	ino, getInoErr := GetIno(ctx, path, db)
	if getInoErr != syscall.F_OK {
		return nil, getInoErr
	}

	file, err := getFile(ctx, ino, db)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		return nil, syscall.ENOENT
	case err == nil:
		return file, syscall.F_OK
	default:
		return nil, syscall.EIO
	}
}

func getFileSize(ctx context.Context, ino int, db *sql.Tx) (int, error) {
	query, args, err := nodesTable.Select("size").Where(goqu.C("ino").Eq(ino)).ToSQL()
	if err != nil {
		return 0, err
	}

	row := db.QueryRowContext(ctx, query, args...)
	size := 0
	if err := row.Scan(&size); err != nil {
		return 0, err
	}

	return size, nil
}

func updateFileSize(ctx context.Context, ino int, size int, db *sql.Tx) error {
	query, args, err := nodesTable.Update().Set(goqu.Record{"size": size}).
		Where(goqu.C("ino").Eq(ino)).ToSQL()
	if err != nil {
		return err
	}

	if _, err := db.ExecContext(ctx, query, args...); err != nil {
		return err
	}

	return nil
}

func updateTimes(ctx context.Context, ino int, times []syscall.Timespec, tx *sql.Tx) syscall.Errno {
	query, args, err := nodesTable.Update().Set(goqu.Record{
		"atime_sec":  times[0].Sec,
		"atime_nsec": times[0].Nsec,
		"mtime_sec":  times[1].Sec,
		"mtime_nsec": times[1].Nsec,
	}).
		Where(goqu.C("ino").Eq(ino)).
		ToSQL()
	if err != nil {
		return syscall.EIO
	}

	if _, err := tx.ExecContext(ctx, query, args...); err != nil {
		return syscall.EIO
	}

	return syscall.F_OK
}
