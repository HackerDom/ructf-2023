package loopback

import (
	"context"
	"database/sql"
	"path/filepath"
	"syscall"

	"github.com/hanwen/go-fuse/v2/fs"
	"github.com/hanwen/go-fuse/v2/fuse"
	"github.com/rs/zerolog"

	"sqlfs/pkg/store"
)

const (
	GB            = 1024 * 1024 * 1024
	FS_SIZE_LIMIT = 5 * GB
	FS_BLOCK_SIZE = 4096
	MAX_FILESIZE  = 100
)

// LoopbackRoot holds the parameters for creating a new loopback
// filesystem. Loopback filesystem delegate their operations to an
// underlying POSIX file system.
type LoopbackRoot struct {
	// The path to the root of the underlying file system.
	Path string

	// The device on which the Path resides. This must be set if
	// the underlying filesystem crosses file systems.
	Dev uint64

	// NewNode returns a new InodeEmbedder to be used to respond
	// to a LOOKUP/CREATE/MKDIR/MKNOD opcode. If not set, use a
	// LoopbackNode.
	NewNode func(rootData *LoopbackRoot, parent *fs.Inode, name string, st *syscall.Stat_t) fs.InodeEmbedder

	Db *sql.DB
}

func (r *LoopbackRoot) newNode(parent *fs.Inode, name string, st *syscall.Stat_t) fs.InodeEmbedder {
	if r.NewNode != nil {
		return r.NewNode(r, parent, name, st)
	}
	return &LoopbackNode{
		RootData: r,
	}
}

// LoopbackNode is a filesystem node in a loopback file system. It is
// public so it can be used as a basis for other loopback based
// filesystems. See NewLoopbackFile or LoopbackRoot for more
// information.
type LoopbackNode struct {
	fs.Inode

	// RootData points back to the root of the loopback filesystem.
	RootData *LoopbackRoot

	store store.Store
}

var _ = (fs.NodeStatfser)((*LoopbackNode)(nil))
var _ = (fs.NodeGetattrer)((*LoopbackNode)(nil))
var _ = (fs.NodeGetxattrer)((*LoopbackNode)(nil))
var _ = (fs.NodeSetxattrer)((*LoopbackNode)(nil))
var _ = (fs.NodeRemovexattrer)((*LoopbackNode)(nil))
var _ = (fs.NodeListxattrer)((*LoopbackNode)(nil))
var _ = (fs.NodeReadlinker)((*LoopbackNode)(nil))
var _ = (fs.NodeOpener)((*LoopbackNode)(nil))
var _ = (fs.NodeCopyFileRanger)((*LoopbackNode)(nil))
var _ = (fs.NodeLookuper)((*LoopbackNode)(nil))
var _ = (fs.NodeOpendirer)((*LoopbackNode)(nil))
var _ = (fs.NodeReaddirer)((*LoopbackNode)(nil))
var _ = (fs.NodeMkdirer)((*LoopbackNode)(nil))
var _ = (fs.NodeMknoder)((*LoopbackNode)(nil))
var _ = (fs.NodeLinker)((*LoopbackNode)(nil))
var _ = (fs.NodeSymlinker)((*LoopbackNode)(nil))
var _ = (fs.NodeUnlinker)((*LoopbackNode)(nil))
var _ = (fs.NodeRmdirer)((*LoopbackNode)(nil))
var _ = (fs.NodeRenamer)((*LoopbackNode)(nil))

func (n *LoopbackNode) Statfs(ctx context.Context, out *fuse.StatfsOut) syscall.Errno {
	logger := zerolog.Ctx(ctx)

	st := &syscall.Statfs_t{
		Type:   0xEF53,
		Bsize:  FS_BLOCK_SIZE,
		Blocks: FS_SIZE_LIMIT / FS_BLOCK_SIZE,
		Fsid: syscall.Fsid{
			X__val: [2]int32{0xEF53, 0xCAFEBAB},
		},
		Namelen: 50,
		Frsize:  4096,
	}

	nodeCount, err := n.store.GetNodeCount(ctx)
	if err != nil {
		logger.Err(err).Msg("failed to get node count")
		return syscall.EIO
	}

	totalSize, err := n.store.GetNodesTotalSize(ctx)
	if err != nil {
		logger.Err(err).Msg("failed to get nodes total size")
		return syscall.EIO
	}

	st.Bfree = st.Blocks - (totalSize / FS_BLOCK_SIZE)
	if totalSize%FS_BLOCK_SIZE != 0 {
		st.Bfree -= 1
	}

	st.Bavail = st.Bfree
	st.Files = nodeCount
	st.Ffree = (FS_SIZE_LIMIT-GB)/MAX_FILESIZE - nodeCount

	out.FromStatfsT(st)
	return fs.OK
}

// path returns the full path to the file in the underlying file
// system.
func (n *LoopbackNode) path() string {
	path := n.Path(n.Root())
	return filepath.Join(n.RootData.Path, path)
}

