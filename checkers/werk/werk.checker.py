#!/usr/bin/env python3.11
import json
import sys
import traceback

import requests
from gornilo import CheckRequest, Verdict, PutRequest, GetRequest, VulnChecker, NewChecker

import api
import generators

checker = NewChecker()
PORT = 7654


DOWN_EXCEPTIONS = {
    requests.exceptions.ConnectTimeout,
    requests.exceptions.ConnectionError,
    requests.exceptions.ReadTimeout
}
MUMBLE_EXCEPTIONS = {
    requests.exceptions.HTTPError,
    requests.exceptions.JSONDecodeError,
}

KNOWN_EXCEPTIONS = DOWN_EXCEPTIONS | MUMBLE_EXCEPTIONS


class ErrorChecker:
    def __init__(self):
        self.verdict = Verdict.OK()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if exc_type in DOWN_EXCEPTIONS:
            self.verdict = Verdict.DOWN("Service is down")

        elif exc_type in MUMBLE_EXCEPTIONS:
            self.verdict = Verdict.MUMBLE("Incorrect http code or content")

        if exc_type:
            print(exc_type)
            print(exc_value.__dict__)
            traceback.print_tb(exc_traceback, file=sys.stdout)
            if exc_type not in KNOWN_EXCEPTIONS:
                raise exc_value

        return True


@checker.define_check
def check_service(request: CheckRequest) -> Verdict:
    return Verdict.OK()


@checker.define_vuln("Flag ID is the username, flag is the content of the payment info")
class TokenoucerChecker(VulnChecker):
    @staticmethod
    def put(request: PutRequest) -> Verdict:
        with ErrorChecker() as ec:
            username = generators.gen_username()

            token, lucky_numbers = api.register(request.hostname, PORT, username, request.flag)

            flag_id = json.dumps({
                'username': username,
                'token': token,
                'lucky_numbers': lucky_numbers,
            })
            ec.verdict = Verdict.OK_WITH_FLAG_ID(username, flag_id)
        return ec.verdict

    @staticmethod
    def get(request: GetRequest) -> Verdict:
        with ErrorChecker() as ec:
            flag_id_json = json.loads(request.flag_id)
            username = flag_id_json["username"]
            token = flag_id_json["token"]
            lucky_numbers = flag_id_json["lucky_numbers"]

            try:
                payment_info_res, lucky_numbers_res = api.get_info(request.hostname, PORT, username, token)
                if payment_info_res != request.flag:
                    print(f"corrupt flag: {payment_info_res} != {request.flag}")
                    ec.verdict = Verdict.CORRUPT("corrupt flag")
                    return ec.verdict

                if lucky_numbers != lucky_numbers_res:
                    ec.verdict = Verdict.MUMBLE("incorrect lucky numbers")
                    return ec.verdict

            except requests.exceptions.HTTPError as e:
                print(f"corrupt flag, incorrect http code: {e.response.__dict__}")
                ec.verdict = Verdict.CORRUPT("corrupt flag, incorrect http code")
                return ec.verdict

        return ec.verdict


if __name__ == '__main__':
    checker.run()
