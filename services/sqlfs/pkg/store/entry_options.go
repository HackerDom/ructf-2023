package store

import "github.com/doug-martin/goqu/v9"

type SelectOption func(*goqu.SelectDataset) *goqu.SelectDataset
type UpdateOption func(*goqu.UpdateDataset) *goqu.UpdateDataset

func WithPath(path string) SelectOption {
	return func(ds *goqu.SelectDataset) *goqu.SelectDataset {
		return ds.Where(goqu.C("path").Eq(path))
	}
}

func WithName(name string) SelectOption {
	return func(ds *goqu.SelectDataset) *goqu.SelectDataset {
		return ds.Where(goqu.C("filename").Eq(name))
	}
}
