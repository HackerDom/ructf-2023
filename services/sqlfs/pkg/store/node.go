package store

import (
	"context"
	"fmt"

	"github.com/doug-martin/goqu/v9"
)

func (s *store) GetNodeCount(ctx context.Context) (uint64, error) {
	query, args, err := nodesTable.Select(goqu.COUNT("*")).ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var nodeCount uint64
	row := s.db.QueryRowContext(ctx, query, args...)
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
	row := s.db.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&totalSize); err != nil {
		return 0, fmt.Errorf("failed to execute query %s: %w", query, err)
	}

	return totalSize, nil
}
