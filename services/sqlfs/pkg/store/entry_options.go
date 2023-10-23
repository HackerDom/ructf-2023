package store

import "github.com/doug-martin/goqu/v9"

type Option func(*goqu.SelectDataset) *goqu.SelectDataset

func WithPath(path string) Option {
	return func(ds *goqu.SelectDataset) *goqu.SelectDataset {
		return ds.Where(goqu.C("path").Eq(path))
	}
}

func WithName(name string) Option {
	return func(ds *goqu.SelectDataset) *goqu.SelectDataset {
		return ds.Where(goqu.C("filename").Eq(name))
	}
}
