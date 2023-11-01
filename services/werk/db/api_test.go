package db

import (
	"back/config"
	"back/utils"
	"github.com/stretchr/testify/assert"
	"testing"
)

var testPgConfig = &config.Postgres{
	Host:         "localhost",
	Port:         5432,
	Database:     "db",
	Username:     "postgres",
	Password:     "password",
	MaxIdleConns: 10,
	MaxOpenConns: 10,
}

func TestSimpleScenario(t *testing.T) {
	dbApi, err := NewApi(testPgConfig)
	assert.NoError(t, err)

	username1 := utils.RandomUsername()
	username2 := utils.RandomUsername()

	token1, err := dbApi.CreateUserPair(username1)
	assert.NoError(t, err)

	token2, err := dbApi.CreateUserPair(username2)
	assert.NoError(t, err)

	assert.True(t, dbApi.IsUserPairValid(username1, token1))
	assert.False(t, dbApi.IsUserPairValid(username2, token1))
	assert.False(t, dbApi.IsUserPairValid(username1, token2))
	assert.True(t, dbApi.IsUserPairValid(username2, token2))

	assert.False(t, dbApi.IsUserPairValid("wrong_username", token1))
	assert.False(t, dbApi.IsUserPairValid(username1, "wrong_token"))

	storageKey := utils.RandomStorageKey()
	imageId, err := dbApi.CreateImageModel(username1, storageKey)
	assert.NoError(t, err)

	assert.True(t, dbApi.IsImageOwnerCorrect(username1, imageId))
	assert.False(t, dbApi.IsImageOwnerCorrect(username2, imageId))
	assert.False(t, dbApi.IsImageOwnerCorrect("wrong_username", imageId))

	runId, err := dbApi.CreateRunModel(username1, imageId)
	assert.NoError(t, err)

	assert.True(t, dbApi.IsRunOwnerCorrect(username1, runId))
	assert.False(t, dbApi.IsRunOwnerCorrect(username2, runId))
	assert.False(t, dbApi.IsRunOwnerCorrect("wrong_username", runId))
	assert.False(t, dbApi.IsRunOwnerCorrect(username1, 983746598))
}
