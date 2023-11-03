package dirstream

import (
	"context"
	"sqlfs/pkg/store"
	"sync/atomic"
	"syscall"

	"github.com/hanwen/go-fuse/v2/fs"
	"github.com/hanwen/go-fuse/v2/fuse"
	"github.com/rs/zerolog"
)

type DirStream interface {
	fs.DirStream

	Init(_ context.Context, path string) syscall.Errno
}

func New(iter store.EntriesIter, logger zerolog.Logger) DirStream {
	return &dirStream{iter: iter}
}

type dirStream struct {
	iter   store.EntriesIter
	logger zerolog.Logger

	callCounter atomic.Int32
}

func (ds *dirStream) Init(ctx context.Context, path string) syscall.Errno {
	if err := ds.iter.Init(ctx, path); err != nil {
		ds.logger.Err(err).Msg("failed to init entries iter")
		return syscall.EIO
	}

	return syscall.F_OK
}

func (ds *dirStream) HasNext() bool {
	return ds.iter.HasNext()
}

func (ds *dirStream) Next() (fuse.DirEntry, syscall.Errno) {
	entry, err := ds.iter.Next()
	if err != nil {
		ds.logger.Err(err).Msg("failed to get next entry")
		return fuse.DirEntry{}, syscall.EIO
	}

	return fuse.DirEntry{
		Mode: entry.Mode,
		Name: entry.Name,
		Ino:  entry.Ino,
	}, syscall.F_OK
}

func (ds *dirStream) Close() {
	if err := ds.iter.Close(); err != nil {
		ds.logger.Err(err).Msg("failed to close entries iter")
	}
}
