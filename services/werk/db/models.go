package db

import "gorm.io/gorm"

type UserPairModel struct {
	gorm.Model
	Name      string `gorm:"uniqueIndex"`
	TokenHash string
}

type ImageModel struct {
	gorm.Model
	Owner      string
	StorageKey string
}

type RunModel struct {
	gorm.Model
	ImageId uint
	Owner   string
	Vd      uint64
}