func (n *LoopbackNode) Lookup(ctx context.Context, name string, out *fuse.EntryOut) (*fs.Inode, syscall.Errno) {
	var st *syscall.Stat_t

	p := filepath.Join(n.Path(n.Root()), name)
	var err syscall.Errno
	st, err = store.GetStat(ctx, p, n.RootData.Db)
	if err != syscall.F_OK {
		return nil, err
	}

	out.Attr.FromStat(st)

	node := n.RootData.newNode(n.EmbeddedInode(), name, st)
	ch := n.NewInode(ctx, node, fs.StableAttr{
		Mode: st.Mode,
		Ino:  st.Ino,
		Gen:  1,
	})
	return ch, 0
}

func (n *LoopbackNode) Mknod(ctx context.Context, name string, mode, rdev uint32, out *fuse.EntryOut) (*fs.Inode, syscall.Errno) {
	p := filepath.Join(n.Path(n.Root()), name)
	st, err := store.Create(ctx, p, mode, 0, 1, n.RootData.Db)
	if err != syscall.F_OK {
		return nil, err
	}

	out.Attr.FromStat(st)

	node := n.RootData.newNode(n.EmbeddedInode(), name, st)
	ch := n.NewInode(ctx, node, fs.StableAttr{
		Mode: st.Mode,
		Ino:  st.Ino,
		Gen:  1,
	})

	return ch, 0
}

func (n *LoopbackNode) Mkdir(ctx context.Context, name string, mode uint32, out *fuse.EntryOut) (*fs.Inode, syscall.Errno) {
	p := filepath.Join(n.Path(n.Root()), name)
	st, err := store.Create(ctx, p, mode|syscall.S_IFDIR, 4096, 2, n.RootData.Db)
	if err != syscall.F_OK {
		return nil, err
	}

	out.Attr.FromStat(st)

	node := n.RootData.newNode(n.EmbeddedInode(), name, st)
	ch := n.NewInode(ctx, node, fs.StableAttr{
		Mode: st.Mode,
		Ino:  st.Ino,
		Gen:  1,
	})

	return ch, 0
}

func (n *LoopbackNode) Rmdir(ctx context.Context, name string) syscall.Errno {
	return store.Rmdir(ctx, n.Path(n.Root()), name, n.RootData.Db)
}

func (n *LoopbackNode) Unlink(ctx context.Context, name string) syscall.Errno {
	return store.Unlink(ctx, n.Path(n.Root()), name, 0, n.RootData.Db)
}

func (n *LoopbackNode) Rename(
	ctx context.Context,
	name string,
	newParent fs.InodeEmbedder,
	newName string,
	flags uint32,
) syscall.Errno {
	path := n.Path(n.Root())
	newPath := newParent.EmbeddedInode().Path(n.Root())

	return store.Rename(ctx, path, name, newPath, newName, n.RootData.Db)
}

var _ = (fs.NodeCreater)((*LoopbackNode)(nil))

func (n *LoopbackNode) Create(ctx context.Context, name string, flags uint32, mode uint32, out *fuse.EntryOut) (inode *fs.Inode, fh fs.FileHandle, fuseFlags uint32, errno syscall.Errno) {
	p := filepath.Join(n.Path(n.Root()), name)
	st, err := store.Create(ctx, p, mode, 0, 1, n.RootData.Db)
	if err != syscall.F_OK {
		return nil, nil, 0, err
	}

	node := n.RootData.newNode(n.EmbeddedInode(), name, st)
	ch := n.NewInode(ctx, node, fs.StableAttr{
		Mode: st.Mode,
		Ino:  st.Ino,
		Gen:  1,
	})
	lf := NewLoopbackFile(0, int(st.Ino), n.RootData.Db)

	out.FromStat(st)
	return ch, lf, 0, 0
}

func path(n *LoopbackNode) string {
	path := n.Path(n.Root())
	return filepath.Join(n.RootData.Path, path)
}

func (n *LoopbackNode) Symlink(
	ctx context.Context,
	target,
	name string,
	out *fuse.EntryOut,
) (*fs.Inode, syscall.Errno) {
	return nil, syscall.ENOSYS
}

func (n *LoopbackNode) Link(ctx context.Context, target fs.InodeEmbedder, name string, out *fuse.EntryOut) (*fs.Inode, syscall.Errno) {
	return nil, syscall.ENOSYS
}

func (n *LoopbackNode) Readlink(ctx context.Context) ([]byte, syscall.Errno) {
	return nil, syscall.ENOSYS
}

func (n *LoopbackNode) Open(ctx context.Context, flags uint32) (fh fs.FileHandle, fuseFlags uint32, errno syscall.Errno) {
	ino, err := store.GetIno(ctx, n.Path(n.Root()), n.RootData.Db)
	if err != syscall.F_OK {
		return 0, 0, err
	}

	lf := NewLoopbackFile(0, int(ino), n.RootData.Db)
	return lf, 0, 0
}

