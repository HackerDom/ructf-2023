package db

import (
	"back/utils"
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

func (api *Api) CreateUserPair(name string) (string, error) {
	token, err := utils.RandomToken()
	if err != nil {
		return "", errors.New("can not gen random string: " + err.Error())
	}
	if err := api.db.Transaction(func(tx *gorm.DB) error {
		var userPairs []UserPairModel

		if err := tx.Find(&userPairs, "name = ?", name).Error; err != nil {
			return errors.New("can not get user pair: " + err.Error())
		}

		if len(userPairs) != 0 {
			return errors.New("can not create user due to duplicated name")
		}

		if err := tx.Create(&UserPairModel{
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

func (api *Api) IsUserPairValid(name, token string) bool {
	var userPair UserPairModel
	if err := api.db.First(&userPair, "name = ?", name).Error; err != nil {
		return false
	}

	return utils.GetSHA1Hash(token) == userPair.TokenHash
}

func (api *Api) CreateAsmCodeModel(owner, storageKey string) (string, error) {
	codeUuid := uuid.New().String()
	if err := api.db.Create(&AsmCodeModel{
		UUID:       codeUuid,
		Owner:      owner,
		StorageKey: storageKey,
	}).Error; err != nil {
		return "", errors.New("can not create asm code: " + err.Error())
	}
	return codeUuid, nil
}
