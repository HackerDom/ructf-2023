#!/usr/bin/env python3

import gornilo

checker = gornilo.NewChecker()
PORT = 13337


@checker.define_check
def check(req: gornilo.CheckRequest) -> gornilo.Verdict:
    pass


@checker.define_vuln("flag_id is an id of test")
class RaceChecker(gornilo.VulnChecker):

    @staticmethod
    def put(req: gornilo.PutRequest) -> gornilo.Verdict:
        pass

    @staticmethod
    def get(req: gornilo.GetRequest) -> gornilo.Verdict:
        pass


def main():
    pass


if __name__ == '__main__':
    main()