func (n *LoopbackNode) Opendir(ctx context.Context) syscall.Errno {
	return fs.OK
}

func (n *LoopbackNode) Readdir(ctx context.Context) (fs.DirStream, syscall.Errno) {
	// return fs.NewLoopbackDirStream(n.path())
	stream := store.NewDirStream(n.RootData.Db)
	if err := stream.Init(ctx, n.Path(n.Root())); err != syscall.F_OK {
		return nil, err
	}

	return stream, syscall.F_OK
}

func (n *LoopbackNode) Getattr(ctx context.Context, f fs.FileHandle, out *fuse.AttrOut) syscall.Errno {
	if f != nil {
		return f.(fs.FileGetattrer).Getattr(ctx, out)
	}

	var st *syscall.Stat_t
	if &n.Inode == n.Root() {
		st = &syscall.Stat_t{}
		if err := syscall.Stat(n.path(), st); err != nil {
			return fs.ToErrno(err)
		}
	} else {
		var err syscall.Errno
		st, err = store.GetStat(ctx, n.Path(n.Root()), n.RootData.Db)
		if err != syscall.F_OK {
			return err
		}
	}

	out.FromStat(st)
	return fs.OK
}

var _ = (fs.NodeSetattrer)((*LoopbackNode)(nil))

func (n *LoopbackNode) Setattr(ctx context.Context, f fs.FileHandle, in *fuse.SetAttrIn, out *fuse.AttrOut) syscall.Errno {
	p := n.path()
	fsa, ok := f.(fs.FileSetattrer)
	if ok && fsa != nil {
		fsa.Setattr(ctx, in, out)
	} else {
		if m, ok := in.GetMode(); ok {
			if err := syscall.Chmod(p, m); err != nil {
				return fs.ToErrno(err)
			}
		}

		uid, uok := in.GetUID()
		gid, gok := in.GetGID()
		if uok || gok {
			suid := -1
			sgid := -1
			if uok {
				suid = int(uid)
			}
			if gok {
				sgid = int(gid)
			}
			if err := syscall.Chown(p, suid, sgid); err != nil {
				return fs.ToErrno(err)
			}
		}

		mtime, mok := in.GetMTime()
		atime, aok := in.GetATime()

		if mok || aok {
			ap := &atime
			mp := &mtime
			if !aok {
				ap = nil
			}
			if !mok {
				mp = nil
			}
			var ts [2]syscall.Timespec
			ts[0] = fuse.UtimeToTimespec(ap)
			ts[1] = fuse.UtimeToTimespec(mp)

			if err := store.UpdateTimes(ctx, n.Path(n.Root()), ts[:], n.RootData.Db); err != syscall.F_OK {
				return err
			}
		}

		// TODO
		// if sz, ok := in.GetSize(); ok {
		// 	if err := syscall.Truncate(p, int64(sz)); err != nil {
		// 		return fs.ToErrno(err)
		// 	}
		// }
	}

	fga, ok := f.(fs.FileGetattrer)
	if ok && fga != nil {
		fga.Getattr(ctx, out)
	} else {
		st, err := store.GetStat(ctx, n.Path(n.Root()), n.RootData.Db)
		if err != syscall.F_OK {
			return err
		}
		out.FromStat(st)
	}
	return fs.OK
}

// NewLoopbackRoot returns a root node for a loopback file system whose
// root is at the given root. This node implements all NodeXxxxer
// operations available.
func NewLoopbackRoot(rootPath string, db *sql.DB) (fs.InodeEmbedder, error) {
	var st syscall.Stat_t
	err := syscall.Stat(rootPath, &st)
	if err != nil {
		return nil, err
	}

	root := &LoopbackRoot{
		Path: rootPath,
		Dev:  uint64(st.Dev),
		Db:   db,
	}

	return root.newNode(nil, "", &st), nil
}

func (n *LoopbackNode) Getxattr(ctx context.Context, attr string, dest []byte) (uint32, syscall.Errno) {
	return 0, syscall.ENOSYS
}

func (n *LoopbackNode) Setxattr(ctx context.Context, attr string, data []byte, flags uint32) syscall.Errno {
	return syscall.ENOSYS
}

func (n *LoopbackNode) Removexattr(ctx context.Context, attr string) syscall.Errno {
	return syscall.ENOSYS
}

func (n *LoopbackNode) Listxattr(ctx context.Context, dest []byte) (uint32, syscall.Errno) {
	return 0, syscall.ENOSYS
}

func (n *LoopbackNode) CopyFileRange(ctx context.Context, fhIn fs.FileHandle,
	offIn uint64, out *fs.Inode, fhOut fs.FileHandle, offOut uint64,
	len uint64, flags uint64) (uint32, syscall.Errno) {
	return 0, syscall.ENOSYS
}
