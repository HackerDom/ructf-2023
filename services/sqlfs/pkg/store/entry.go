package store

import (
	"context"
	"fmt"

	"github.com/doug-martin/goqu/v9"
)

func (s *store) GetNodeIno(ctx context.Context, path string) (uint64, error) {
	path, file := split(path)

	query, args, err := enriesTable.Select("ino").Where(goqu.Ex{
		"path":     path,
		"filename": file,
	}).ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var ino uint64
	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&ino); err != nil {
		return 0, fmt.Errorf("failed to exec query: %w", err)
	}

	return ino, nil
}

func (s *store) CreateEntry(ctx context.Context, path, filename string, ino uint64) error {
	query, args, err := enriesTable.Insert().Rows(goqu.Record{
		"path":     path,
		"filename": filename,
		"ino":      ino,
	}).ToSQL()
	if err != nil {
		return fmt.Errorf("failed to build query: %w", err)
	}

	if _, err := s.ExecContext(ctx, query, args...); err != nil {
		return fmt.Errorf("failed to exec query: %w", err)
	}

	return nil
}
