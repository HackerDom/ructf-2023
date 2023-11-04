#!/usr/bin/env python3.11
import sys
import math
import os
import random
import traceback
from perune_lib import PeruneClient, Gift, Prayer

from gornilo import CheckRequest, Verdict, PutRequest, GetRequest, VulnChecker, NewChecker

checker = NewChecker()
PORT = 18484

PeruneNeededScore = 1337.0
coefs = [0.1, 0.2, 1.337, 0.55, 0.77, 1.5]

def MakeRandomInvalidGift() -> Gift:
    password = os.urandom(32).hex().encode()
    type = random.randrange(0, len(coefs))
    count = random.randrange(1, math.floor(PeruneNeededScore/coefs[type]))
    return Gift(type, count, password)

def MakeRandomValidGift(use_cnt: int) -> Gift:
    password = os.urandom(32).hex().encode()
    type = random.randrange(0, len(coefs))
    count = random.randrange(math.floor(PeruneNeededScore/coefs[type]) * use_cnt, math.ceil(PeruneNeededScore/coefs[type]) * (use_cnt+1))
    return Gift(type, count, password)

def check_gifts(gift1: Gift, gift2: Gift) -> bool:
    if gift1.count != gift2.count:
        return False
    if gift1.type != gift2.type:
        return False
    if gift1.password != gift2.password:
        return False
    if gift1.get_id() != gift2.get_id():
        return False
    return True

class ErrorChecker:
    def __init__(self):
        self.verdict = Verdict.OK()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if exc_type in {Verdict}:
            self.verdict = exc_value
        elif exc_type in {TimeoutError, ConnectionError, ConnectionRefusedError}:
            self.verdict = Verdict.DOWN("Service is down")
        elif exc_type in {EOFError}:
            self.verdict = Verdict.MUMBLE("Got EOF in communication")
        
        # if exc_type:
        #     print(exc_type)
        #     print(exc_value.__dict__)
        #     traceback.print_tb(exc_traceback, file=sys.stdout)

        return True

@checker.define_check
def check_service(request: CheckRequest) -> Verdict:
    with ErrorChecker() as ec:
        cli = PeruneClient(request.hostname, PORT)
        cli.connection()
        cli.read_menu()
        gift = MakeRandomValidGift(10)
        gift_id = cli.make_gift(gift)
        gift.set_id(gift_id)
        prayer = cli.make_prayer(gift, os.urandom(16).hex().encode())

        if prayer is None:
            ec.verdict = Verdict.MUMBLE("Can't create prayer with gift")
            cli.exit()
            return ec.verdict
        
        viewed_gift = cli.view_gift(gift_id, gift.password)
        viewed_gift.set_id(gift_id)

        if not check_gifts(gift, viewed_gift):
            ec.verdict = Verdict.MUMBLE("Viewed gift is not the same as created gift!")
            cli.exit()
            return ec.verdict

        res = cli.call_perune(prayer.id)
        if prayer.text not in res:
            ec.verdict = Verdict.MUMBLE("Perune logic is changed!")
            cli.exit()
            return ec.verdict

        gift1 = MakeRandomInvalidGift()
        gift1_id = cli.make_gift(gift1)
        gift1.set_id(gift1_id)
        prayer1 = cli.make_prayer(gift1, os.urandom(16).hex().encode())

        if prayer1 is None:
            ec.verdict = Verdict.MUMBLE("Can't create prayer with gift")
            cli.exit()
            return ec.verdict

        viewed_gift1 = cli.view_gift(gift1_id, gift1.password)
        viewed_gift1.set_id(gift1_id)

        if not check_gifts(gift1, viewed_gift1):
            ec.verdict = Verdict.MUMBLE("Viewed gift is not the same as created gift!")
            cli.exit()
            return ec.verdict
        
        res = cli.call_perune(prayer1.id)
        if b"lizard" not in res:
            ec.verdict = Verdict.MUMBLE("Perune logic is changed!")
            cli.exit()
            return ec.verdict
        
        if prayer1.text in res:
            ec.verdict = Verdict.MUMBLE("Perune logic is changed!")
            cli.exit()
            return ec.verdict
        
        cli.exit()
        ec.verdict = Verdict.OK('OK!')
            
    return ec.verdict

@checker.define_vuln("flag_id is an username")
class CryptoChecker(VulnChecker):
    @staticmethod
    def put(request: PutRequest) -> Verdict:
        with ErrorChecker() as ec:
            cli = PeruneClient(request.hostname, PORT)
            cli.connection()
            cli.read_menu()
            gift = MakeRandomValidGift(10)
            gift_id = cli.make_gift(gift)
            gift.set_id(gift_id)
            prayer = cli.make_prayer(gift, request.flag.encode())
            
            if prayer is None:
                ec.verdict = Verdict.MUMBLE("Can't create prayer with gift")
                cli.exit()
                return ec.verdict
            
            res = cli.call_perune(prayer.id)
            
            if prayer.text in res:
                cli.exit()
                return Verdict.OK_WITH_FLAG_ID(gift_id.decode(), prayer.id.decode())
            else:
                ec.verdict = Verdict.MUMBLE("Perune logic is changed!")

            cli.exit()

        return ec.verdict

    @staticmethod
    def get(request: GetRequest) -> Verdict:
        with ErrorChecker() as ec:
            prayer_id = request.flag_id.encode()
            cli = PeruneClient(request.hostname, PORT)
            cli.connection()
            cli.read_menu()
            perune_verdict = cli.call_perune(prayer_id)

            if b"lizard" in perune_verdict:
                ec.verdict = Verdict.CORRUPT("Get lizard message instead of flag message!")
            elif request.flag.encode() in perune_verdict:
                ec.verdict = Verdict.OK()
            else:
                ec.verdict = Verdict.CORRUPT("Invalid flag!")
            
            cli.exit()
        return ec.verdict

if __name__ == '__main__':
    checker.run()
