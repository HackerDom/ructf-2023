package server

import (
	"context"
	"errors"
	"fmt"
	"github.com/redis/go-redis/v9"
	"time"
)

type DBApi struct {
	client *redis.Client
}

func NewDBApi(config *Config) *DBApi {
	client := redis.NewClient(&redis.Options{
		Addr:     fmt.Sprintf("%v:%v", config.RedisHost, config.RedisPort),
		Password: "", // no password set
		DB:       0,  // use default DB
	})

	return &DBApi{
		client: client,
	}
}

func (dbApi *DBApi) get2(key string) (string, error) {
	return dbApi.client.Get(context.Background(), key).Result()
}

func (dbApi *DBApi) set(key string, value interface{}) *redis.StatusCmd {
	return dbApi.client.Set(context.Background(), key, value, time.Hour)
}

func (dbApi *DBApi) Register(username, paymentInfo string) error {
	if _, err := dbApi.get2("info/" + username); err != redis.Nil {
		return errors.New("user is already exist")
	}

	if err := dbApi.set("info/"+username, paymentInfo).Err(); err != nil {
		return errors.New("can not set user info: " + err.Error())
	}

	return nil
}

func (dbApi *DBApi) IssueData(username string) (string, string, error) {
	luckyNumbers := RandomLuckyNumbers()
	salt := RandomString(20)

	if err := dbApi.set("lucky/"+username, fmt.Sprintf("%v", luckyNumbers)).Err(); err != nil {
		return "", "", errors.New("can not write user salt: " + err.Error())
	}

	if err := dbApi.set("salt/"+username, salt).Err(); err != nil {
		return "", "", errors.New("can not write user salt: " + err.Error())
	}

	return GetBaseHash(username + salt), fmt.Sprintf("%v", luckyNumbers), nil
}

func (dbApi *DBApi) IsValidSecret(username, token string) bool {
	salt, err := dbApi.get2("salt/" + username)
	if err != nil {
		return false
	}

	return GetBaseHash(username+salt) == token
}

func (dbApi *DBApi) GetInfo(username string) (string, error) {
	userInfo, err := dbApi.get2("info/" + username)
	if err != nil {
		return "", errors.New("can not find user info")
	}

	return userInfo, nil
}

func (dbApi *DBApi) GetLuckyNumbers(username string) (string, error) {
	luckyNumbers, err := dbApi.get2("lucky/" + username)
	if err != nil {
		return "", errors.New("can not find user lucky numbers")
	}

	return luckyNumbers, nil
}
