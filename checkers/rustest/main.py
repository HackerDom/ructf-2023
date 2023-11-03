#!/usr/bin/env python3
import json
from typing import Optional

import gornilo
import requests

import generators

checker = gornilo.NewChecker()
PORT = 13337


class CheckerError:
    def __init__(self, verdict: gornilo.Verdict, message: str):
        self.verdict = verdict
        self.error_message = message

    def wrapped_by(self, root_err: str) -> 'CheckerError':
        self.error_message = f'{root_err}: {self.error_message}'
        return self


def make_request(
        url: str,
        method: str,
        payload: Optional[dict],
        headers: Optional[dict],
        query: Optional[dict],
) -> tuple[Optional[requests.Response], Optional[CheckerError]]:
    kwargs = {}
    if payload is not None:
        kwargs['json'] = payload
    if headers is not None:
        kwargs['headers'] = headers
    if query is not None:
        kwargs['params'] = query

    try:
        resp = requests.request(method, url, **kwargs)
        return resp, None
    except requests.exceptions.ConnectionError:
        return None, CheckerError(gornilo.Verdict.DOWN('cannot connect to the service'), 'cannot make request to service: connection error')
    except Exception as ex:
        print(ex)
        raise


def get_json_response(
        url: str,
        method: str,
        payload: Optional[dict] = None,
        headers: Optional[dict] = None,
        query: Optional[dict] = None,
) -> tuple[dict, Optional[requests.Response], Optional[CheckerError]]:
    resp, err = make_request(url, method, payload, headers, query)
    if err is not None:
        return {}, None, err.wrapped_by('cannot get json response: cannot make any request to server')

    try:
        return resp.json(), resp, None
    except requests.exceptions.JSONDecodeError:
        return {}, resp, CheckerError(
            gornilo.Verdict.MUMBLE('cannot parse json'),
            'cannot get json response: cannot parse json from response'
        )
    except Exception as ex:
        print(ex)
        raise


def register_with(url: str, login: str, password: str) -> tuple[str, Optional[CheckerError]]:
    payload = dict(
        login=login,
        password=password,
    )

    resp_data, _, err = get_json_response(f'{url}/api/register', method='post', payload=payload)
    if err is not None:
        return '', err.wrapped_by(f'cannot register user, {login=}')

    if 'token' not in resp_data:
        return '', CheckerError(
            gornilo.Verdict.MUMBLE('invalid json on register'),
            f'token field wasn`t present in register json response {resp_data=}'
        )

    return resp_data['json'], None


def login_as(url: str, login: str, password: str) -> tuple[str, Optional[CheckerError]]:
    payload = dict(
        login=login,
        password=password,
    )

    resp_data, _, err = get_json_response(f'{url}/api/login', method='post', payload=payload)
    if err is not None:
        return '', err.wrapped_by(f'user cannot login, {login=}')

    if 'token' not in resp_data:
        return '', CheckerError(
            gornilo.Verdict.MUMBLE('invalid json on login'),
            f'token field wasn`t present in login json response {resp_data=}'
        )

    return resp_data['json'], None


def get_test_state(url: str, token: str, test_id: str) -> tuple[dict, Optional[CheckerError]]:
    resp_data, _, err = get_json_response(
        f'{url}/api/rustest/{test_id}/solve',
        'get',
        headers={'token': f'Authorization: Bearer {token}'}
    )

    if err is not None:
        return {}, err.wrapped_by(f'cannot get test state of {test_id=}')

    return resp_data, None


def submit_answer_on_test_round(url: str, token: str, test_id: str, rnd: int, answer: int) -> tuple[dict, Optional[CheckerError]]:
    resp_data, _, err = get_json_response(
        f'{url}/api/rustest/{test_id}/submit',
        'post',
        payload={'round': rnd, 'answer': answer},
        headers={'Authorization': f'Bearer {token}'}
    )

    if err is not None:
        return {}, err.wrapped_by(f'cannot submit answer on test round {test_id}')

    return resp_data, None


def create_rustest(url: str, token: str, test: dict) -> tuple[dict, Optional[CheckerError]]:
    resp_data, _, err = get_json_response(
        f'{url}/api/rustest',
        'post',
        payload=test,
        headers={'Authorization': f'Bearer {token}'}
    )

    if err is not None:
        return {}, err.wrapped_by('cannot create rustest')

    if 'id' not in resp_data or 'owner' not in resp_data or 'reward' not in resp_data:
        return {}, CheckerError(
            gornilo.Verdict.MUMBLE('some fields of rustest are not stored on backend'),
            'id, owner, reward fields are required'
        )

    return resp_data, None


