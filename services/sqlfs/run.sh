go build -o sqlfs ./pkg/main.go
umount /mnt/loop
./sqlfs /mnt/loop /home/user/loopbackfs
