#!/usr/bin/env python3.11

import random

import gornilo
from gornilo.utils import generate_flag

from generators import generate_while_i, generate_name
from utils import wrap_exceptions, random_string
import api
PORT = 41114

checker = gornilo.NewChecker()


def check_drink_water(c):
    res_start = c.drink_water('baikal')
    if not isinstance(res_start, int):
        print(str(res_start)[:100])
        return gornilo.Verdict.MUMBLE(f'No valid result for drink_water')
    res = c.drink_water(random_string(len('baikal'), 'sqezvrwoxugpmhdcytfjn'))
    if res != 'GO AWAY LIZARD!!!':
        print(str(res)[:100])
        return gornilo.Verdict.MUMBLE(f'No valid result for bad drink_water')
    res_another = c.drink_water('baikal')
    if not isinstance(res_another, int):
        print(str(res_another)[:100])
        return gornilo.Verdict.MUMBLE(f'No valid result for drink_water')
    if res_start <= res_another:
        return gornilo.Verdict.MUMBLE(f'No valid drink water. Not drink something')
    return None


def register_user(c: api.APIClient):
    username = random_string(25)
    password = random_string(40)
    c.register(username, password)
    token = c.login(username, password)
    res = c.get_user_data(username)
    if not isinstance(res["id"], int):
        raise api.APIValidationError(f'no excepted type in id user data : id')
    if not isinstance(res["username"], str):
        raise api.APIValidationError(f'no excepted type in id user data : username')
    if res["username"] != username:
        raise api.APIValidationError(f'no excepted username in user data')
    res["token"] = token
    res["password"] = password
    return res


def validate_udar(resp, name, phonk, teacher):
    code, data = resp
    if code != 200:
        raise api.APIValidationError(f'unexpected status code: {code}')
    if 'name' not in data or data['name'] != name.upper():
        raise api.APIValidationError(f'Wrong name of get udar')
    if 'phonk' not in data or data['phonk'] != phonk:
        raise api.APIValidationError(f'Wrong phonk of get udar')
    if 'map' not in data or not isinstance(data['map'], int):
        raise api.APIValidationError(f'Wrong map of get udar')
    if 'description' not in data or not isinstance(data['description'], str):
        raise api.APIValidationError(f'Wrong description of get udar')
    if 'trustedRusi' not in data or not isinstance(data['trustedRusi'], list):
        raise api.APIValidationError(f'Wrong trustedRusi of get udar')
    if 'teacherId' not in data or teacher != data['teacherId']:
        raise api.APIValidationError(f'Wrong teacherId of get udar')
    return data


