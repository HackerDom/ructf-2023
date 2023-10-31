package models

import "gorm.io/gorm"

type UserPairModel struct {
	gorm.Model
	Name      string `gorm:"uniqueIndex"`
	TokenHash string
}
