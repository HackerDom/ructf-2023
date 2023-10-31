package main

import (
	"back/db"
	"back/models"
	"context"
	"fmt"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

type werkServerImpl struct {
	models.UnimplementedWerkServer
	dbApi *db.Api
}

func (s *werkServerImpl) Hello(ctx context.Context, in *models.HelloRequest) (*models.HelloResponse, error) {
	return &models.HelloResponse{Message: fmt.Sprintf("Hello from WERK, %q!", in.Name)}, nil
}

func (s *werkServerImpl) Register(ctx context.Context, in *models.CreateUserRequest) (*models.CreateUserResponse, error) {
	token, err := s.dbApi.CreateUserPair(in.Name)
	if err != nil {
		return nil, status.Error(codes.InvalidArgument, "can not register user")
	}

	return &models.CreateUserResponse{UserPair: &models.UserPair{
		Name:  in.Name,
		Token: token,
	}}, nil
}
