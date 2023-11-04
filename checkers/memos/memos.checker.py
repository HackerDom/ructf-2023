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


def hash_image(content: bytes) -> str:
    return hashlib.sha256(content).hexdigest()


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
    x = random.randrange(10, 50)
    y = random.randrange(5, 40)

    size = random.randrange(24, 32)
    offset = random.randrange(32, 48)

    labels = []

    for i in range(4):
        r = random.randrange(0, 100)
        g = random.randrange(0, 100)
        b = random.randrange(0, 100)

        label = api.Label(
            font = 'bold',
            color = '#%02x%02x%02x' % (r, g, b),
            position = (x, y + (i + 1) * offset),
            size = size,
            text = flag[i*64 : (i + 1)*64],
        )

        labels.append(label)

    return labels


def generate_meme_background() -> bytes:
    backgrounds = [
        'images/00.jpg',
        'images/01.jpg',
        'images/02.jpg',
        'images/03.jpg',
        'images/04.jpg',
        'images/05.jpg',
        'images/06.jpg',
        'images/07.jpg',
        'images/08.jpg',
        'images/09.jpg',
    ]

    background = random.choice(backgrounds)

    img = Image.open(background)
    width, height = img.size

    coeff = 0.2 * random.random() - 0.1

    width = width + int(width * coeff)
    height = height + int(height * coeff)

    buffer = io.BytesIO()

    img = img.resize((width, height))
    img.save(buffer, 'jpeg')

    buffer.seek(0)

    return buffer.read()


def generate_meme_labels() -> List[api.Label]:
    labels_count = random.randint(1, 4)
    labels = []

    for _ in range(labels_count):
        font = random.choice(api.FONTS)

        if random.choice([True, False]):
            x = random.randrange(0, 500)
            y = random.randrange(0, 500)
        else:
            x = random.randrange(500, 1000)
            y = random.randrange(500, 1000)

        size = random.randrange(16, 200)

        r = random.randrange(0, 256)
        g = random.randrange(0, 100)
        b = random.randrange(0, 100)

        choice = random.randint(0, 1)

        if choice == 0:
            alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ !,.'
        else:
            alphabet = 'абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ !,.'

        length = random.randrange(8, 32)    
        text = ''.join(
            random.choice(alphabet) for _ in range(length)
        )

        label = api.Label(
            font = font,
            color = '#%02x%02x%02x' % (r, g, b),
            position = (x, y),
            size = size,
            text = text,
        )

        labels.append(label)

    return labels


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
    image_uuid = client.release_draft(draft_uuid, password)

    if image_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to release draft')

    image = client.get_image(image_uuid, password)

    if image is None:
        return gornilo.Verdict.MUMBLE('failed to get image')
    
    image_hash = hash_image(image)

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

    actual_image_hash = hash_image(image)

    if expected_image_hash != actual_image_hash:
        return gornilo.Verdict.CORRUPT('flag was changed')
    
    if random.random() < 0.2:
        recognized_flag = recognize_text(image)

        recognized_flag = recognized_flag.replace(' ', '').replace('\n', '')
        recognized_flag = recognized_flag.replace('O', '0')

        recognized_flag = ''.join(
            x for x in recognized_flag if x == '0' or x == '1'
        )

        if recognized_flag != flag:
            return gornilo.Verdict.CORRUPT(f'invalid flag')
    
    return gornilo.Verdict.OK()


def do_check(request: gornilo.CheckRequest) -> gornilo.Verdict:
    client = api.Api(request.hostname, port)
    client.index()

    background = generate_meme_background()
    labels = generate_meme_labels()

    image_uuid = client.upload_image(background)

    if image_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to upload image')
    
    draft_uuid = client.create_draft(image_uuid)

    if draft_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to create draft')

    result = client.draw_text(draft_uuid, labels)

    if result is None:
        return gornilo.Verdict.MUMBLE('failed to draw text')
    
    draft_content = client.view_draft(draft_uuid)

    if draft_content is None:
        return gornilo.Verdict.MUMBLE('failed to view draft')
    
    draft_content_hash = hash_image(draft_content)

    client.close()

    client = api.Api(request.hostname, port)
    client.index()
    
    regenerate = random.choice([True, False, False, False])

    if regenerate:
        background = generate_meme_background()
        labels = generate_meme_labels()

        image_uuid = client.upload_image(background)

        if image_uuid is None:
            return gornilo.Verdict.MUMBLE('failed to upload image')

        result = client.change_background(draft_uuid, image_uuid)

        if result is None:
            return gornilo.Verdict.MUMBLE('failed to change background')
        
        result = client.draw_text(draft_uuid, labels)

        if result is None:
            return gornilo.Verdict.MUMBLE('failed to draw text')
        
        draft_content = client.view_draft(draft_uuid)

        if draft_content is None:
            return gornilo.Verdict.MUMBLE('failed to view draft')
        
        draft_content_hash = hash_image(draft_content)

        client.close()

        client = api.Api(request.hostname, port)
        client.index()

    use_password = random.choice([True, False])

    if use_password:
        password = generate_password()
    else:
        password = None

    image_uuid = client.release_draft(draft_uuid, password)

    if image_uuid is None:
        return gornilo.Verdict.MUMBLE('failed to release draft')
    
    image_content = client.get_image(image_uuid, password)

    if image_content is None:
        return gornilo.Verdict.MUMBLE('failed to get image')
    
    image_content_hash = hash_image(image_content)

    if image_content_hash != draft_content_hash:
        return gornilo.Verdict.MUMBLE('invalid image release')
    
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
