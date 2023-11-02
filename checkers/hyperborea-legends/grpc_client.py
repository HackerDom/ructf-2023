import uuid
from typing import Callable, Any

import grpc

import utils
from client.ancestor.ancestor_pb2 import (
    AncestorData,
    AddDirectAncestorRequest,
    AddDirectAncestorResponse,
    CreateAncestorResponse,
    GetAncestorRequest,
    EmptyRequest,
    AncestorsCountResponse
)
from client.ancestor.ancestor_pb2_grpc import AncestorServStub
from client.auth.auth_pb2 import (
    RegisterRequest,
    RegisterReply,
    LoginRequest,
    LoginReply
)
from client.auth.auth_pb2_grpc import AutherStub


def _send_request(request: Callable, request_data: Any, api: str, metadata: tuple = ()):
    try:
        return request(request_data, metadata=metadata)
    except grpc.RpcError as rpc_error:
        if rpc_error.code() == grpc.StatusCode.UNAVAILABLE:
            raise utils.raise_grpc_error('Server unavailable')
        verdict_msg = f"Invalid status code: {rpc_error.code()} for {api}"
        msg = f"send: {request_data}"
        utils.raise_invalid_grpc_code_error(verdict_msg, msg)


class Client:
    def __init__(self, host: str, port: int):
        self.url = f'{host}:{port}'

    def __enter__(self):
        self.channel = grpc.insecure_channel(self.url)
        self.auth_stub = AutherStub(self.channel)
        self.ancestor_stub = AncestorServStub(self.channel)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.channel.close()

    def register_user(self, username: str, password: str, species_type: str) -> RegisterReply:
        return _send_request(
            self.auth_stub.Register,
            RegisterRequest(username=username, password=password, species_type=species_type),
            'Register'
        )

    def login_user(self, username: str, password: str) -> LoginReply:
        return _send_request(
            self.auth_stub.Login,
            LoginRequest(username=username, password=password),
            'Login'
        )

    def get_ancestor(self, id: uuid.UUID, token: str) -> AncestorData:
        return _send_request(
            self.ancestor_stub.GetAncestor,
            GetAncestorRequest(id=id.bytes),
            'GetAncestor',
            metadata=(('authorization', f'Bearer {token}'),),
        )

    def create_ancestor(
            self,
            id: uuid.UUID,
            name: str,
            description: str,
            ancestor_type: str,
            burial_place: str,
            token: str
    ) -> CreateAncestorResponse:
        return _send_request(
            self.ancestor_stub.CreateAncestor,
            AncestorData(
                id=id.bytes,
                name=name,
                description=description,
                ancestor_type=ancestor_type,
                burial_place=burial_place
            ),
            'CreateAncestor',
            metadata=(('authorization', f'Bearer {token}'),),
        )

    def add_direct_ancestor(self, ancestor_id: uuid.UUID, token: str) -> AddDirectAncestorResponse:
        return _send_request(
            self.ancestor_stub.AddDirectAncestor,
            AddDirectAncestorRequest(ancestor_id=ancestor_id.bytes),
            'AddDirectAncestor',
            metadata=(('authorization', f'Bearer {token}'),),
        )

    def get_ancestors_count(self, token: str) -> AncestorsCountResponse:
        return _send_request(
            self.ancestor_stub.GetAncestorsCount,
            EmptyRequest(),
            'GetAncestorsCount',
            metadata=(('authorization', f'Bearer {token}'),),
        )
