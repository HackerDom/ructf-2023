package store

import (
	"context"
	"fmt"
	"sqlfs/pkg/model"
	"time"

	"github.com/doug-martin/goqu/v9"
)

func (s *store) GetNodeCount(ctx context.Context) (uint64, error) {
	query, args, err := nodesTable.Select(goqu.COUNT("*")).ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var nodeCount uint64
	row := s.baseStore.QueryRowContext(ctx, query, args...)
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
	row := s.baseStore.QueryRowContext(ctx, query, args...)
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
	row := s.baseStore.QueryRowContext(ctx, query, args...)
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
		return nil, err
	}

	node.Atime = time.Unix(atimeSec, atimeNsec)
	node.Mtime = time.Unix(mtimeSec, mtimeNsec)
	node.Ctime = time.Unix(ctimeSec, ctimeNsec)

	return &node, nil
}
