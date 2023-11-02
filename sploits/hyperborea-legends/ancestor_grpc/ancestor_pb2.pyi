from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class GetAncestorRequest(_message.Message):
    __slots__ = ["id"]
    ID_FIELD_NUMBER: _ClassVar[int]
    id: bytes
    def __init__(self, id: _Optional[bytes] = ...) -> None: ...

class AncestorData(_message.Message):
    __slots__ = ["id", "name", "description", "ancestor_type", "burial_place"]
    ID_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    DESCRIPTION_FIELD_NUMBER: _ClassVar[int]
    ANCESTOR_TYPE_FIELD_NUMBER: _ClassVar[int]
    BURIAL_PLACE_FIELD_NUMBER: _ClassVar[int]
    id: bytes
    name: str
    description: str
    ancestor_type: str
    burial_place: str
    def __init__(self, id: _Optional[bytes] = ..., name: _Optional[str] = ..., description: _Optional[str] = ..., ancestor_type: _Optional[str] = ..., burial_place: _Optional[str] = ...) -> None: ...

class CreateAncestorResponse(_message.Message):
    __slots__ = ["id"]
    ID_FIELD_NUMBER: _ClassVar[int]
    id: bytes
    def __init__(self, id: _Optional[bytes] = ...) -> None: ...

class AddDirectAncestorRequest(_message.Message):
    __slots__ = ["ancestor_id"]
    ANCESTOR_ID_FIELD_NUMBER: _ClassVar[int]
    ancestor_id: bytes
    def __init__(self, ancestor_id: _Optional[bytes] = ...) -> None: ...

class AddDirectAncestorResponse(_message.Message):
    __slots__ = ["success"]
    SUCCESS_FIELD_NUMBER: _ClassVar[int]
    success: bool
    def __init__(self, success: bool = ...) -> None: ...

class EmptyRequest(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class AncestorsCountResponse(_message.Message):
    __slots__ = ["count"]
    COUNT_FIELD_NUMBER: _ClassVar[int]
    count: int
    def __init__(self, count: _Optional[int] = ...) -> None: ...
