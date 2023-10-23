package store

import (
	"context"
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
	var atimeNsec, atimeSec, mtimeNsec, mtimeSec, ctimeNsec, ctimeSec int64
	row := s.QueryRowContext(ctx, query, args...)
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
		return nil, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	node.AccessTime = time.Unix(atimeSec, atimeNsec)
	node.ModifyTime = time.Unix(mtimeSec, mtimeNsec)
	node.StatusChangeTime = time.Unix(ctimeSec, ctimeNsec)

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

func (s *store) DecrementNodeNlink(ctx context.Context, ino uint64) (uint64, error) {
	query, args, err := nodesTable.Update().Set(
		goqu.Record{"nlink": goqu.L("nlink - 1")},
	).Where(goqu.C("ino").Eq(ino)).Returning("nlink").ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var nlink uint64
	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&nlink); err != nil {
		return 0, fmt.Errorf("failed to execute query %s: %w", query, err)
	}

	return nlink, nil
}

func (s *store) DeleteNode(ctx context.Context, ino uint64) error {
	query, args, err := nodesTable.Delete().
		Where(goqu.C("ino").Eq(ino)).ToSQL()
	if err != nil {
		return fmt.Errorf("failed to build query: %w", err)
	}

	r, err := s.ExecContext(ctx, query, args...)
	if err != nil {
		return fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	deleted, err := r.RowsAffected()
	if err != nil {
		return fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	if deleted != 1 {
		return fmt.Errorf("failed to delete node with query %s: zero rows affected", query)
	}

	return nil
}
