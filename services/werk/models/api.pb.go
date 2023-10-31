// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.28.0
// 	protoc        v3.21.12
// source: proto/api.proto

package models

import (
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	reflect "reflect"
	sync "sync"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

type StatusCode int32

const (
	StatusCode_SUCCESS StatusCode = 0
	StatusCode_ERROR   StatusCode = 1
)

// Enum value maps for StatusCode.
var (
	StatusCode_name = map[int32]string{
		0: "SUCCESS",
		1: "ERROR",
	}
	StatusCode_value = map[string]int32{
		"SUCCESS": 0,
		"ERROR":   1,
	}
)

func (x StatusCode) Enum() *StatusCode {
	p := new(StatusCode)
	*p = x
	return p
}

func (x StatusCode) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (StatusCode) Descriptor() protoreflect.EnumDescriptor {
	return file_proto_api_proto_enumTypes[0].Descriptor()
}

func (StatusCode) Type() protoreflect.EnumType {
	return &file_proto_api_proto_enumTypes[0]
}

func (x StatusCode) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use StatusCode.Descriptor instead.
func (StatusCode) EnumDescriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{0}
}

type GetVMStateResponse_VMState int32

const (
	GetVMStateResponse_RUNNING GetVMStateResponse_VMState = 0
	GetVMStateResponse_SUCCESS GetVMStateResponse_VMState = 1
	GetVMStateResponse_KILLED  GetVMStateResponse_VMState = 2
	GetVMStateResponse_ERROR   GetVMStateResponse_VMState = 3
)

// Enum value maps for GetVMStateResponse_VMState.
var (
	GetVMStateResponse_VMState_name = map[int32]string{
		0: "RUNNING",
		1: "SUCCESS",
		2: "KILLED",
		3: "ERROR",
	}
	GetVMStateResponse_VMState_value = map[string]int32{
		"RUNNING": 0,
		"SUCCESS": 1,
		"KILLED":  2,
		"ERROR":   3,
	}
)

func (x GetVMStateResponse_VMState) Enum() *GetVMStateResponse_VMState {
	p := new(GetVMStateResponse_VMState)
	*p = x
	return p
}

func (x GetVMStateResponse_VMState) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (GetVMStateResponse_VMState) Descriptor() protoreflect.EnumDescriptor {
	return file_proto_api_proto_enumTypes[1].Descriptor()
}

func (GetVMStateResponse_VMState) Type() protoreflect.EnumType {
	return &file_proto_api_proto_enumTypes[1]
}

func (x GetVMStateResponse_VMState) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use GetVMStateResponse_VMState.Descriptor instead.
func (GetVMStateResponse_VMState) EnumDescriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{10, 0}
}

type HelloRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Name string `protobuf:"bytes,1,opt,name=name,proto3" json:"name,omitempty"`
}

func (x *HelloRequest) Reset() {
	*x = HelloRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *HelloRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*HelloRequest) ProtoMessage() {}

func (x *HelloRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use HelloRequest.ProtoReflect.Descriptor instead.
func (*HelloRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{0}
}

func (x *HelloRequest) GetName() string {
	if x != nil {
		return x.Name
	}
	return ""
}

type HelloResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Message string `protobuf:"bytes,1,opt,name=message,proto3" json:"message,omitempty"`
}

func (x *HelloResponse) Reset() {
	*x = HelloResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *HelloResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*HelloResponse) ProtoMessage() {}

func (x *HelloResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use HelloResponse.ProtoReflect.Descriptor instead.
func (*HelloResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{1}
}

func (x *HelloResponse) GetMessage() string {
	if x != nil {
		return x.Message
	}
	return ""
}

type CreateUserRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Name string `protobuf:"bytes,1,opt,name=name,proto3" json:"name,omitempty"`
}

func (x *CreateUserRequest) Reset() {
	*x = CreateUserRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CreateUserRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CreateUserRequest) ProtoMessage() {}

