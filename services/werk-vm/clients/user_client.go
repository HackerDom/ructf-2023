package clients

import (
	"back/config"
	"back/models"
	"context"
	"errors"
	"fmt"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	"log"
	"time"
)

type UserClient struct {
	grpcConn       *grpc.ClientConn
	werkGrpcClient models.WerkClient
}

func (uc *UserClient) Close() error {
	return uc.grpcConn.Close()
}

func (uc *UserClient) getTimeout() (context.Context, context.CancelFunc) {
	return context.WithTimeout(context.Background(), time.Second)
}

func NewUserClient(cfg *config.ClientConfig) *UserClient {
	addr := fmt.Sprintf("%v:%v", cfg.Host, cfg.Port)
	conn, err := grpc.Dial(addr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}

	c := models.NewWerkClient(conn)

	return &UserClient{
		grpcConn:       conn,
		werkGrpcClient: c,
	}
}

func (uc *UserClient) Register(name string) (string, error) {
	ctx, cancel := uc.getTimeout()
	defer cancel()

	result, err := uc.werkGrpcClient.Register(ctx, &models.CreateUserRequest{Name: name})

	if err != nil {
		return "", errors.New("can not register user: " + err.Error())
	}

	return result.UserPair.Token, nil
}

func (uc *UserClient) CreateImage(username, token, text string) (uint32, error) {
	ctx, cancel := uc.getTimeout()
	defer cancel()

	result, err := uc.werkGrpcClient.CreateImage(ctx, &models.CreateImageRequest{
		UserPair: &models.UserPair{
			Name:  username,
			Token: token,
		},
		Text: text,
	})

	if err != nil {
		return 0, errors.New("can not create image: " + err.Error())
	}

	return result.ImageId, nil
}
