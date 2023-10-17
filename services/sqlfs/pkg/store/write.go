package store

import (
	"bytes"
	"context"
	"database/sql"
	"errors"
	"strconv"
	"syscall"
	"time"

	"github.com/doug-martin/goqu/v9"
)

const MAX_FILESIZE = 100

func Write(ctx context.Context, data []byte, off int64, ino int, db *sql.DB) (uint32, syscall.Errno) {
	if off != 0 {
		return 0, syscall.EINVAL
	}

	tx, err := db.BeginTx(ctx, nil)
	if err != nil {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	sizeBeforeWrite, err := getFileSize(ctx, ino, tx)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		_ = tx.Rollback()
		return 0, syscall.ENOENT
	case err == nil:
	default:
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	if sizeBeforeWrite+len(data) > MAX_FILESIZE {
		_ = tx.Rollback()
		return 0, syscall.ENOSPC
	}

	lines := bytes.Split(data, []byte("\n"))
	if len(lines) > 0 {
		lines = lines[:len(lines)-1]
	}
	rows := make([]goqu.Record, 0)

	for _, line := range lines {
		splitted := bytes.SplitN(line, []byte(","), 5)
		if len(splitted) < 5 {
			_ = tx.Rollback()
			return 0, syscall.EINVAL
		}

		age, err := strconv.Atoi(string(splitted[2]))
		if err != nil {
			_ = tx.Rollback()
			return 0, syscall.EINVAL
		}

		var private bool
		switch {
		case string(splitted[4]) == "true":
			private = true
		case string(splitted[4]) == "false":
			private = false
		default:
			_ = tx.Rollback()
			return 0, syscall.EINVAL
		}

		rows = append(rows, goqu.Record{
			"first_name": splitted[0],
			"last_name":  splitted[1],
			"age":        age,
			"email":      splitted[3],
			"ino":        ino,
			"private":    private,
		})
	}

	if len(rows) == 0 {
		_ = tx.Rollback()
		return 0, syscall.EINVAL
	}

	query, args, err := usersTable.Insert().Rows(rows).ToSQL()
	if err != nil {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	if _, err := tx.ExecContext(ctx, query, args...); err != nil {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	err = updateFileSize(ctx, ino, sizeBeforeWrite+len(data), tx)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		_ = tx.Rollback()
		return 0, syscall.ENOENT
	case err == nil:
	default:
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	now := time.Now()
	nowTimespec := syscall.Timespec{
		Sec:  now.Unix(),
		Nsec: int64(now.Nanosecond()),
	}
	if err := updateTimes(ctx, ino, []syscall.Timespec{nowTimespec, nowTimespec}, tx); err != syscall.F_OK {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	return uint32(len(data)), syscall.F_OK
}
