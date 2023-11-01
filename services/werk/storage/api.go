package storage

import (
	"back/utils"
	"errors"
	"os"
	"path/filepath"
)

type Api struct {
	root string
}

func NewApi(path string) (*Api, error) {
	if err := os.MkdirAll(path, os.ModePerm); err != nil {
		return nil, errors.New("can not create dir for storage api")
	}

	return &Api{root: path}, nil
}

func (api *Api) PutObject(value []byte) (string, error) {
	key, err := utils.RandomStorageKey()
	if err != nil {
		return "", errors.New("can not get random storage key: " + err.Error())
	}
	prefix := key[:2]

	if err := os.MkdirAll(filepath.Join(api.root, prefix), os.ModePerm); err != nil {
		return "", errors.New("can not mkdir for object")
	}

	if err := os.WriteFile(filepath.Join(api.root, prefix, key), value, 0644); err != nil {
		return "", errors.New("can not write object")
	}
	return key, nil
}
