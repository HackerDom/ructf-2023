import requests


REQUEST_URL_PATTERN = "http://{host}:{port}/{method}"


def register(host, port, username, payment_info):
    url = REQUEST_URL_PATTERN.format(host=host, port=port, method='register')
    r = requests.post(url, timeout=10, json={'username': username, 'payment_info': payment_info})
    r.raise_for_status()
    res = r.json()
    return res['token'], res['lucky_numbers']


def get_info(host, port, username, token):
    url = REQUEST_URL_PATTERN.format(host=host, port=port, method='get_info')
    r = requests.post(url, timeout=10, json={'username': username, 'token': token})
    r.raise_for_status()
    res = r.json()
    return res['payment_info'], res['lucky_numbers']
