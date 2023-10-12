package main

import (
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

	listener, err := net.Listen("tcp", fmt.Sprintf(":%d", cfg.GrpcPort))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	grpcServer := grpc.NewServer()
	models.RegisterWerkServer(grpcServer, &werkServerImpl{})

	reflection.Register(grpcServer)

	log.Printf("server listening at %s", listener.Addr())

	if err := grpcServer.Serve(listener); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
