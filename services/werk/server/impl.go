package main

import (
	"back/db"
	"back/models"
	"back/storage"
	"context"
	"errors"
	"fmt"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

type werkServerImpl struct {
	models.UnimplementedWerkServer
	dbApi      *db.Api
	storageApi *storage.Api
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

func (s *werkServerImpl) CreateImage(ctx context.Context, in *models.CreateImageRequest) (*models.CreateImageResponse, error) {
	if !s.dbApi.IsUserPairValid(in.UserPair.Name, in.UserPair.Token) {
		return nil, status.Error(codes.Unauthenticated, "invalid user pair")
	}

	translatedAsmCode := []byte(in.Text)

	storageKey, err := s.storageApi.PutObject(translatedAsmCode)
	if err != nil {
		return nil, errors.New("can not put object: " + err.Error())
	}

	asmCodeUuid, err := s.dbApi.CreateAsmCodeModel(in.UserPair.Name, storageKey)
	if err != nil {
		return nil, errors.New("can not create asm code: " + err.Error())
	}

	return &models.CreateImageResponse{
		ImageUuid: asmCodeUuid,
	}, nil
}

func (s *werkServerImpl) RunVM(ctx context.Context, in *models.RunVMRequest) (*models.RunVMResponse, error) {
	if !s.dbApi.IsUserPairValid(in.UserPair.Name, in.UserPair.Token) {
		return nil, status.Error(codes.Unauthenticated, "invalid user pair")
	}

	if !s.dbApi.IsImageOwnerCorrect(in.UserPair.Name, in.ImageUuid) {
		return nil, status.Error(codes.NotFound, "user or image not found")
	}

	return nil, status.Error(codes.Unimplemented, "not implemented yet")

	runUuid, err := s.dbApi.CreateRunModel(in.UserPair.Name, in.ImageUuid)
	if err != nil {
		return nil, status.Error(codes.Internal, "can not create run model: "+err.Error())
	}

	return &models.RunVMResponse{RunUuid: runUuid}, nil
}

func (s *werkServerImpl) GetVMState(ctx context.Context, in *models.GetVMStateRequest) (*models.GetVMStateResponse, error) {
	if !s.dbApi.IsUserPairValid(in.UserPair.Name, in.UserPair.Token) {
		return nil, status.Error(codes.Unauthenticated, "invalid user pair")
	}

	if !s.dbApi.IsRunOwnerCorrect(in.UserPair.Name, in.RunUuid) {
		return nil, status.Error(codes.NotFound, "user or run not found")
	}

	return nil, status.Error(codes.Unimplemented, "not implemented yet")
	return &models.GetVMStateResponse{State: models.GetVMStateResponse_RUNNING}, nil
}