func (x *CreateUserRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CreateUserRequest.ProtoReflect.Descriptor instead.
func (*CreateUserRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{2}
}

func (x *CreateUserRequest) GetName() string {
	if x != nil {
		return x.Name
	}
	return ""
}

type UserPair struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Name  string `protobuf:"bytes,1,opt,name=name,proto3" json:"name,omitempty"`
	Token string `protobuf:"bytes,2,opt,name=token,proto3" json:"token,omitempty"`
}

func (x *UserPair) Reset() {
	*x = UserPair{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *UserPair) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*UserPair) ProtoMessage() {}

func (x *UserPair) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use UserPair.ProtoReflect.Descriptor instead.
func (*UserPair) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{3}
}

func (x *UserPair) GetName() string {
	if x != nil {
		return x.Name
	}
	return ""
}

func (x *UserPair) GetToken() string {
	if x != nil {
		return x.Token
	}
	return ""
}

type CreateUserResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	UserPair *UserPair `protobuf:"bytes,1,opt,name=user_pair,json=userPair,proto3" json:"user_pair,omitempty"`
}

func (x *CreateUserResponse) Reset() {
	*x = CreateUserResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[4]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CreateUserResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CreateUserResponse) ProtoMessage() {}

func (x *CreateUserResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[4]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CreateUserResponse.ProtoReflect.Descriptor instead.
func (*CreateUserResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{4}
}

func (x *CreateUserResponse) GetUserPair() *UserPair {
	if x != nil {
		return x.UserPair
	}
	return nil
}

type CreateImageRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	UserPair *UserPair `protobuf:"bytes,1,opt,name=user_pair,json=userPair,proto3" json:"user_pair,omitempty"`
	Text     string    `protobuf:"bytes,2,opt,name=text,proto3" json:"text,omitempty"`
}

func (x *CreateImageRequest) Reset() {
	*x = CreateImageRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[5]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CreateImageRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CreateImageRequest) ProtoMessage() {}

func (x *CreateImageRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[5]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CreateImageRequest.ProtoReflect.Descriptor instead.
func (*CreateImageRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{5}
}

func (x *CreateImageRequest) GetUserPair() *UserPair {
	if x != nil {
		return x.UserPair
	}
	return nil
}

func (x *CreateImageRequest) GetText() string {
	if x != nil {
		return x.Text
	}
	return ""
}

type CreateImageResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	ImageUuid string `protobuf:"bytes,1,opt,name=image_uuid,json=imageUuid,proto3" json:"image_uuid,omitempty"`
}

func (x *CreateImageResponse) Reset() {
	*x = CreateImageResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[6]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CreateImageResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CreateImageResponse) ProtoMessage() {}

func (x *CreateImageResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[6]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CreateImageResponse.ProtoReflect.Descriptor instead.
func (*CreateImageResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{6}
}

func (x *CreateImageResponse) GetImageUuid() string {
	if x != nil {
		return x.ImageUuid
	}
	return ""
}

type RunVMRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	UserPair  *UserPair `protobuf:"bytes,1,opt,name=user_pair,json=userPair,proto3" json:"user_pair,omitempty"`
	ImageUuid string    `protobuf:"bytes,2,opt,name=image_uuid,json=imageUuid,proto3" json:"image_uuid,omitempty"`
}

func (x *RunVMRequest) Reset() {
	*x = RunVMRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[7]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *RunVMRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*RunVMRequest) ProtoMessage() {}

func (x *RunVMRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[7]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use RunVMRequest.ProtoReflect.Descriptor instead.
func (*RunVMRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{7}
}

func (x *RunVMRequest) GetUserPair() *UserPair {
	if x != nil {
		return x.UserPair
	}
	return nil
}

func (x *RunVMRequest) GetImageUuid() string {
	if x != nil {
		return x.ImageUuid
	}
	return ""
}

type RunVMResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	RunUuid string `protobuf:"bytes,1,opt,name=run_uuid,json=runUuid,proto3" json:"run_uuid,omitempty"`
}

func (x *RunVMResponse) Reset() {
	*x = RunVMResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[8]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *RunVMResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*RunVMResponse) ProtoMessage() {}

