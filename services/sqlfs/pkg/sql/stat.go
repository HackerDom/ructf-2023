package sql

import (
	"context"
	"database/sql"
	"syscall"

	"github.com/doug-martin/goqu/v9"
)

const (
	GB            = 1024 * 1024 * 1024
	FS_SIZE_LIMIT = 5 * GB
	FS_BLOCK_SIZE = 4096
)

func GetStatfs(ctx context.Context, db *sql.DB) (*syscall.Statfs_t, syscall.Errno) {
	st := &syscall.Statfs_t{
		Type:   0xEF53,
		Bsize:  FS_BLOCK_SIZE,
		Blocks: FS_SIZE_LIMIT / FS_BLOCK_SIZE,
		Fsid: syscall.Fsid{
			X__val: [2]int32{0xEF53, 0xCAFEBAB},
		},
		Namelen: 50,
		Frsize:  4096,
	}

	query, args, err := nodesTable.Select(goqu.COUNT("*"), goqu.SUM("size")).ToSQL()
	if err != nil {
		return nil, syscall.EIO
	}

	var nodesCount, totalSize uint64
	row := db.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&nodesCount, &totalSize); err != nil {
		return nil, syscall.EIO
	}

	st.Bfree = st.Blocks - (totalSize / FS_BLOCK_SIZE)
	if totalSize%FS_BLOCK_SIZE != 0 {
		st.Bfree -= 1
	}

	st.Bavail = st.Bfree
	st.Files = nodesCount
	st.Ffree = (FS_SIZE_LIMIT-GB)/MAX_FILESIZE - nodesCount

	return st, syscall.F_OK
}