def check_valid_udar(c: api.APIClient):
    fake_flag = generate_flag()
    name = generate_name()
    user = register_user(c)
    map_id = random.randint(1, 4)
    description = random_string(random.randint(90, 120))
    # Check is create correctly
    res = c.make_udar(name, fake_flag, [], map_id, description)
    if res is not None:
        print(str(res)[:100])
        raise api.APIValidationError(f'Not valid result for make udar')
    validate_udar(c.get_udar(name), name, fake_flag, user['id'])
    # Check is not exist return 404
    code, data = c.get_udar(name + random_string(10))
    if code != 404:
        raise api.APIValidationError(f'unexpected status code: {code}')
    # Check is upper case name return right
    validate_udar(c.get_udar(name.upper()), name, fake_flag, user['id'])
    # Check is another user cant read not your item
    another_user = register_user(c)
    code, data = c.get_udar(name)
    if code != 404:
        raise api.APIValidationError(f'unexpected status code: {code}')
    # Check is another user cant rewrite not your item
    res = c.make_udar(name, generate_flag(), [], random.randint(1, 4), random_string(100))
    if res is None or 'details' not in res or res['details'] != 'No access':
        print(str(res)[:100])
        raise api.APIValidationError(f'Wrong answer on make udar')

    # Check is update available

    c.set_token(user['token'])
    res = c.make_udar(name, fake_flag, [another_user['id']], random.randint(1, 4), description)
    if res is not None:
        print(str(res)[:100])
        raise api.APIValidationError(f'Not valid result for make udar')
    data = validate_udar(c.get_udar(name), name, fake_flag, user['id'])
    if another_user['id'] not in data['trustedRusi']:
        raise api.APIValidationError(f'Lost trusted Rusi')

    items = c.get_user_udars()
    if len(items) != 1:
        raise api.APIValidationError(f'Wrong number of udars returned to user list {len(items)}')
    my_udar = items[0]
    if my_udar['name'] != name.upper():
        raise api.APIValidationError(f'Wrong name in list udars get')
    if my_udar['phonk'] != fake_flag:
        raise api.APIValidationError(f'Lost phonk in list udars')
    # Check is trustedRusi can access item
    c.set_token(another_user['token'])
    data = validate_udar(c.get_udar(name), name, fake_flag, user['id'])
    if another_user['id'] not in data['trustedRusi']:
        raise api.APIValidationError(f'Lost trusted Rusi')

    items = c.get_user_udars()
    if len(items) != 0:
        raise api.APIValidationError(f'Wrong number of udars returned to user list when nothing created {len(items)}')
    # Check can create with three trusted
    another_name = generate_name()
    another_flag = generate_flag()
    rand_user_id = random.randint(1, 99999)
    while rand_user_id == user['id'] or another_user['id'] == rand_user_id:
        rand_user_id = random.randint(1, 99999)
    res = c.make_udar(another_name, another_flag, [user['id'], another_user['id'], rand_user_id], random.randint(1, 4), random_string(100))
    if res is not None:
        print(str(res)[:100])
        raise api.APIValidationError(f'Not valid result for make udar')
    data = validate_udar(c.get_udar(another_name.lower()), another_name, another_flag, another_user['id'])
    users = data['trustedRusi']
    if another_user['id'] not in users or user['id'] not in users or rand_user_id not in users:
        raise api.APIValidationError(f'Lost trusted Rusi')
    return None


def do_check(request: gornilo.CheckRequest) -> gornilo.Verdict:
    with api.APIClient(request.hostname, PORT) as c:
        c.check_frontend()
        err = check_drink_water(c)
        if err is not None:
            return err
        check_valid_udar(c)
    return gornilo.Verdict.OK()


def do_put(request: gornilo.PutRequest) -> gornilo.Verdict:
    username = random_string(25)
    password = random_string(40)
    with api.APIClient(request.hostname, PORT) as c:
        c.register(username, password)
        token = c.login(username, password)
        name = generate_while_i()
        res = c.make_udar(name, request.flag, [], random.randint(1, 4), random_string(100))
        if res is not None:
            print(str(res)[:100])
            return gornilo.Verdict.MUMBLE('Wrong status code on put flag')
        if random.randint(0, 1) == 0:
            c.register(random_string(25), random_string(40))
        return gornilo.Verdict.OK_WITH_FLAG_ID(name, token)


def do_get(request: gornilo.GetRequest) -> gornilo.Verdict:
    if not request.flag_id or ':' in request.flag_id:
        print("BAD Flag ID: ", request.flag_id)
        return gornilo.Verdict.MUMBLE(f'Lost flag_id')
    with api.APIClient(request.hostname, PORT, request.flag_id) as c:
        code, data = c.get_udar(request.public_flag_id)
        if code != 200:
            return gornilo.Verdict.MUMBLE(f'Wrong status code on get flag: {code}')
        if 'phonk' not in data:
            return gornilo.Verdict.MUMBLE(f'No phonk field in data')
        if data['phonk'] != request.flag:
            return gornilo.Verdict.CORRUPT('invalid flag')
    return gornilo.Verdict.OK()

@checker.define_check
def check(request: gornilo.CheckRequest) -> gornilo.Verdict:
    return wrap_exceptions(do_check, request)


@checker.define_vuln('flag_id is name of udar')
class RUSIChecker(gornilo.VulnChecker):
    @staticmethod
    def put(request: gornilo.PutRequest) -> gornilo.Verdict:
        return wrap_exceptions(do_put, request)

    @staticmethod
    def get(request: gornilo.GetRequest) -> gornilo.Verdict:
        return wrap_exceptions(do_get, request)


if __name__ == '__main__':
    checker.run()
