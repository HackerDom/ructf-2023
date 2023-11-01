package db

import (
	"back/config"
	"back/utils"
	"errors"
	"fmt"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
)

type Api struct {
	db *gorm.DB
}

func NewApi(cfg *config.Postgres) (*Api, error) {
	dsn := fmt.Sprintf(
		"host=%v user=%v password=%v dbname=%v port=%v sslmode=disable",
		cfg.Host,
		cfg.Username,
		cfg.Password,
		cfg.Database,
		cfg.Port,
	)
	gormDb, err := gorm.Open(postgres.Open(dsn), &gorm.Config{})

	if err != nil {
		return nil, errors.New("can not open gorm db: " + err.Error())
	}
	if err := gormDb.AutoMigrate(
		UserPairModel{},
		ImageModel{},
		RunModel{},
	); err != nil {
		return nil, errors.New("can not migrate gorm db: " + err.Error())
	}

	sqlDb, err := gormDb.DB()
	if err != nil {
		return nil, errors.New("can not get sql db from gorm db")
	}

	sqlDb.SetMaxIdleConns(int(cfg.MaxIdleConns))
	sqlDb.SetMaxOpenConns(int(cfg.MaxOpenConns))

	return &Api{db: gormDb}, nil
}

func (api *Api) CreateUserPair(name string) (string, error) {
	token := utils.RandomToken()
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

func (api *Api) CreateImageModel(owner, storageKey string) (uint, error) {
	imageModel := &ImageModel{
		Owner:      owner,
		StorageKey: storageKey,
	}
	if err := api.db.Create(imageModel).Error; err != nil {
		return 0, errors.New("can not create asm code: " + err.Error())
	}
	return imageModel.ID, nil
}

func (api *Api) IsImageOwnerCorrect(name string, imageId uint) bool {
	var imageModel ImageModel
	if err := api.db.First(&imageModel, imageId).Error; err != nil {
		return false
	}

	return imageModel.Owner == name
}

func (api *Api) IsRunOwnerCorrect(name string, runId uint) bool {
	var runModel RunModel
	if err := api.db.First(&runModel, runId).Error; err != nil {
		return false
	}

	return runModel.Owner == name
}

func (api *Api) CreateRunModel(owner string, imageId uint) (uint, error) {
	runModel := RunModel{
		ImageId: imageId,
		Owner:   owner,
	}
	res := api.db.Create(&runModel)

	if res.Error != nil {
		return 0, errors.New("can not create run: " + res.Error.Error())
	}

	return runModel.ID, nil
}
