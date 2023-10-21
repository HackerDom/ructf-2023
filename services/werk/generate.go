package back

//go:generate protoc --go_out=models proto/config.proto
//go:generate protoc --go_out=models proto/api.proto
//go:generate protoc --go-grpc_out=models proto/api.proto
