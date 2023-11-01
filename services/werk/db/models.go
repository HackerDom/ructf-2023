package db

import "gorm.io/gorm"

type UserPairModel struct {
	gorm.Model
	Name      string `gorm:"uniqueIndex"`
	TokenHash string
}

type AsmCodeModel struct {
	gorm.Model
	UUID       string `gorm:"uniqueIndex"`
	Owner      string
	StorageKey string
}

type RunModel struct {
	gorm.Model
	ImageUUID string
	Owner     string
	Vd        uint64
}
