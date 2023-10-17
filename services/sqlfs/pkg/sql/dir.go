package sql

import (
	"context"
	"database/sql"
	"syscall"

	"github.com/doug-martin/goqu/v9"
	"github.com/hanwen/go-fuse/v2/fs"
	"github.com/hanwen/go-fuse/v2/fuse"
)

type DirStream interface {
	fs.DirStream

	Init(ctx context.Context, path string) syscall.Errno
}

func NewDirStream(db *sql.DB) DirStream {
	return &dirStream{db: db}
}

type dirStream struct {
	db      *sql.DB
	rows    *sql.Rows
	hasNext bool
}

func (ds *dirStream) Init(ctx context.Context, path string) syscall.Errno {
	query, args, err := enriesTable.
		Join(goqu.T("nodes"), goqu.Using("ino")).
		Select("filename", "mode", "ino").
		Where(goqu.C("path").Eq(path)).ToSQL()
	if err != nil {
		return syscall.EIO
	}

	rows, err := ds.db.QueryContext(ctx, query, args...)
	if err != nil {
		return syscall.EIO
	}
	ds.rows = rows
	ds.hasNext = rows.Next()

	return syscall.F_OK
}

func (ds *dirStream) HasNext() bool {
	return ds.hasNext
}

func (ds *dirStream) Next() (fuse.DirEntry, syscall.Errno) {
	if !ds.hasNext {
		return fuse.DirEntry{}, syscall.EIO
	}

	var entry fuse.DirEntry
	if err := ds.rows.Scan(&entry.Name, &entry.Mode, &entry.Ino); err != nil {
		return fuse.DirEntry{}, syscall.EIO
	}
	ds.hasNext = ds.rows.Next()

	return entry, syscall.F_OK
}

func (ds *dirStream) Close() {
	_ = ds.rows.Close()
}
