import random

import gornilo
from utils import wrap_exceptions, random_string
import api
PORT = 41114

checker = gornilo.NewChecker()


def do_check(request: gornilo.CheckRequest) -> gornilo.Verdict:
    # TODO:
    return gornilo.Verdict.OK()


def do_put(request: gornilo.PutRequest) -> gornilo.Verdict:
    username = random_string(13)
    password = random_string(40)
    with api.APIClient(request.hostname, PORT) as c:
        c.register(username, password)
        token = c.login(username, password)
        print(token)
        # TODO:
        name = random_string(50)
        res = c.make_udar(name, request.flag, [], random.randint(1, 4), random_string(100))
        if res is not None:
            print(res)
            return gornilo.Verdict.MUMBLE('Wrong status code on put flag')
        return gornilo.Verdict.OK_WITH_FLAG_ID(name, token)


def do_get(request: gornilo.GetRequest) -> gornilo.Verdict:
    print(request.flag_id)
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
