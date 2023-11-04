package tests

import (
	"back/clients"
	"back/utils"
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestUserClientSimpleScenario(t *testing.T) {
	userClient := clients.NewUserClient(testUserClientConfig)

	username := utils.RandomUsername()
	token, err := userClient.Register(username)
	assert.NoError(t, err)

	_, err = userClient.CreateImage(username, token, "my text")
}
