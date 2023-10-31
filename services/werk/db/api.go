package db

import (
	"back/models"
	"back/utils"
	"errors"
	"gorm.io/gorm"
)

const tokenLength = 32

type Api struct {
	db *gorm.DB
}

func NewApi(db *gorm.DB) *Api {
	return &Api{db: db}
}

func (api *Api) CreateUserPair(name string) (string, error) {
	token, err := utils.RandomString(tokenLength)
	if err != nil {
		return "", errors.New("can not gen random string: " + err.Error())
	}
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
			TokenHash: utils.GetSHA1Hash(token),
		}).Error; err != nil {
			return errors.New("can not create user: " + err.Error())
		}
		return nil
	}); err != nil {
		return "", err
	}
	return token, nil
}
