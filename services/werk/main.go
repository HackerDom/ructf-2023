package main

import (
	"encoding/json"
	"flag"
	"log"
	"os"
	"werk/server"
)

func main() {
	configFilename := flag.String("config", "", "path to server config")
	flag.Parse()

	if configFilename == nil || len(*configFilename) == 0 {
		flag.Usage()
		log.Fatalf("expected config argument")
	}

	rawConfig, err := os.ReadFile(*configFilename)
	if err != nil {
		log.Fatalln("can not read config file: " + err.Error())
	}

	var config server.Config
	if err := json.Unmarshal(rawConfig, &config); err != nil {
		log.Fatalln("can not unmarshal config: " + err.Error())
	}

	server.RunServer(&config)
}
