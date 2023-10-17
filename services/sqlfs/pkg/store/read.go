package store

import (
	"database/sql"
	"fmt"
	"time"

	"github.com/doug-martin/goqu/v9"
	_ "github.com/lib/pq"

	"github.com/hanwen/go-fuse/v2/fuse"
)

type readResult struct {
	ino  int
	size int
	off  int64
	db   *sql.DB
}

func NewReadResult(ino int, size int, off int64, db *sql.DB) fuse.ReadResult {
	return &readResult{ino: ino, size: size, off: off, db: db}
}

// TODO update access time
func (r *readResult) Bytes(buf []byte) ([]byte, fuse.Status) {
	if r.size < len(buf) {
		r.size = len(buf)
	}

	query, args, err := usersTable.Select("first_name", "last_name", "age", "email").
		Where(goqu.C("ino").Eq(r.ino)).ToSQL()
	if err != nil {
		return nil, fuse.EIO
	}

	users, err := r.db.Query(query, args...)
	if err != nil {
		return nil, fuse.EIO
	}

	result := make([]byte, 0)
	for users.Next() {
		var firstName, lastName, email string
		var age int

		if err := users.Scan(&firstName, &lastName, &age, &email); err != nil {
			return nil, fuse.EIO
		}

		userStr := fmt.Sprintf("%s,%s,%d,%s\n", firstName, lastName, age, email)
		user := []byte(userStr)

		if len(result)+len(user) > r.size {
			break
		}

		result = append(result, user...)
	}

	atime := time.Now()
	query, args, err = nodesTable.Update().Set(goqu.Record{
		"atime_sec":  atime.Unix(),
		"atime_nsec": atime.Nanosecond(),
	}).
		Where(goqu.C("ino").Eq(r.ino)).
		ToSQL()
	if err != nil {
		return nil, fuse.EIO
	}

	if _, err := r.db.Exec(query, args...); err != nil {
		return nil, fuse.EIO
	}

	return result, fuse.OK
}

func (r *readResult) Size() int {
	return r.size
}

func (r *readResult) Done() {}
