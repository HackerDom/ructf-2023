package main

import (
	"back/db"
	"back/storage"
	"flag"
	"fmt"
	"log"
	"net"

	"back/config"
	"back/models"

	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

func main() {
	configFilename := flag.String("config", "", "path to server config")
	flag.Parse()

	if configFilename == nil || len(*configFilename) == 0 {
		flag.Usage()
		log.Fatalf("expected config argument")
	}

	var cfg config.ServerConfig
	if err := config.InitConfig[*config.ServerConfig](*configFilename, &cfg); err != nil {
		log.Fatalf("failed to load config: %v", err)
	}

	storageApi, err := storage.NewApi(cfg.Storage.Path)
	if err != nil {
		if err != nil {
			log.Fatalf("failed to init storage api: " + err.Error())
		}
	}

	dbApi, err := db.NewApi(cfg.Postgres)
	if err != nil {
		log.Fatalf("failed to init db api: " + err.Error())
	}

	listener, err := net.Listen("tcp", fmt.Sprintf("%v:%d", cfg.Grpc.Host, cfg.Grpc.Port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	grpcServer := grpc.NewServer()
	models.RegisterWerkServer(grpcServer, &werkServerImpl{dbApi: dbApi, storageApi: storageApi})

	reflection.Register(grpcServer)

	log.Printf("server listening at %s", listener.Addr())

	if err := grpcServer.Serve(listener); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