func (x *RunVMResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[8]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use RunVMResponse.ProtoReflect.Descriptor instead.
func (*RunVMResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{8}
}

func (x *RunVMResponse) GetRunUuid() string {
	if x != nil {
		return x.RunUuid
	}
	return ""
}

type GetVMStateRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	UserPair *UserPair `protobuf:"bytes,1,opt,name=user_pair,json=userPair,proto3" json:"user_pair,omitempty"`
	RunUuid  string    `protobuf:"bytes,2,opt,name=run_uuid,json=runUuid,proto3" json:"run_uuid,omitempty"`
}

func (x *GetVMStateRequest) Reset() {
	*x = GetVMStateRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[9]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *GetVMStateRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GetVMStateRequest) ProtoMessage() {}

func (x *GetVMStateRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[9]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GetVMStateRequest.ProtoReflect.Descriptor instead.
func (*GetVMStateRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{9}
}

func (x *GetVMStateRequest) GetUserPair() *UserPair {
	if x != nil {
		return x.UserPair
	}
	return nil
}

func (x *GetVMStateRequest) GetRunUuid() string {
	if x != nil {
		return x.RunUuid
	}
	return ""
}

type GetVMStateResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	State GetVMStateResponse_VMState `protobuf:"varint,1,opt,name=state,proto3,enum=models.GetVMStateResponse_VMState" json:"state,omitempty"`
}

func (x *GetVMStateResponse) Reset() {
	*x = GetVMStateResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[10]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *GetVMStateResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GetVMStateResponse) ProtoMessage() {}

func (x *GetVMStateResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[10]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GetVMStateResponse.ProtoReflect.Descriptor instead.
func (*GetVMStateResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{10}
}

func (x *GetVMStateResponse) GetState() GetVMStateResponse_VMState {
	if x != nil {
		return x.State
	}
	return GetVMStateResponse_RUNNING
}

type KillVMRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	UserPair *UserPair `protobuf:"bytes,1,opt,name=user_pair,json=userPair,proto3" json:"user_pair,omitempty"`
	RunUuid  string    `protobuf:"bytes,2,opt,name=run_uuid,json=runUuid,proto3" json:"run_uuid,omitempty"`
}

func (x *KillVMRequest) Reset() {
	*x = KillVMRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[11]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *KillVMRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*KillVMRequest) ProtoMessage() {}

func (x *KillVMRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[11]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use KillVMRequest.ProtoReflect.Descriptor instead.
func (*KillVMRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{11}
}

func (x *KillVMRequest) GetUserPair() *UserPair {
	if x != nil {
		return x.UserPair
	}
	return nil
}

func (x *KillVMRequest) GetRunUuid() string {
	if x != nil {
		return x.RunUuid
	}
	return ""
}

type KillVMResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields
}

func (x *KillVMResponse) Reset() {
	*x = KillVMResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[12]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *KillVMResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*KillVMResponse) ProtoMessage() {}

func (x *KillVMResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[12]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use KillVMResponse.ProtoReflect.Descriptor instead.
func (*KillVMResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{12}
}

type GetVMSerialRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	UserPair *UserPair `protobuf:"bytes,1,opt,name=user_pair,json=userPair,proto3" json:"user_pair,omitempty"`
	RunUuid  string    `protobuf:"bytes,2,opt,name=run_uuid,json=runUuid,proto3" json:"run_uuid,omitempty"`
}

func (x *GetVMSerialRequest) Reset() {
	*x = GetVMSerialRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[13]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *GetVMSerialRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GetVMSerialRequest) ProtoMessage() {}

func (x *GetVMSerialRequest) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[13]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GetVMSerialRequest.ProtoReflect.Descriptor instead.
func (*GetVMSerialRequest) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{13}
}

func (x *GetVMSerialRequest) GetUserPair() *UserPair {
	if x != nil {
		return x.UserPair
	}
	return nil
}

func (x *GetVMSerialRequest) GetRunUuid() string {
	if x != nil {
		return x.RunUuid
	}
	return ""
}

type GetVMSerialResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Data string `protobuf:"bytes,1,opt,name=data,proto3" json:"data,omitempty"`
}

func (x *GetVMSerialResponse) Reset() {
	*x = GetVMSerialResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_proto_api_proto_msgTypes[14]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *GetVMSerialResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GetVMSerialResponse) ProtoMessage() {}

func (x *GetVMSerialResponse) ProtoReflect() protoreflect.Message {
	mi := &file_proto_api_proto_msgTypes[14]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GetVMSerialResponse.ProtoReflect.Descriptor instead.
func (*GetVMSerialResponse) Descriptor() ([]byte, []int) {
	return file_proto_api_proto_rawDescGZIP(), []int{14}
}

func (x *GetVMSerialResponse) GetData() string {
	if x != nil {
		return x.Data
	}
	return ""
}

var File_proto_api_proto protoreflect.FileDescriptor

var file_proto_api_proto_rawDesc = []byte{
	0x0a, 0x0f, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x2f, 0x61, 0x70, 0x69, 0x2e, 0x70, 0x72, 0x6f, 0x74,
	0x6f, 0x12, 0x06, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x22, 0x22, 0x0a, 0x0c, 0x48, 0x65, 0x6c,
	0x6c, 0x6f, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x12, 0x0a, 0x04, 0x6e, 0x61, 0x6d,
	0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x29, 0x0a,
	0x0d, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x18,
	0x0a, 0x07, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52,
	0x07, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x22, 0x27, 0x0a, 0x11, 0x43, 0x72, 0x65, 0x61,
	0x74, 0x65, 0x55, 0x73, 0x65, 0x72, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x12, 0x0a,
	0x04, 0x6e, 0x61, 0x6d, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x6e, 0x61, 0x6d,
	0x65, 0x22, 0x34, 0x0a, 0x08, 0x55, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x12, 0x12, 0x0a,
	0x04, 0x6e, 0x61, 0x6d, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x6e, 0x61, 0x6d,
	0x65, 0x12, 0x14, 0x0a, 0x05, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09,
	0x52, 0x05, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x22, 0x43, 0x0a, 0x12, 0x43, 0x72, 0x65, 0x61, 0x74,
	0x65, 0x55, 0x73, 0x65, 0x72, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x2d, 0x0a,
	0x09, 0x75, 0x73, 0x65, 0x72, 0x5f, 0x70, 0x61, 0x69, 0x72, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b,
	0x32, 0x10, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x55, 0x73, 0x65, 0x72, 0x50, 0x61,
	0x69, 0x72, 0x52, 0x08, 0x75, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x22, 0x57, 0x0a, 0x12,
	0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x49, 0x6d, 0x61, 0x67, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65,
	0x73, 0x74, 0x12, 0x2d, 0x0a, 0x09, 0x75, 0x73, 0x65, 0x72, 0x5f, 0x70, 0x61, 0x69, 0x72, 0x18,
	0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x10, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x55,
	0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x52, 0x08, 0x75, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69,
	0x72, 0x12, 0x12, 0x0a, 0x04, 0x74, 0x65, 0x78, 0x74, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09, 0x52,
	0x04, 0x74, 0x65, 0x78, 0x74, 0x22, 0x34, 0x0a, 0x13, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x49,
	0x6d, 0x61, 0x67, 0x65, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x1d, 0x0a, 0x0a,
	0x69, 0x6d, 0x61, 0x67, 0x65, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09,
	0x52, 0x09, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x55, 0x75, 0x69, 0x64, 0x22, 0x5c, 0x0a, 0x0c, 0x52,
	0x75, 0x6e, 0x56, 0x4d, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x2d, 0x0a, 0x09, 0x75,
	0x73, 0x65, 0x72, 0x5f, 0x70, 0x61, 0x69, 0x72, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x10,
	0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x55, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72,
	0x52, 0x08, 0x75, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x12, 0x1d, 0x0a, 0x0a, 0x69, 0x6d,
	0x61, 0x67, 0x65, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x09,
	0x69, 0x6d, 0x61, 0x67, 0x65, 0x55, 0x75, 0x69, 0x64, 0x22, 0x2a, 0x0a, 0x0d, 0x52, 0x75, 0x6e,
	0x56, 0x4d, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x19, 0x0a, 0x08, 0x72, 0x75,
	0x6e, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x72, 0x75,
	0x6e, 0x55, 0x75, 0x69, 0x64, 0x22, 0x5d, 0x0a, 0x11, 0x47, 0x65, 0x74, 0x56, 0x4d, 0x53, 0x74,
	0x61, 0x74, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x2d, 0x0a, 0x09, 0x75, 0x73,
	0x65, 0x72, 0x5f, 0x70, 0x61, 0x69, 0x72, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x10, 0x2e,
	0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x55, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x52,
	0x08, 0x75, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x12, 0x19, 0x0a, 0x08, 0x72, 0x75, 0x6e,
	0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x72, 0x75, 0x6e,
	0x55, 0x75, 0x69, 0x64, 0x22, 0x8a, 0x01, 0x0a, 0x12, 0x47, 0x65, 0x74, 0x56, 0x4d, 0x53, 0x74,
	0x61, 0x74, 0x65, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x38, 0x0a, 0x05, 0x73,
	0x74, 0x61, 0x74, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0e, 0x32, 0x22, 0x2e, 0x6d, 0x6f, 0x64,
	0x65, 0x6c, 0x73, 0x2e, 0x47, 0x65, 0x74, 0x56, 0x4d, 0x53, 0x74, 0x61, 0x74, 0x65, 0x52, 0x65,
	0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x2e, 0x56, 0x4d, 0x53, 0x74, 0x61, 0x74, 0x65, 0x52, 0x05,
	0x73, 0x74, 0x61, 0x74, 0x65, 0x22, 0x3a, 0x0a, 0x07, 0x56, 0x4d, 0x53, 0x74, 0x61, 0x74, 0x65,
	0x12, 0x0b, 0x0a, 0x07, 0x52, 0x55, 0x4e, 0x4e, 0x49, 0x4e, 0x47, 0x10, 0x00, 0x12, 0x0b, 0x0a,
	0x07, 0x53, 0x55, 0x43, 0x43, 0x45, 0x53, 0x53, 0x10, 0x01, 0x12, 0x0a, 0x0a, 0x06, 0x4b, 0x49,
	0x4c, 0x4c, 0x45, 0x44, 0x10, 0x02, 0x12, 0x09, 0x0a, 0x05, 0x45, 0x52, 0x52, 0x4f, 0x52, 0x10,
	0x03, 0x22, 0x59, 0x0a, 0x0d, 0x4b, 0x69, 0x6c, 0x6c, 0x56, 0x4d, 0x52, 0x65, 0x71, 0x75, 0x65,
	0x73, 0x74, 0x12, 0x2d, 0x0a, 0x09, 0x75, 0x73, 0x65, 0x72, 0x5f, 0x70, 0x61, 0x69, 0x72, 0x18,
	0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x10, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x55,
	0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x52, 0x08, 0x75, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69,
	0x72, 0x12, 0x19, 0x0a, 0x08, 0x72, 0x75, 0x6e, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x02, 0x20,
	0x01, 0x28, 0x09, 0x52, 0x07, 0x72, 0x75, 0x6e, 0x55, 0x75, 0x69, 0x64, 0x22, 0x10, 0x0a, 0x0e,
	0x4b, 0x69, 0x6c, 0x6c, 0x56, 0x4d, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x22, 0x5e,
	0x0a, 0x12, 0x47, 0x65, 0x74, 0x56, 0x4d, 0x53, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x52, 0x65, 0x71,
	0x75, 0x65, 0x73, 0x74, 0x12, 0x2d, 0x0a, 0x09, 0x75, 0x73, 0x65, 0x72, 0x5f, 0x70, 0x61, 0x69,
	0x72, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x10, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73,
	0x2e, 0x55, 0x73, 0x65, 0x72, 0x50, 0x61, 0x69, 0x72, 0x52, 0x08, 0x75, 0x73, 0x65, 0x72, 0x50,
	0x61, 0x69, 0x72, 0x12, 0x19, 0x0a, 0x08, 0x72, 0x75, 0x6e, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18,
	0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x72, 0x75, 0x6e, 0x55, 0x75, 0x69, 0x64, 0x22, 0x29,
	0x0a, 0x13, 0x47, 0x65, 0x74, 0x56, 0x4d, 0x53, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x52, 0x65, 0x73,
	0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x12, 0x0a, 0x04, 0x64, 0x61, 0x74, 0x61, 0x18, 0x01, 0x20,
	0x01, 0x28, 0x09, 0x52, 0x04, 0x64, 0x61, 0x74, 0x61, 0x2a, 0x24, 0x0a, 0x0a, 0x53, 0x74, 0x61,
	0x74, 0x75, 0x73, 0x43, 0x6f, 0x64, 0x65, 0x12, 0x0b, 0x0a, 0x07, 0x53, 0x55, 0x43, 0x43, 0x45,
	0x53, 0x53, 0x10, 0x00, 0x12, 0x09, 0x0a, 0x05, 0x45, 0x52, 0x52, 0x4f, 0x52, 0x10, 0x01, 0x32,
	0xfd, 0x01, 0x0a, 0x04, 0x57, 0x65, 0x72, 0x6b, 0x12, 0x34, 0x0a, 0x05, 0x48, 0x65, 0x6c, 0x6c,
	0x6f, 0x12, 0x14, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x48, 0x65, 0x6c, 0x6c, 0x6f,
	0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x15, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73,
	0x2e, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x41,
	0x0a, 0x08, 0x52, 0x65, 0x67, 0x69, 0x73, 0x74, 0x65, 0x72, 0x12, 0x19, 0x2e, 0x6d, 0x6f, 0x64,
	0x65, 0x6c, 0x73, 0x2e, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x55, 0x73, 0x65, 0x72, 0x52, 0x65,
	0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x1a, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x43,
	0x72, 0x65, 0x61, 0x74, 0x65, 0x55, 0x73, 0x65, 0x72, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73,
	0x65, 0x12, 0x46, 0x0a, 0x0b, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x49, 0x6d, 0x61, 0x67, 0x65,
	0x12, 0x1a, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65,
	0x49, 0x6d, 0x61, 0x67, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x1b, 0x2e, 0x6d,
	0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x49, 0x6d, 0x61, 0x67,
	0x65, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x34, 0x0a, 0x05, 0x52, 0x75, 0x6e,
	0x56, 0x4d, 0x12, 0x14, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x2e, 0x52, 0x75, 0x6e, 0x56,
	0x4d, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x15, 0x2e, 0x6d, 0x6f, 0x64, 0x65, 0x6c,
	0x73, 0x2e, 0x52, 0x75, 0x6e, 0x56, 0x4d, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x42,
	0x0b, 0x5a, 0x09, 0x2e, 0x2e, 0x2f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x73, 0x62, 0x06, 0x70, 0x72,
	0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_proto_api_proto_rawDescOnce sync.Once
	file_proto_api_proto_rawDescData = file_proto_api_proto_rawDesc
)

func file_proto_api_proto_rawDescGZIP() []byte {
	file_proto_api_proto_rawDescOnce.Do(func() {
		file_proto_api_proto_rawDescData = protoimpl.X.CompressGZIP(file_proto_api_proto_rawDescData)
	})
	return file_proto_api_proto_rawDescData
}

var file_proto_api_proto_enumTypes = make([]protoimpl.EnumInfo, 2)
var file_proto_api_proto_msgTypes = make([]protoimpl.MessageInfo, 15)
var file_proto_api_proto_goTypes = []interface{}{
	(StatusCode)(0),                 // 0: models.StatusCode
	(GetVMStateResponse_VMState)(0), // 1: models.GetVMStateResponse.VMState
	(*HelloRequest)(nil),            // 2: models.HelloRequest
	(*HelloResponse)(nil),           // 3: models.HelloResponse
	(*CreateUserRequest)(nil),       // 4: models.CreateUserRequest
	(*UserPair)(nil),                // 5: models.UserPair
	(*CreateUserResponse)(nil),      // 6: models.CreateUserResponse
	(*CreateImageRequest)(nil),      // 7: models.CreateImageRequest
	(*CreateImageResponse)(nil),     // 8: models.CreateImageResponse
	(*RunVMRequest)(nil),            // 9: models.RunVMRequest
	(*RunVMResponse)(nil),           // 10: models.RunVMResponse
	(*GetVMStateRequest)(nil),       // 11: models.GetVMStateRequest
	(*GetVMStateResponse)(nil),      // 12: models.GetVMStateResponse
	(*KillVMRequest)(nil),           // 13: models.KillVMRequest
	(*KillVMResponse)(nil),          // 14: models.KillVMResponse
	(*GetVMSerialRequest)(nil),      // 15: models.GetVMSerialRequest
	(*GetVMSerialResponse)(nil),     // 16: models.GetVMSerialResponse
}
var file_proto_api_proto_depIdxs = []int32{
	5,  // 0: models.CreateUserResponse.user_pair:type_name -> models.UserPair
	5,  // 1: models.CreateImageRequest.user_pair:type_name -> models.UserPair
	5,  // 2: models.RunVMRequest.user_pair:type_name -> models.UserPair
	5,  // 3: models.GetVMStateRequest.user_pair:type_name -> models.UserPair
	1,  // 4: models.GetVMStateResponse.state:type_name -> models.GetVMStateResponse.VMState
	5,  // 5: models.KillVMRequest.user_pair:type_name -> models.UserPair
	5,  // 6: models.GetVMSerialRequest.user_pair:type_name -> models.UserPair
	2,  // 7: models.Werk.Hello:input_type -> models.HelloRequest
	4,  // 8: models.Werk.Register:input_type -> models.CreateUserRequest
	7,  // 9: models.Werk.CreateImage:input_type -> models.CreateImageRequest
	9,  // 10: models.Werk.RunVM:input_type -> models.RunVMRequest
	3,  // 11: models.Werk.Hello:output_type -> models.HelloResponse
	6,  // 12: models.Werk.Register:output_type -> models.CreateUserResponse
	8,  // 13: models.Werk.CreateImage:output_type -> models.CreateImageResponse
	10, // 14: models.Werk.RunVM:output_type -> models.RunVMResponse
	11, // [11:15] is the sub-list for method output_type
	7,  // [7:11] is the sub-list for method input_type
	7,  // [7:7] is the sub-list for extension type_name
	7,  // [7:7] is the sub-list for extension extendee
	0,  // [0:7] is the sub-list for field type_name
}

func init() { file_proto_api_proto_init() }
func file_proto_api_proto_init() {
	if File_proto_api_proto != nil {
		return
	}
	if !protoimpl.UnsafeEnabled {
		file_proto_api_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*HelloRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*HelloResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CreateUserRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*UserPair); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[4].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CreateUserResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[5].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CreateImageRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[6].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CreateImageResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[7].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*RunVMRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[8].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*RunVMResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[9].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*GetVMStateRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[10].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*GetVMStateResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[11].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*KillVMRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[12].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*KillVMResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[13].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*GetVMSerialRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_proto_api_proto_msgTypes[14].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*GetVMSerialResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_proto_api_proto_rawDesc,
			NumEnums:      2,
			NumMessages:   15,
			NumExtensions: 0,
			NumServices:   1,
		},
		GoTypes:           file_proto_api_proto_goTypes,
		DependencyIndexes: file_proto_api_proto_depIdxs,
		EnumInfos:         file_proto_api_proto_enumTypes,
		MessageInfos:      file_proto_api_proto_msgTypes,
	}.Build()
	File_proto_api_proto = out.File
	file_proto_api_proto_rawDesc = nil
	file_proto_api_proto_goTypes = nil
	file_proto_api_proto_depIdxs = nil
}
