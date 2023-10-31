package clients

import (
	"encoding/binary"
	"fmt"
	"io"
	"net"
	"time"
)

type VmClient struct {
	sockPath string
	conn     net.Conn
	timeout  time.Duration
}

func (c *VmClient) setupReadTimeouts() error {
	if c.timeout != 0 {
		return c.conn.SetReadDeadline(time.Now().Add(c.timeout))
	}

	return nil
}

func (c *VmClient) setupWriteTimeout() error {
	if c.timeout != 0 {
		return c.conn.SetWriteDeadline(time.Now().Add(c.timeout))
	}

	return nil
}

func (c *VmClient) write(data []byte) error {
	if err := c.setupWriteTimeout(); err != nil {
		return err
	}

	n, err := c.conn.Write(data)
	if err != nil {
		return err
	}

	if n != len(data) {
		return fmt.Errorf("written only %d bytes, expected %d", n, len(data))
	}

	return nil
}

func (c *VmClient) read(data []byte) error {
	_, err := io.ReadFull(c.conn, data)
	if err != nil {
		return err
	}

	return nil
}

func (c *VmClient) Close() {
	_ = c.write([]byte{'Q'})
	_ = c.conn.Close()
}

func NewVmClientSession(sockPath string, timeout time.Duration) (*VmClient, error) {
	conn, err := net.Dial("unix", sockPath)
	if err != nil {
		return nil, err
	}

	if timeout == 0 {
		if err := conn.SetReadDeadline(time.Time{}); err != nil {
			return nil, err
		}

		if err := conn.SetWriteDeadline(time.Time{}); err != nil {
			return nil, err
		}
	}

	return &VmClient{
		sockPath: sockPath,
		conn:     conn,
		timeout:  timeout,
	}, nil
}

func (c *VmClient) Run(req *RunRequest) (*RunResponse, error) {
	if len(req.BinaryPath) > 4096 {
		return nil, fmt.Errorf("too long binary path")
	}

	if err := c.setupWriteTimeout(); err != nil {
		return nil, err
	}
	if err := c.setupReadTimeouts(); err != nil {
		return nil, err
	}

	if err := binary.Write(c.conn, binary.LittleEndian, byte('R')); err != nil {
		return nil, fmt.Errorf("command write failed: %v", err)
	}

	if err := binary.Write(c.conn, binary.LittleEndian, uint16(len(req.BinaryPath))); err != nil {
		return nil, fmt.Errorf("path length write failed: %v", err)
	}

	if err := c.write([]byte(req.BinaryPath)); err != nil {
		return nil, err
	}

	var success uint8
	var vd uint64
	var msgLen uint64

	if err := binary.Read(c.conn, binary.LittleEndian, &success); err != nil {
		return nil, err
	}
	if err := binary.Read(c.conn, binary.LittleEndian, &vd); err != nil {
		return nil, err
	}
	if err := binary.Read(c.conn, binary.LittleEndian, &msgLen); err != nil {
		return nil, err
	}

	if success == 1 {
		return &RunResponse{Success: true, Vd: vd, ErrorMessage: ""}, nil
	}

	if success == 0 {
		msgBytes := make([]byte, msgLen)
		err := c.read(msgBytes)
		if err != nil {
			return nil, err
		}

		return &RunResponse{Success: false, Vd: vd, ErrorMessage: string(msgBytes)}, nil
	}

	return nil, fmt.Errorf("protocol error (unexpected status = %v)", success)
}

func (c *VmClient) Kill(req *KillRequest) (*KillResponse, error) {
	if err := c.setupWriteTimeout(); err != nil {
		return nil, err
	}
	if err := c.setupReadTimeouts(); err != nil {
		return nil, err
	}

	if err := binary.Write(c.conn, binary.LittleEndian, byte('K')); err != nil {
		return nil, err
	}
	if err := binary.Write(c.conn, binary.LittleEndian, req.Vd); err != nil {
		return nil, err
	}

	var success uint8
	if err := binary.Read(c.conn, binary.LittleEndian, &success); err != nil {
		return nil, err
	}

	if success == 0 {
		return &KillResponse{Success: false}, nil
	}

	if success == 1 {
		return &KillResponse{Success: true}, nil
	}

	return nil, fmt.Errorf("protocol error (unexpected status = %v)", success)
}

func (c *VmClient) Status(req *StatusRequest) (*StatusResponse, error) {
	if err := c.setupWriteTimeout(); err != nil {
		return nil, err
	}
	if err := c.setupReadTimeouts(); err != nil {
		return nil, err
	}

	if err := binary.Write(c.conn, binary.LittleEndian, byte('S')); err != nil {
		return nil, err
	}
	if err := binary.Write(c.conn, binary.LittleEndian, req.Vd); err != nil {
		return nil, err
	}

	return nil, fmt.Errorf("not implemented")
}
