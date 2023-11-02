import urllib.parse
import base69
from gornilo.http_clients import requests_with_retries
import re

class APIProtocolError(Exception):
    pass


class APIValidationError(Exception):
    pass


class APIClient:
    def __init__(self, host: str, port: int, token: str = None):
        self._addr = f'http://{host}:{port}'
        self._session = requests_with_retries(
            status_forcelist=(500, 502, 503))
        self._token = token
        if token is not None:
            self.set_token(token)

    def __enter__(self):
        return self

    def _route(self, path: str) -> str:
        return f'{self._addr}{path}'

    def __exit__(self, exc_type, exc_value, exc_traceback):
        return self._session.__exit__(exc_type, exc_value, exc_traceback)

    def register(self, username, password):
        resp = self._session.post(self._route('/auth/register/'), json={
            'username': username,
            'password': password
        })
        if resp.status_code != 201:
            raise APIProtocolError(f'unexpected status code register: {resp.status_code}')

    def set_token(self, token):
        self._session.headers.update({'x-access-token': token})
        self._token = token

    def login(self, username, password):
        resp = self._session.post(self._route('/auth/login/'), json={
            'username': username,
            'password': password
        })
        if resp.status_code != 200:
            raise APIProtocolError(f'unexpected status code login: {resp.status_code}')
        data = resp.json()
        if "accessToken" not in data:
            raise APIValidationError(f'no excepted field in json: accessToken')
        token = data['accessToken']
        if not isinstance(token, str):
            raise APIValidationError(f'no excepted type of accessToken')
        self.set_token(token)
        return token

    def drink_water(self, key):
        resp = self._session.post(self._route('/voda/drink/'), params={'lake': key})
        data = resp.json()
        if resp.status_code == 201:
            if not isinstance(data, list) or len(data) != 1 or not isinstance(data[0], dict) or "water" not in data[0]:
                raise APIValidationError(f'Wrong response from drink')
            try:
                return int(data[0]['water'])
            except Exception:
                raise APIValidationError(f'cant parse int from resp')
        if resp.status_code == 400:
            if "details" not in data:
                raise APIValidationError(f'no excepted field in json: details')
            return data['details']
        raise APIProtocolError(f'unexpected status code drink_water: {resp.status_code}')

    def get_user_data(self, username):
        resp = self._session.get(self._route(f'/users/{username}'))
        if resp.status_code != 200:
            raise APIProtocolError(f'unexpected status code get_user_data: {resp.status_code}')
        data = resp.json()
        if "id" not in data:
            raise APIValidationError(f'no excepted field in json: id')
        if "username" not in data:
            raise APIValidationError(f'no excepted field in json: username')
        return data

    def get_udar(self, name):
        resp = self._session.get(self._route(f'/udar/{name}/'))
        if resp.status_code not in (200, 400, 401, 404):
            raise APIProtocolError(f'unexpected status code get_udar: {resp.status_code}')
        if resp.status_code == 404:
            return resp.status_code, None
        return resp.status_code, resp.json()

    def make_udar(self, name, phonk, trusted_ids, map_id, description):
        encoded_name = base69.encodeString(name)
        resp = self._session.put(self._route('/udar/'), json={
            'name': encoded_name,
            'phonk': phonk,
            'map': map_id,
            'description': description,
            'trustedRusi': trusted_ids
        })
        if resp.status_code == 201:
            return None
        if resp.status_code not in (400, 401, 422):
            raise APIProtocolError(f'unexpected status code make_udar: {resp.status_code}')
        return resp.json()

    def check_frontend(self):
        resp = self._session.get(self._route('/'))
        if resp.status_code not in (200, 201, 304):
            raise APIProtocolError(f'unexpected status code on front: {resp.status_code}')
        res = re.search(r'<script type="module" crossorigin src="(\/assets\/index-.+\.js)">', resp.text)
        if res is None:
            raise APIProtocolError(f'Cant find script in frontend index')
        resp_js = self._session.get(self._route(res.group(1)))
        if resp_js.status_code not in (200, 201, 304):
            raise APIProtocolError(f'unexpected status code on front js: {resp_js.status_code}')
        res = re.search(r'<link rel="stylesheet" href="(/assets/index-.+\.css)">', resp.text)
        if res is None:
            raise APIProtocolError(f'Cant find css in frontend index')
        resp_css = self._session.get(self._route(res.group(1)))
        if resp_css.status_code not in (200, 201, 304):
            raise APIProtocolError(f'unexpected status code on front css: {resp_css.status_code}')

    def get_user_udars(self):
        resp = self._session.get(self._route('/udar/'))
        if resp.status_code != 200:
            raise APIProtocolError(f'unexpected status code on get_user_udars: {resp.status_code}')
        data = resp.json()
        if not isinstance(data, list):
            raise APIProtocolError(f'unexpected return type from get_user_udars')
        return data

