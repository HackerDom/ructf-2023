package model

import "time"

type Node struct {
	Dev     uint64
	Ino     uint64
	Nlink   uint64
	Mode    uint32
	Uid     uint32
	Gid     uint32
	Rdev    uint64
	Size    uint64
	Blksize int
	Blocks  int
	Atime   time.Time
	Mtime   time.Time
	Ctime   time.Time
}
