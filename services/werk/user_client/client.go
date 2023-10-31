package main

import (
	"back/models"
	"back/utils"
	"context"
	"flag"
	"fmt"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	"log"
	"time"
)

func main() {
	addr := "localhost:7654"
	name, _ := utils.RandomStorageKey()

	fmt.Println(addr, name)

	flag.Parse()
	// Set up a connection to the server.
	conn, err := grpc.Dial(addr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := models.NewWerkClient(conn)

	// Contact the server and print out its response.
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	r, err := c.Register(ctx, &models.CreateUserRequest{Name: name})
	if err != nil {
		log.Fatalf("could not greet: %v", err)
	}
	fmt.Println(r.UserPair)

	createResp, err := c.CreateImage(ctx, &models.CreateImageRequest{
		UserPair: r.UserPair,
		Text:     "my cool code",
	})
	if err != nil {
		log.Fatalf(err.Error())
	}

	fmt.Println(createResp)

	runResp, err := c.RunVM(ctx, &models.RunVMRequest{
		UserPair:  r.UserPair,
		ImageUuid: createResp.ImageUuid,
	})

	if err != nil {
		log.Fatalf(err.Error())
	}

	fmt.Println(runResp)

	getStateResp, err := c.GetVMState(ctx, &models.GetVMStateRequest{
		UserPair: r.UserPair,
		RunUuid:  runResp.RunUuid,
	})

	if err != nil {
		log.Fatalf(err.Error())
	}

	fmt.Println(getStateResp)
}
