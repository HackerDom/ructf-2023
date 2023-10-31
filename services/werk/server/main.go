package main

import (
	"back/db"
	"errors"
	"flag"
	"fmt"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
	"log"
	"net"

	"back/config"
	"back/models"

	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

func initGormDB(cfg *config.Postgres) (*gorm.DB, error) {
	dsn := fmt.Sprintf(
		"host=%v user=%v password=%v dbname=%v port=%v sslmode=disable",
		cfg.Host,
		cfg.Username,
		cfg.Password,
		cfg.Database,
		cfg.Port,
	)
	db, err := gorm.Open(postgres.Open(dsn), &gorm.Config{})

	if err != nil {
		return nil, errors.New("can not open gorm db: " + err.Error())
	}
	fmt.Println("migrate!")
	if err := db.AutoMigrate(models.UserPairModel{}); err != nil {
		return nil, errors.New("can not migrate gorm db: " + err.Error())
	}

	return db, nil
}

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

	gormDb, err := initGormDB(cfg.Postgres)
	if err != nil {
		log.Fatalf("failed to init gorm db: " + err.Error())
	}

	listener, err := net.Listen("tcp", fmt.Sprintf("%v:%d", cfg.Grpc.Host, cfg.Grpc.Port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	grpcServer := grpc.NewServer()
	dbApi := db.NewApi(gormDb)
	models.RegisterWerkServer(grpcServer, &werkServerImpl{dbApi: dbApi})

	reflection.Register(grpcServer)

	log.Printf("server listening at %s", listener.Addr())

	if err := grpcServer.Serve(listener); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
