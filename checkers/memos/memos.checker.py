#!/usr/bin/env python3.11

import os
import secrets
import hashlib
import contextlib
import subprocess
import requests.exceptions
from typing import Callable

import gornilo

import api


checker = gornilo.NewChecker()


@contextlib.contextmanager
def make_client(hostname: str):
    client = api.Api(hostname)

    try:
        yield client
    finally:
        client.close()

    return


def draw_text(label: api.Label) -> bytes:
    filename = secrets.token_hex(8) + '.png'
    result = secrets.token_hex(8) + '.jpg'

    process = subprocess.Popen(
        [
            './magick',
            '-background', 'transparent',
            '-fill', label.color,
            '-font', f'./fonts/{label.font}.ttf',
            '-pointsize', str(label.size),
            f'label:{label.text}',
            filename,
        ],
        stdin = subprocess.DEVNULL,
        stdout = subprocess.DEVNULL,
        stderr = subprocess.DEVNULL,
    )
    process.wait()

    process = subprocess.Popen(
        [
            './magick',
            'background.jpg',
            filename,
            '-geometry', f'+{label.position[0]}+{label.position[1]}',
            '-composite',
            result,
        ],
        stdin = subprocess.DEVNULL,
        stdout = subprocess.DEVNULL,
        stderr = subprocess.DEVNULL,
    )
    process.wait()

    with open(result, 'rb') as file:
        data = file.read()

    os.unlink(result)
    os.unlink(filename)

    return data


def do_put(request: gornilo.PutRequest) -> gornilo.Verdict:
    with make_client(request.hostname) as client:
        client.index()

        with open('background.jpg', 'rb') as file:
            background = file.read()

        image_uuid = client.upload_image(background)

        if image_uuid is None:
            return gornilo.Verdict.MUMBLE('failed to upload image')
        
        draft_uuid = client.create_draft(image_uuid)

        if draft_uuid is None:
            return gornilo.Verdict.MUMBLE('failed to create draft')
        
        label = api.Label(
            font = 'bold',
            color = '#000000',
            position = (30, 30),
            size = 48,
            text = request.flag,
        )

        result = client.draw_text(draft_uuid, [label])

        if result is None:
            return gornilo.Verdict.MUMBLE('failed to draw text')
        
        password = secrets.token_hex(16)

        image_uuid = client.draft_release(draft_uuid, password)

        if image_uuid is None:
            return gornilo.Verdict.MUMBLE('failed to release draft')
        
        image = client.get_image(image_uuid, password)

        if image is None:
            return gornilo.Verdict.MUMBLE('failed to get image')
        
        image_hash = hashlib.sha256(image).hexdigest()

        constructed_image = draw_text(label)
        constructed_image_hash = hashlib.sha256(constructed_image).hexdigest()

        if constructed_image_hash != image_hash:
            return gornilo.Verdict.MUMBLE('invalid generation')

        public_flag_id = image_uuid
        private_flag_id = password + '|' + constructed_image_hash
        
        return gornilo.Verdict.OK_WITH_FLAG_ID(public_flag_id, private_flag_id)


def do_get(request: gornilo.GetRequest) -> gornilo.Verdict:
    with make_client(request.hostname) as client:
        client.index()

        image_uuid = request.public_flag_id
        password, expected_image_hash = request.flag_id.split('|')

        content = client.get_image(image_uuid, password)

        if content is None:
            return gornilo.Verdict.CORRUPT('failed to get flag')
        
        actual_image_hash = hashlib.sha256(content).hexdigest()

        if expected_image_hash != actual_image_hash:
            return gornilo.Verdict.CORRUPT('invalid flag')
        
        return gornilo.Verdict.OK()


def do_check(request: gornilo.CheckRequest) -> gornilo.Verdict:
    with make_client(request.hostname) as client:
        client.index()

        return gornilo.Verdict.OK()


def wrap_exceptions(
        action: Callable[[gornilo.CheckRequest], gornilo.Verdict],
        request: gornilo.CheckRequest,
) -> gornilo.Verdict:
    try:
        return action(request)
    # except model.ProtocolError as error:
    #     return gornilo.Verdict.MUMBLE(f'protocol error: {error}')
    # except model.ValidationError as error:
    #     return gornilo.Verdict.MUMBLE(f'validation error: {error}')
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
