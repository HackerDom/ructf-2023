package clients

import (
	"log"
	"testing"
	"time"
)

func TestVmClientPlayground(t *testing.T) {
	t.Skip("playground")

	c, err := NewVmClientSession("/home/rkhapov/file.sock", time.Second)
	if err != nil {
		log.Fatal(err)
	}
	defer c.Close()

	res, err := c.Run(&RunRequest{BinaryPath: "binary", SerialOutPath: "serial"})
	if err != nil {
		log.Fatal(err)
	}

	log.Printf("%v", *res)

	res, err = c.Run(&RunRequest{BinaryPath: "error", SerialOutPath: "serial"})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v", *res)
}