def get_page_of_tests(url: str, token: str, page: int = 0) -> tuple[dict, Optional[CheckerError]]:
    resp_data, _, err = get_json_response(
        f'{url}/api/rustests',
        'get',
        headers={'Authorization': f'Bearer {token}'},
        query={'page': page},
    )

    if err is not None:
        return {}, err.wrapped_by('cannot get page of tests')

    return resp_data, None


def solve_test_with_answers(url: str, token: str, test_id: str, correct_answers: [int]) -> tuple[dict, Optional[CheckerError]]:
    cur_state, err = get_test_state(url, token, test_id)
    if err is not None:
        return {}, err.wrapped_by('cannot solve test: cannot get initial state')

    if 'state' not in cur_state or cur_state['state'] != 'Running':
        return {}, CheckerError(
            gornilo.Verdict.MUMBLE(f'invalid state for test'),
            f'invalid state for test `{test_id}`'
        )

    for rnd_idx, ans in enumerate(correct_answers):
        resp, err = submit_answer_on_test_round(url, token, test_id, rnd_idx, ans)
        if err is not None:
            return {}, err.wrapped_by(f'cannot submit answer on test round {test_id=}, {rnd_idx=}')

        if rnd_idx != len(correct_answers) - 1:
            # if it is not last question
            if 'state' not in resp or resp['state'] != 'Running':
                return {}, CheckerError(
                    gornilo.Verdict.MUMBLE('cannot solve test, invalid state got on round submit'),
                    f'invalid state got on response in `{test_id}` in round `{rnd_idx}` ({resp=})'
                )

        cur_state = resp

    if 'state' not in cur_state or cur_state['state'] != 'Final':
        return {}, CheckerError(
            gornilo.Verdict.MUMBLE('not final state after solving all questions'),
            f'state is not final after solving all questions, {cur_state=}'
        )

    # state is final
    if 'final_state' not in cur_state:
        return {}, CheckerError(
            gornilo.Verdict.MUMBLE('state is final, but result is not in response'),
            f'cannot get result of test in final state ({cur_state=})',
        )

    final_state = cur_state['final_state']
    if 'result' not in final_state:
        return {}, CheckerError(
            gornilo.Verdict.MUMBLE('final result doesn`t contain result (win or lose)'),
            f'final result doesn`t contain result (win or lose): {final_state=}'
        )

    # we got final state, return to caller
    return final_state, None


def register_random(url: str) -> tuple[str, Optional[CheckerError]]:
    login = generators.gen_login()
    password = generators.gen_password()
    return register_with(url, login, password)


def build_url(req) -> str:
    return f'http://{req.hostname}:{PORT}'


@checker.define_check
def check(req: gornilo.CheckRequest) -> gornilo.Verdict:
    pass


@checker.define_vuln('flag_id is an id of test')
class RaceChecker(gornilo.VulnChecker):
    @staticmethod
    def put(req: gornilo.PutRequest) -> gornilo.Verdict:
        url = build_url(req)

        # we must register a user
        token, err = register_random(url)
        if err is not None:
            print(f'put error: {err.error_message}')
            return err.verdict

        # create a test with a reward.
        # WE CAN MAKE TESTS EASIER TO HACK DURING THE COMPETITION BY VARYING QUESTIONS NUM AND ANSWERS PER QUESTION NUM
        test = generators.gen_test(questions_num=10, answers_num=4, flag=req.flag)
        test, err = create_rustest(url, token, test)
        if err is not None:
            print(f'put error: {err.error_message}')
            return err.verdict

        test_id = test['id']
        # we need to dump all the test, because we need to solve test with correct answers
        return gornilo.Verdict.OK_WITH_FLAG_ID(test_id, json.dumps(test))

    @staticmethod
    def get(req: gornilo.GetRequest) -> gornilo.Verdict:
        url = build_url(req)

        # we must register a user
        token, err = register_random(url)
        if err is not None:
            print(f'get error: {err.error_message}')
            return err.verdict

        test = json.loads(req.flag_id)
        test_id = req.public_flag_id
        flag = test['reward']
        correct_answers = [question['correct_idx'] for question in test['questions']]

        final_state, err = solve_test_with_answers(url, token, test_id, correct_answers)
        if err is not None:
            print(f'get error: {err.error_message}')
            return err.verdict

        if final_state['result'] != 'Win' or 'reward' not in final_state:
            print('wrong state {final_state}')
            return gornilo.Verdict.CORRUPT('cannot win the victorine to get reward')

        svc_reward = final_state['reward']
        if svc_reward != flag:
            print(f'wrong flag (svc flag) != (checker flag) {svc_reward} != {flag}')
            return gornilo.Verdict.CORRUPT('wrong flag')


def main():
    pass


if __name__ == '__main__':
    main()
