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
		return 0, fmt.Errorf("failed to exec query %s: %w", query, err)
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
		return fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return nil
}

func (s *store) GetEntriesCount(ctx context.Context, options ...SelectOption) (uint64, error) {
	ds := enriesTable.Select(goqu.COUNT("*"))

	for _, opt := range options {
		ds = opt(ds)
	}

	query, args, err := ds.ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var count uint64
	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&count); err != nil {
		return 0, fmt.Errorf("failed to execute query %s: %w", query, err)
	}

	return count, nil
}

func (s *store) DeleteEntries(ctx context.Context, path, name string) (uint64, error) {
	query, args, err := enriesTable.Delete().Where(goqu.Ex{
		"path":     path,
		"filename": name,
	}).Returning("ino").ToSQL()
	if err != nil {
		return 0, fmt.Errorf("failed to build query: %w", err)
	}

	var ino uint64
	row := s.QueryRowContext(ctx, query, args...)
	if err := row.Scan(&ino); err != nil {
		return 0, fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return ino, nil
}

func (s *store) UpdateEntries(
	ctx context.Context,
	currentPath, newPath string,
	mask *UpdateEntryMask,
	opts ...SelectOption,
) error {
	ds := enriesTable
	for _, opt := range opts {
		ds = opt(ds)
	}

	mask.record["path"] = newPath
	updateDs := ds.Update().Set(mask.record).Where(goqu.Ex{"path": currentPath})

	query, args, err := updateDs.ToSQL()
	if err != nil {
		return fmt.Errorf("failed to build query: %w", err)
	}

	if _, err := s.ExecContext(ctx, query, args...); err != nil {
		return fmt.Errorf("failed to exec query %s: %w", query, err)
	}

	return nil
}
