package main

import (
	"context"
	"fmt"

	"back/models"
)

type werkServerImpl struct {
	models.UnimplementedWerkServer
}

func (s *werkServerImpl) Hello(ctx context.Context, in *models.HelloRequest) (*models.HelloResponse, error) {
	return &models.HelloResponse{Message: fmt.Sprintf("Hello from WERK, %q!", in.Name)}, nil
}
