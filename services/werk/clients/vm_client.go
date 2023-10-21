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
	if len(req.BinaryPath) > 255 {
		return nil, fmt.Errorf("too long binary path")
	}

	if len(req.SerialOutPath) > 255 {
		return nil, fmt.Errorf("too long serial out path")
	}

	reqBinary := make([]byte, 0)

	reqBinary = append(reqBinary, byte('R'))
	reqBinary = append(reqBinary, byte(len(req.BinaryPath)))
	reqBinary = append(reqBinary, byte(len(req.SerialOutPath)))
	reqBinary = append(reqBinary, []byte(req.BinaryPath)...)
	reqBinary = append(reqBinary, []byte(req.SerialOutPath)...)

	if err := c.write(reqBinary); err != nil {
		return nil, err
	}

	resBinary := make([]byte, 1)
	if err := c.read(resBinary); err != nil {
		return nil, err
	}

	if resBinary[0] == 0 {
		msgLenBytes := make([]byte, 2)
		if err := c.read(msgLenBytes); err != nil {
			return nil, err
		}
		msgLen := int(msgLenBytes[0]) | (int(msgLenBytes[1]) << 8)
		errMsg := make([]byte, msgLen)
		if err := c.read(errMsg); err != nil {
			return nil, err
		}

		return &RunResponse{Success: false, ErrorMessage: string(errMsg)}, nil
	} else if resBinary[0] == 1 {
		var vd uint64
		if err := binary.Read(c.conn, binary.LittleEndian, &vd); err != nil {
			return nil, err
		}

		return &RunResponse{Success: true, Vd: vd}, nil
	}

	return nil, fmt.Errorf("protocol error (status = %d)", resBinary[0])
}

//func reader(r io.Reader) {
//	buf := make([]byte, 1024)
//	for {
//		reader := bufio.NewReader(r)
//		io.ReadFull()
//
//		reader.Read()
//
//		n, err := r.Read(buf[:])
//		if err != nil {
//			return
//		}
//		println("Client got:", string(buf[0:n]))
//	}
//}
