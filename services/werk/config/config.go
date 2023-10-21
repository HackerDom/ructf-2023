package config

import (
	"log"
	"os"

	"google.golang.org/protobuf/encoding/protojson"
	"google.golang.org/protobuf/proto"
)

func InitConfig[T proto.Message](filename string, config T) error {
	rawConfig, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	log.Printf("using config:\n%s", string(rawConfig))

	err = protojson.Unmarshal(rawConfig, config)
	if err != nil {
		return err
	}

	return nil
}
