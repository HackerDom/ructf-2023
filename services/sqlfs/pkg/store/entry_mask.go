package store

import "github.com/doug-martin/goqu/v9"

type UpdateEntryMask struct {
	record goqu.Record
}

func NewUpdateEntryMask() *UpdateEntryMask {
	return &UpdateEntryMask{record: goqu.Record{}}
}

func (m *UpdateEntryMask) Name(name string) *UpdateEntryMask {
	mask := UpdateEntryMask{record: m.record}
	mask.record["filename"] = name
	return &mask
}
