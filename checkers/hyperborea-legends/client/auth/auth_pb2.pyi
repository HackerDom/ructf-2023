from typing import ClassVar as _ClassVar, Optional as _Optional

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message

DESCRIPTOR: _descriptor.FileDescriptor


class RegisterRequest(_message.Message):
    __slots__ = ["username", "password", "species_type"]
    USERNAME_FIELD_NUMBER: _ClassVar[int]
    PASSWORD_FIELD_NUMBER: _ClassVar[int]
    SPECIES_TYPE_FIELD_NUMBER: _ClassVar[int]
    username: str
    password: str
    species_type: str

    def __init__(self, username: _Optional[str] = ..., password: _Optional[str] = ...,
                 species_type: _Optional[str] = ...) -> None: ...


class LoginRequest(_message.Message):
    __slots__ = ["username", "password"]
    USERNAME_FIELD_NUMBER: _ClassVar[int]
    PASSWORD_FIELD_NUMBER: _ClassVar[int]
    username: str
    password: str

    def __init__(self, username: _Optional[str] = ..., password: _Optional[str] = ...) -> None: ...


class RegisterReply(_message.Message):
    __slots__ = ["success"]
    SUCCESS_FIELD_NUMBER: _ClassVar[int]
    success: bool

    def __init__(self, success: bool = ...) -> None: ...


class LoginReply(_message.Message):
    __slots__ = ["access_token"]
    ACCESS_TOKEN_FIELD_NUMBER: _ClassVar[int]
    access_token: str

    def __init__(self, access_token: _Optional[str] = ...) -> None: ...
