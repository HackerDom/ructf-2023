package store

import (
	"context"
	"database/sql"
	"fmt"
	"sqlfs/pkg/model"
	"syscall"
	"time"

	"github.com/doug-martin/goqu/v9"
)

func (s *store) GetNodeCount(ctx context.Context) (uint64, error) {
	query, args, err := nodesTable.Select(goqu.COUNT("*")).ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var nodeCount uint64
	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&nodeCount); err != nil {
		return 0, fmt.Errorf("failed to execute query %s: %w", query, err)
	}

	return nodeCount, nil
}

func (s *store) GetNodesTotalSize(ctx context.Context) (uint64, error) {
	query, args, err := nodesTable.Select(goqu.SUM("size")).ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var totalSize uint64
	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&totalSize); err != nil {
		return 0, fmt.Errorf("failed to execute query %s: %w", query, err)
	}

	return totalSize, nil
}

func (s *store) GetNode(ctx context.Context, ino uint64) (*model.Node, error) {
	query, args, err := nodesTable.Where(goqu.C("ino").Eq(ino)).ToSQL()
	if err != nil {
		return nil, fmt.Errorf("failed to build query: %w", err)
	}

	var node model.Node
	row := s.QueryRowContext(ctx, query, args...)
	if err := scanNode(&node, row); err != nil {
		return nil, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return &node, nil
}

func (s *store) CreateNode(ctx context.Context, mode uint32, size uint64, nlink uint64) (*model.Node, error) {
	now := time.Now()

	node := &model.Node{
		Dev:              70000,
		Ino:              0,
		Nlink:            nlink,
		Mode:             mode,
		Uid:              1000,
		Gid:              1000,
		Rdev:             0,
		Size:             size,
		Blksize:          4096,
		Blocks:           8,
		AccessTime:       now,
		ModifyTime:       now,
		StatusChangeTime: now,
	}

	query, args, err := nodesTable.Insert().Rows(goqu.Record{
		"dev":        node.Dev,
		"nlink":      node.Nlink,
		"mode":       node.Mode,
		"uid":        node.Uid,
		"gid":        node.Gid,
		"rdev":       node.Rdev,
		"size":       node.Size,
		"blksize":    node.Blksize,
		"blocks":     node.Blocks,
		"atime_sec":  node.AccessTime.Unix(),
		"atime_nsec": node.AccessTime.Nanosecond(),
		"mtime_sec":  node.ModifyTime.Unix(),
		"mtime_nsec": node.ModifyTime.Nanosecond(),
		"ctime_sec":  node.StatusChangeTime.Unix(),
		"ctime_nsec": node.StatusChangeTime.Nanosecond(),
	}).Returning("ino").ToSQL()
	if err != nil {
		return nil, syscall.EIO
	}

	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&node.Ino); err != nil {
		return nil, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return node, nil
}

func (s *store) TryDeleteZeroNlinkNode(ctx context.Context, ino uint64) (bool, error) {
	query, args, err := nodesTable.Delete().
		Where(goqu.And(
			goqu.C("ino").Eq(ino),
			goqu.C("nlink").Lt(1),
		)).ToSQL()
	if err != nil {
		return false, fmt.Errorf("failed to build query: %w", err)
	}

	r, err := s.ExecContext(ctx, query, args...)
	if err != nil {
		return false, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	deleted, err := r.RowsAffected()
	if err != nil {
		return false, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return deleted == 1, nil
}

func (s *store) GetNodeByEntry(ctx context.Context, path, name string) (*model.Node, error) {
	query, args, err := nodesTable.
		Select(
			"dev", "ino", "nlink", "mode", "uid", "gid", "rdev", "size", "blksize", "blocks",
			"atime_sec", "atime_nsec", "mtime_sec", "mtime_nsec", "ctime_sec", "ctime_nsec",
		).
		Join(
			goqu.T("entries"),
			goqu.Using("ino"),
		).Where(goqu.Ex{
		"path":     path,
		"filename": name,
	}).ToSQL()
	if err != nil {
		return nil, fmt.Errorf("failed to build query: %w", err)
	}

	var node model.Node
	row := s.QueryRowContext(ctx, query, args...)
	if err := scanNode(&node, row); err != nil {
		return nil, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return &node, nil
}

func scanNode(node *model.Node, row *sql.Row) error {
	var atimeNsec, atimeSec, mtimeNsec, mtimeSec, ctimeNsec, ctimeSec int64

	if err := row.Scan(
		&node.Dev,
		&node.Ino,
		&node.Nlink,
		&node.Mode,
		&node.Uid,
		&node.Gid,
		&node.Rdev,
		&node.Size,
		&node.Blksize,
		&node.Blocks,
		&atimeSec,
		&atimeNsec,
		&mtimeSec,
		&mtimeNsec,
		&ctimeSec,
		&ctimeNsec,
	); err != nil {
		return fmt.Errorf("failed to scan node: %w", err)
	}

	node.AccessTime = time.Unix(atimeSec, atimeNsec)
	node.ModifyTime = time.Unix(mtimeSec, mtimeNsec)
	node.StatusChangeTime = time.Unix(ctimeSec, ctimeNsec)

	return nil
}
