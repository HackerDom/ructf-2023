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

	res, err := c.Run(&RunRequest{BinaryPath: "binary"})
	if err != nil {
		log.Fatal(err)
	}

	log.Printf("%v", *res)

	res, err = c.Run(&RunRequest{BinaryPath: "error"})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v", *res)

	res, err = c.Run(&RunRequest{BinaryPath: "binary"})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v", *res)

	kres, err := c.Kill(&KillRequest{Vd: 0xdeadbeefcafebabe})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", kres)

	kres, err = c.Kill(&KillRequest{Vd: 0xdeadbeefcafebabe + 1})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", kres)

	kres, err = c.Kill(&KillRequest{Vd: 0xdeadbeefcafebabe})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", kres)

	sres, err := c.Status(&StatusRequest{Vd: 0xdeadbeefcafebabe})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", sres)

	sres, err = c.Status(&StatusRequest{Vd: 0xdeadbeefcafebabe + 1})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", sres)

	sres, err = c.Status(&StatusRequest{Vd: 0xdeadbeefcafebabe + 2})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", sres)

	dres, err := c.Delete(&DeleteRequest{Vd: 0xdeadbeefcafebabe})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", dres)

	dres, err = c.Delete(&DeleteRequest{Vd: 0xdeadbeefcafebabe + 1})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", dres)

	dres, err = c.Delete(&DeleteRequest{Vd: 0xdeadbeefcafebabe})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", dres)

	srres, err := c.GetSerial(&GetSerialRequest{Vd: 0xdeadbeefcafebabe})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", srres)

	srres, err = c.GetSerial(&GetSerialRequest{Vd: 0xdeadbeefcafebabe + 1})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", srres)

	srres, err = c.GetSerial(&GetSerialRequest{Vd: 0xdeadbeefcafebabe + 2})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("%v\n", srres)
}
