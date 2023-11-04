#!/usr/bin/env python3.11

import io
import os
import random
import secrets
import hashlib
import subprocess
from typing import Callable, List

import gornilo
import requests.exceptions
from PIL import Image

import api


port = 17171
checker = gornilo.NewChecker()


def generate_password() -> str:
    return secrets.token_hex(8)


def generate_flag_background() -> bytes:
    r = random.randrange(200, 256)
    g = random.randrange(200, 256)
    b = random.randrange(200, 256)

    width = random.randrange(1400, 1800)
    height = random.randrange(300, 500)

    buffer = io.BytesIO()

    img = Image.new('RGB', (width, height), (r, g, b))
    img.save(buffer, 'jpeg')

    buffer.seek(0)

    return buffer.read()


def generate_flag_labels(flag: str) -> api.Label:
    r = random.randrange(0, 100)
    g = random.randrange(0, 100)
    b = random.randrange(0, 100)

    x = random.randrange(10, 50)
    y = random.randrange(5, 40)

    size = random.randrange(24, 32)
    offset = random.randrange(32, 48)

    label1 = api.Label(
        font = 'bold',
        color = '#%02x%02x%02x' % (r, g, b),
        position = (x, y + 1 * offset),
        size = size,
        text = flag[:64],
    )

    label2 = api.Label(
        font = 'bold',
        color = '#%02x%02x%02x' % (r, g, b),
        position = (x, y + 2 * offset),
        size = size,
        text = flag[64:128],
    )

    label3 = api.Label(
        font = 'bold',
        color = '#%02x%02x%02x' % (r, g, b),
        position = (x, y + 3 * offset),
        size = size,
        text = flag[128:192],
    )

    label4 = api.Label(
        font = 'bold',
        color = '#%02x%02x%02x' % (r, g, b),
        position = (x, y + 4 * offset),
        size = size,
        text = flag[192:256],
    )

    return [label1, label2, label3, label4]


def generate_meme_background() -> bytes:
    return generate_flag_background()


def generate_meme_labels() -> List[api.Label]:
    return generate_flag_labels('kekosik')


def recognize_text(image: bytes) -> str:
    filename = secrets.token_hex(8) + '.jpg'

    buffer = io.BytesIO(image)
    img = Image.open(buffer)

    img = img.convert('L')
    img = img.point( lambda p: 255 if p > 128 else 0 )
    img = img.convert('1')

    img.save(filename, 'jpeg')

    process = subprocess.Popen(
        [
            'tesseract',
            '--tessdata-dir', '.',
            '-l', 'final',
            filename,
            'stdout',
        ],
        stdout = subprocess.PIPE,
    )

    content, _ = process.communicate()

    os.unlink(filename)

    return content.decode()


def do_put(request: gornilo.PutRequest) -> gornilo.Verdict:
    client = api.Api(request.hostname, port)
    client.index()

    flag = bin(int.from_bytes(request.flag.encode(), 'big'))[2:]
    flag = '0' * (len(request.flag) * 8 - len(flag)) + flag

    background = generate_flag_background()
    labels = generate_flag_labels(flag)

    image_uuid = client.upload_image(background)

    if image_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to upload image')

    draft_uuid = client.create_draft(image_uuid)

    if draft_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to create draft')

    result = client.draw_text(draft_uuid, labels)

    if result is None:
        return gornilo.Verdict.MUMBLE('failed to draw text')

    password = generate_password()
    image_uuid = client.draft_release(draft_uuid, password)

    if image_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to release draft')

    image = client.get_image(image_uuid, password)

    if image is None:
        return gornilo.Verdict.MUMBLE('failed to get image')
    
    image_hash = hashlib.sha256(image).hexdigest()

    public_flag_id = image_uuid
    private_flag_id = password + '|' + image_hash
    
    return gornilo.Verdict.OK_WITH_FLAG_ID(public_flag_id, private_flag_id)


def do_get(request: gornilo.GetRequest) -> gornilo.Verdict:
    client = api.Api(request.hostname, port)
    client.index()

    flag = bin(int.from_bytes(request.flag.encode(), 'big'))[2:]
    flag = '0' * (len(request.flag) * 8 - len(flag)) + flag

    image_uuid = request.public_flag_id
    password, expected_image_hash = request.flag_id.split('|')

    image = client.get_image(image_uuid, password)

    if image is None:
        return gornilo.Verdict.CORRUPT('failed to get flag')
    
    actual_image_hash = hashlib.sha256(image).hexdigest()

    if expected_image_hash != actual_image_hash:
        return gornilo.Verdict.MUMBLE('image was changed')
    
    recognized_flag = recognize_text(image)
    recognized_flag = recognized_flag.replace(' ', '').replace('\n', '')

    recognized_flag = ''.join(
        x for x in recognized_flag if x == '0' or x == '1'
    )

    if recognized_flag != flag:
        return gornilo.Verdict.CORRUPT(f'invalid flag')
    
    return gornilo.Verdict.OK()


def do_check(request: gornilo.CheckRequest) -> gornilo.Verdict:
    client = api.Api(request.hostname, port)
    client.index()

    return gornilo.Verdict.OK()


def wrap_exceptions(
        action: Callable[[gornilo.CheckRequest], gornilo.Verdict],
        request: gornilo.CheckRequest,
) -> gornilo.Verdict:
    try:
        return action(request)
    except requests.ConnectionError:
        return gornilo.Verdict.DOWN('connection error')
    except requests.Timeout:
        return gornilo.Verdict.DOWN('timeout error')
    except requests.RequestException:
        return gornilo.Verdict.MUMBLE('http error')


@checker.define_vuln('flag_id is image_uuid')
class MemosChecker(gornilo.VulnChecker):
    @staticmethod
    def put(request: gornilo.PutRequest) -> gornilo.Verdict:
        return wrap_exceptions(do_put, request)

    @staticmethod
    def get(request: gornilo.GetRequest) -> gornilo.Verdict:
        return wrap_exceptions(do_get, request)

    @checker.define_check
    def check(request: gornilo.CheckRequest) -> gornilo.Verdict:
        return wrap_exceptions(do_check, request)


if __name__ == '__main__':
    checker.run()
