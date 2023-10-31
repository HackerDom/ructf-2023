package utils

import (
	"crypto/rand"
	"errors"
	"math/big"
	"strings"
)

var fullAlpha = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890")
var storageAlpha = []rune("abcdefghijklmnopqrstuvwxyz1234567890")

func RandomString(alpha []rune, n int) (string, error) {
	res := strings.Builder{}
	for i := 0; i < n; i++ {
		nBig, err := rand.Int(rand.Reader, big.NewInt(int64(len(alpha))))
		if err != nil {
			return "", errors.New("can not gen random string: " + err.Error())
		}
		res.WriteRune(alpha[nBig.Int64()])
	}
	return res.String(), nil
}

func RandomToken() (string, error) {
	return RandomString(fullAlpha, 32)
}

func RandomStorageKey() (string, error) {
	return RandomString(storageAlpha, 32)
}
