package store

import (
	"context"
	"database/sql"
	"errors"
	xpath "path"
	"strings"
	"syscall"
	"time"

	"github.com/doug-martin/goqu/v9"
)

func Create(
	ctx context.Context,
	path string,
	mode uint32,
	size int64,
	nlink uint64,
	db *sql.DB,
) (st *syscall.Stat_t, errno syscall.Errno) {
	ino, err := GetIno(ctx, path, db)
	if err != syscall.F_OK && err != syscall.ENOENT {
		return nil, err
	}

	st, getFileErr := getFile(ctx, ino, db)
	if errors.Is(getFileErr, sql.ErrNoRows) {
		var err syscall.Errno
		st, err = createFile(ctx, path, mode, size, nlink, db)
		if err != syscall.F_OK {
			return nil, err
		}
	} else if getFileErr != nil {
		return nil, syscall.EIO
	}

	return st, syscall.F_OK
}

func createFile(
	ctx context.Context,
	path string,
	mode uint32,
	size int64,
	nlink uint64,
	db *sql.DB,
) (*syscall.Stat_t, syscall.Errno) {
	now := time.Now()
	nowTimespec := syscall.Timespec{
		Sec:  now.Unix(),
		Nsec: int64(now.Nanosecond()),
	}

	st := &syscall.Stat_t{
		Dev:       70000,
		Ino:       0,
		Nlink:     nlink,
		Mode:      mode,
		Uid:       1000,
		Gid:       1000,
		X__pad0:   0,
		Rdev:      0,
		Size:      size,
		Blksize:   4096,
		Blocks:    8,
		Atim:      nowTimespec,
		Mtim:      nowTimespec,
		Ctim:      nowTimespec,
		X__unused: [3]int64{},
	}

	path, filename := split(path)

	tx, err := db.BeginTx(ctx, nil)
	if err != nil {
		_ = tx.Rollback()
		return nil, syscall.EIO
	}

	query, args, err := nodesTable.Insert().Rows(goqu.Record{
		"dev":        st.Dev,
		"nlink":      st.Nlink,
		"mode":       st.Mode,
		"uid":        st.Uid,
		"gid":        st.Gid,
		"rdev":       st.Rdev,
		"size":       st.Size,
		"blksize":    st.Blksize,
		"blocks":     st.Blocks,
		"atime_sec":  st.Atim.Sec,
		"atime_nsec": st.Atim.Nsec,
		"mtime_sec":  st.Mtim.Sec,
		"mtime_nsec": st.Mtim.Nsec,
		"ctime_sec":  st.Ctim.Sec,
		"ctime_nsec": st.Ctim.Nsec,
	}).Returning("ino").ToSQL()
	if err != nil {
		return nil, syscall.EIO
	}

	row := tx.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&st.Ino); err != nil {
		_ = tx.Rollback()
		return nil, syscall.EIO
	}

	query, args, err = enriesTable.Insert().Rows(goqu.Record{
		"path":     path,
		"filename": filename,
		"ino":      st.Ino,
	}).ToSQL()
	if err != nil {
		return nil, syscall.EIO
	}

	if _, err := tx.ExecContext(ctx, query, args...); err != nil {
		_ = tx.Rollback()
		return nil, syscall.EIO
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return nil, syscall.EIO
	}

	return st, syscall.F_OK
}

func getFile(ctx context.Context, ino uint64, db *sql.DB) (*syscall.Stat_t, error) {
	query, args, err := nodesTable.Where(goqu.C("ino").Eq(ino)).ToSQL()
	if err != nil {
		return nil, err
	}

	row := db.QueryRowContext(ctx, query, args...)
	var st syscall.Stat_t
	st.Atim = syscall.Timespec{}
	st.Mtim = syscall.Timespec{}
	st.Ctim = syscall.Timespec{}
	if err := row.Scan(
		&st.Dev,
		&st.Ino,
		&st.Nlink,
		&st.Mode,
		&st.Uid,
		&st.Gid,
		&st.Rdev,
		&st.Size,
		&st.Blksize,
		&st.Blocks,
		&st.Atim.Sec,
		&st.Atim.Nsec,
		&st.Mtim.Sec,
		&st.Mtim.Nsec,
		&st.Ctim.Sec,
		&st.Ctim.Nsec,
	); err != nil {
		return nil, err
	}

	return &st, nil
}

func GetIno(ctx context.Context, path string, db *sql.DB) (uint64, syscall.Errno) {
	tx, err := db.Begin()
	if err != nil {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	ino, getInoErr := getIno(ctx, path, tx)
	if getInoErr != syscall.F_OK {
		_ = tx.Rollback()
		return 0, getInoErr
	}

	if err := tx.Commit(); err != nil {
		_ = tx.Rollback()
		return 0, syscall.EIO
	}

	return uint64(ino), syscall.F_OK
}

func getIno(ctx context.Context, path string, tx *sql.Tx) (int, syscall.Errno) {
	path, file := split(path)

	query, args, err := enriesTable.Select("ino").Where(goqu.Ex{
		"path":     path,
		"filename": file,
	}).ToSQL()
	if err != nil {
		return 0, syscall.EIO
	}

	row := tx.QueryRowContext(ctx, query, args...)
	var ino int
	err = row.Scan(&ino)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		return 0, syscall.ENOENT
	case err == nil:
		return ino, syscall.F_OK
	default:
		return 0, syscall.EIO
	}
}

func split(path string) (dir, file string) {
	dir, file = xpath.Split(path)
	return strings.TrimSuffix(dir, "/"), file
}
