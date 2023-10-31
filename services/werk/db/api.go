package db

import (
	"back/models"
	"crypto/md5"
	"encoding/hex"
	"errors"
	"github.com/google/uuid"
	"gorm.io/gorm"
)

type Api struct {
	db *gorm.DB
}

func NewApi(db *gorm.DB) *Api {
	return &Api{db: db}
}

func GetMD5Hash(text string) string {
	hash := md5.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}

func (api *Api) CreateUserPair(name string) (string, error) {
	token := uuid.New().String()
	if err := api.db.Transaction(func(tx *gorm.DB) error {
		var userPairs []models.UserPairModel

		if err := tx.Find(&userPairs, "name = ?", name).Error; err != nil {
			return errors.New("can not get user pair: " + err.Error())
		}

		if len(userPairs) != 0 {
			return errors.New("can not create user due to duplicated name")
		}

		if err := tx.Create(&models.UserPairModel{
			Name:      name,
			TokenHash: GetMD5Hash(token),
		}).Error; err != nil {
			return errors.New("can not create user: " + err.Error())
		}
		return nil
	}); err != nil {
		return "", err
	}
	return token, nil
}
