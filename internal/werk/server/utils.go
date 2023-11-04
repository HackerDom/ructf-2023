package server

import (
	"crypto/sha1"
	"encoding/base64"
	"math/rand"
	"regexp"
)

var alpha = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890")

func RandomString(n int) string {
	b := make([]rune, n)
	for i := range b {
		b[i] = alpha[rand.Intn(len(alpha))]
	}
	return string(b)
}

func RandomLuckyNumbers() []int {
	var res []int
	for i := 0; i < 4; i++ {
		res = append(res, rand.Intn(1000))
	}
	return res
}

func GetBaseHash(data string) string {
	hasher := sha1.New()
	hasher.Write([]byte(data))
	return base64.URLEncoding.EncodeToString(hasher.Sum(nil))
}

func IsValid(text string) bool {
	return regexp.MustCompile("[a-zA-Z\\d]{4,20}").MatchString(text)
}
