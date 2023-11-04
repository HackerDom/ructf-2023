#!/usr/bin/env python3.11

import re
import dataclasses
from typing import List, Tuple

import gornilo
import requests


FONTS = [
    'extralight',
    'extralight-italic',
    'light',
    'light-italic',
    'thin',
    'thin-italic',
    'regular',
    'regular-italic',
    'medium',
    'medium-italic',
    'semibold',
    'semibold-italic',
    'bold',
    'bold-italic',
]


@dataclasses.dataclass
class Label:
    font: str
    color: str
    position: Tuple[int, int]
    size: int
    text: str

    def serialize(self) -> bytes:
        return b'|'.join(
            [
                self.font.encode(),
                self.color.encode(),
                str(self.position[0]).encode(),
                str(self.position[1]).encode(),
                str(self.size).encode(),
                self.text.encode('utf-8'),
            ],
        )


def validate_uuid(uuid: str) -> bool:
    match = re.match(
        r'^[0-9a-f]{8}\-[0-9a-f]{4}\-[0-9a-f]{4}\-[0-9a-f]{4}\-[0-9a-f]{12}$',
        uuid,
    )

    return bool(match)


class Api:
    def __init__(self, hostname: str, port: int) -> None:
        self.url = f'http://{hostname}:{port}'
        self.session = requests.session()

        return
    
    def close(self) -> None:
        self.session.close()

        return

    def index(self) -> None:
        response = self.session.get(f'{self.url}/')
        html = response.text

        if response.status_code != 200 or '<title>memos</title>' not in html:
            raise gornilo.Verdict.MUMBLE('broken index.html')
        
        return
    
    def get_image(self, image_uuid: str, password: str = None) -> bytes | None:
        if password is None:
            response = self.session.get(f'{self.url}/images/get/{image_uuid}')
        else:
            response = self.session.get(f'{self.url}/images/get/{image_uuid}?{password}')

        if response.status_code != 200:
            return None
        
        return response.content

    def list_images(self) -> List[str] | None:
        response = self.session.get(f'{self.url}/images/list')

        if response.status_code != 200:
            return None
        
        image_uuids = response.text.split(', ')

        for image_uuid in image_uuids:
            if not validate_uuid(image_uuid):
                raise gornilo.Verdict.MUMBLE('invalid image uuid')

        return image_uuids

    def view_draft(self, draft_uuid: str) -> bytes | None:
        response = self.session.get(f'{self.url}/drafts/view/{draft_uuid}')

        if response.status_code != 200:
            return None
        
        return response.content

    def upload_image(self, image_data: bytes) -> str | None:
        response = self.session.post(
            f'{self.url}/images/upload',
            data = image_data,
        )

        if response.status_code != 200:
            return None

        image_uuid = response.text

        if not validate_uuid(image_uuid):
            raise gornilo.Verdict.MUMBLE('invalid image uuid')

        return image_uuid

    def create_draft(self, image_uuid: str) -> str | None:
        response = self.session.post(
            f'{self.url}/drafts/create',
            data = image_uuid,
        )

        if response.status_code != 200:
            return None

        draft_uuid = response.text

        if not validate_uuid(draft_uuid):
            raise gornilo.Verdict.MUMBLE('invalid draft uuid')

        return draft_uuid

    def change_background(self, draft_uuid: str, image_uuid: str) -> bool | None:
        response = self.session.post(
            f'{self.url}/drafts/background/{draft_uuid}',
            data = image_uuid,
        )

        if response.status_code != 200:
            return None

        if len(response.content) > 0:
            raise gornilo.Verdict.MUMBLE('unexpected response body')

        return True
    
    def draw_text(self, draft_uuid: str, labels: List[Label]) -> bool | None:
        response = self.session.post(
            f'{self.url}/drafts/text/{draft_uuid}',
            data = b'||'.join(label.serialize() for label in labels),
        )

        if response.status_code != 200:
            return None

        if len(response.content) > 0:
            raise gornilo.Verdict.MUMBLE('unexpected response body')

        return True

    def release_draft(self, draft_uuid: str, password: str = None) -> str | None:
        response = self.session.post(
            f'{self.url}/drafts/release/{draft_uuid}',
            data = password,
        )

        if response.status_code != 200:
            return None

        image_uuid = response.text

        if not validate_uuid(image_uuid):
            raise gornilo.Verdict.MUMBLE('invalid image uuid')
        
        return image_uuid
