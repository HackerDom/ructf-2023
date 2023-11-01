package loopback

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"path"
	"path/filepath"
	"syscall"

	"github.com/hanwen/go-fuse/v2/fs"
	"github.com/hanwen/go-fuse/v2/fuse"
	"github.com/rs/zerolog"

	"sqlfs/pkg/model"
	"sqlfs/pkg/store"
)

const (
	GB            = 1024 * 1024 * 1024
	FS_SIZE_LIMIT = 5 * GB
	FS_BLOCK_SIZE = 4096
	MAX_FILESIZE  = 100
	FS_BLOCKS     = FS_SIZE_LIMIT / FS_BLOCK_SIZE
)

var errFileNotFound = errors.New("file not found")

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

	store  store.Store
	logger *zerolog.Logger
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
var _ = (fs.NodeCreater)((*LoopbackNode)(nil))

func (n *LoopbackNode) Statfs(ctx context.Context, out *fuse.StatfsOut) syscall.Errno {
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

	nodeCount, err := n.RootData.store.GetNodeCount(ctx)
	if err != nil {
		n.RootData.logger.Err(err).Msg("failed to get node count")
		return syscall.EIO
	}

	totalSize, err := n.RootData.store.GetNodesTotalSize(ctx)
	if err != nil {
		n.RootData.logger.Err(err).Msg("failed to get nodes total size")
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
	p := filepath.Join(n.Path(n.Root()), name)

	ino, err := n.RootData.store.GetNodeIno(ctx, p)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		return nil, syscall.ENOENT
	case err == nil:
	default:
		n.RootData.logger.Err(err).Msg("failed to get ino of node")
		return nil, syscall.EIO
	}

	node, err := n.RootData.store.GetNode(ctx, ino)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		return nil, syscall.ENOENT
	case err == nil:
	default:
		n.RootData.logger.Err(err).
			Str("ino", fmt.Sprintf("%d", ino)).
			Msg("failed to get node by ino")
		return nil, syscall.EIO
	}

	st := nodeToStat(node)
	out.Attr.FromStat(st)
	inodeEmbedder := n.RootData.newNode(n.EmbeddedInode(), name, st)
	inode := n.NewInode(ctx, inodeEmbedder, fs.StableAttr{
		Mode: st.Mode,
		Ino:  st.Ino,
		Gen:  1,
	})

	return inode, 0
}

func (n *LoopbackNode) Mknod(
	ctx context.Context,
	name string,
	mode, rdev uint32,
	out *fuse.EntryOut,
) (*fs.Inode, syscall.Errno) {
	return n.createNode(ctx, name, mode, 0, 1, out)
}

func (n *LoopbackNode) Mkdir(ctx context.Context, name string, mode uint32, out *fuse.EntryOut) (*fs.Inode, syscall.Errno) {
	return n.createNode(ctx, name, mode|syscall.S_IFDIR, 4096, 2, out)
}

func (n *LoopbackNode) Rmdir(ctx context.Context, name string) syscall.Errno {
	errDirNotEmpty := errors.New("directory is not empty")
	errFileNotFound := errors.New("file not found")

	err := n.RootData.store.RunInTransaction(ctx, func(ctx context.Context) error {
		fullPath := path.Join(n.Path(n.Root()), name)
		childCount, err := n.RootData.store.GetEntriesCount(
			ctx, store.WithPath(fullPath),
		)
		if err != nil {
			return fmt.Errorf("failed to get entries: %w", err)
		}

		if childCount > 0 {
			return errDirNotEmpty
		}

		return n.unlink(ctx, name, 1)
	})

	switch {
	case errors.Is(err, errDirNotEmpty):
		return syscall.ENOTEMPTY
	case errors.Is(err, errFileNotFound):
		return syscall.ENOENT
	case err == nil:
		return syscall.F_OK
	default:
		n.RootData.logger.Err(err).Msg("failed to remove directory")
		return syscall.EIO
	}
}

func (n *LoopbackNode) Unlink(ctx context.Context, name string) syscall.Errno {
	err := n.unlink(ctx, name, 0)
	switch {
	case errors.Is(err, errFileNotFound):
		return syscall.ENOENT
	case err == nil:
		return syscall.F_OK
	default:
		n.RootData.logger.Err(err).Msg("failed to unlink")
		return syscall.EIO
	}
}

func (n *LoopbackNode) Rename(
	ctx context.Context,
	oldName string,
	newParent fs.InodeEmbedder,
	newName string,
	flags uint32,
) syscall.Errno {
	oldPath := n.Path(n.Root())
	newPath := newParent.EmbeddedInode().Path(n.Root())

	_, err := n.RootData.store.GetNodeIno(ctx, path.Join(newPath, newName))
	switch {
	case errors.Is(err, sql.ErrNoRows):
	case err == nil:
		return syscall.EEXIST
	default:
		n.RootData.logger.Err(err).Msg("failed to rename node")
		return syscall.EIO
	}

	node, err := n.RootData.store.GetNodeByEntry(ctx, oldPath, oldName)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		return syscall.ENOENT
	case err == nil:
	default:
		n.RootData.logger.Err(err).Msg("failed to rename node")
		return syscall.EIO
	}

	err = n.RootData.store.RunInTransaction(ctx, func(ctx context.Context) error {
		if err := n.RootData.store.UpdateEntries(
			ctx, oldPath, newPath,
			store.NewUpdateEntryMask().Name(newName),
			store.WithName(oldName),
		); err != nil {
			return fmt.Errorf("failed to update entry of dir: %w", err)
		}

		if node.Mode&syscall.S_IFDIR != syscall.S_IFDIR {
			return nil
		}

		childrenPath := path.Join(oldPath, oldName)
		newChildrenPath := path.Join(newPath, newName)
		if err := n.RootData.store.UpdateEntries(ctx, childrenPath, newChildrenPath, store.NewUpdateEntryMask()); err != nil {
			return fmt.Errorf("failed to update entries of directory children: %w", err)
		}

		return nil
	})
	if err != nil {
		n.RootData.logger.Err(err).Msg("failed to rename node")
		return syscall.EIO
	}

	return syscall.F_OK
}

var _ = (fs.NodeCreater)((*LoopbackNode)(nil))

func (n *LoopbackNode) Create(ctx context.Context, name string, flags uint32, mode uint32, out *fuse.EntryOut) (inode *fs.Inode, fh fs.FileHandle, fuseFlags uint32, errno syscall.Errno) {
	ch, err := n.createNode(ctx, name, mode, 0, 1, out)
	if err != syscall.F_OK {
		return nil, nil, 0, err
	}
	lf := NewLoopbackFile(0, int(ch.StableAttr().Ino), n.RootData.Db)

	return ch, lf, 0, syscall.F_OK
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
		_ = fsa.Setattr(ctx, in, out)
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
		_ = fga.Getattr(ctx, out)
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
func NewLoopbackRoot(rootPath string, db *sql.DB, store store.Store, logger *zerolog.Logger) (fs.InodeEmbedder, error) {
	var st syscall.Stat_t
	err := syscall.Stat(rootPath, &st)
	if err != nil {
		return nil, err
	}

	root := &LoopbackRoot{
		Path:   rootPath,
		Dev:    uint64(st.Dev),
		Db:     db,
		store:  store,
		logger: logger,
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

func (n *LoopbackNode) createNode(
	ctx context.Context,
	name string,
	mode uint32,
	size uint64,
	nlink uint64,
	out *fuse.EntryOut,
) (*fs.Inode, syscall.Errno) {
	p := filepath.Join(n.Path(n.Root()), name)
	var node *model.Node

	ino, err := n.RootData.store.GetNodeIno(ctx, p)
	switch {
	case errors.Is(err, sql.ErrNoRows):
		err := n.RootData.store.RunInTransaction(ctx, func(ctx context.Context) error {
			var err error
			node, err = n.RootData.store.CreateNode(ctx, mode, size, nlink)
			if err != nil {
				return fmt.Errorf("failed to create node: %w", err)
			}

			if err := n.RootData.store.CreateEntry(ctx, n.Path(n.Root()), name, node.Ino); err != nil {
				return fmt.Errorf("failed to create entry: %w", err)
			}

			return nil
		})

		if err != nil {
			n.RootData.logger.Err(err).Msg("failed to create node")
			return nil, syscall.EIO
		}
	case err == nil:
		var err error
		node, err = n.RootData.store.GetNode(ctx, ino)
		if err != nil {
			n.RootData.logger.Err(err).Msg("failed to get node")
			return nil, syscall.EIO
		}
	default:
		n.RootData.logger.Err(err).Msg("failed to get ino if inode")
		return nil, syscall.EIO
	}

	st := nodeToStat(node)

	out.Attr.FromStat(st)

	inode := n.RootData.newNode(n.EmbeddedInode(), name, st)
	ch := n.NewInode(ctx, inode, fs.StableAttr{
		Mode: st.Mode,
		Ino:  st.Ino,
		Gen:  1,
	})

	return ch, fuse.F_OK
}

func nodeToStat(node *model.Node) *syscall.Stat_t {
	blockCount := node.Size / FS_BLOCK_SIZE
	if node.Size%FS_BLOCK_SIZE > 0 {
		blockCount++
	}

	return &syscall.Stat_t{
		Dev:     node.Dev,
		Ino:     node.Ino,
		Nlink:   node.Nlink,
		Mode:    node.Mode,
		Uid:     node.Uid,
		Gid:     node.Gid,
		Rdev:    node.Rdev,
		Size:    int64(node.Size),
		Blksize: FS_BLOCK_SIZE,
		Blocks:  int64(blockCount),
		Atim:    syscall.Timespec{Sec: node.AccessTime.Unix(), Nsec: int64(node.AccessTime.Nanosecond())},
		Mtim:    syscall.Timespec{Sec: node.ModifyTime.Unix(), Nsec: int64(node.ModifyTime.Nanosecond())},
		Ctim:    syscall.Timespec{Sec: node.StatusChangeTime.Unix(), Nsec: int64(node.StatusChangeTime.Nanosecond())},
	}
}

func (n *LoopbackNode) unlink(ctx context.Context, name string, deleteNlink uint64) error {
	return n.RootData.store.RunInTransaction(ctx, func(ctx context.Context) error {
		ino, err := n.RootData.store.DeleteEntries(ctx, n.Path(n.Root()), name)
		switch {
		case errors.Is(err, sql.ErrNoRows):
			return errFileNotFound
		case err == nil:
		default:
			return fmt.Errorf("failed to delete entries: %w", err)
		}

		nlink, err := n.RootData.store.DecrementNodeNlink(ctx, ino)
		if err != nil {
			return fmt.Errorf("failed to decrement node nlink: %w", err)
		}

		if nlink > deleteNlink {
			return nil
		}

		if err := n.RootData.store.DeleteNode(ctx, ino); err != nil {
			return fmt.Errorf("failed to delete node: %w", err)
		}

		return nil
	})
}
