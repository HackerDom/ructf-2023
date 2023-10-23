import threading
import uuid

import requests as requests

URL = 'http://localhost:3000'
RUSTEST_ID = '12c09e1b-61f1-4b45-9ead-e8d150969256'


def get_question(test_id: str, user_id: str):
    return requests.get(
        f'{URL}/rustest/{test_id}',
        headers={'Authorization': f'Bearer {user_id}'},
    ).json()


def submit_answer(test_id: str, user_id: str, answer: int) -> dict:
    print(f'submitting answer for {test_id} with answer {answer} by user {user_id}')

    resp = requests.post(
        f'{URL}/rustest/{test_id}/submit',
        headers={'Authorization': f'Bearer {user_id}'},
        json={'round': 0, 'answer': answer}
    )

    try:
        data = resp.json()
        print(f'{{ {answer} }} -> {data}')
        return data
    except requests.exceptions.JSONDecodeError:
        return {'status_code': resp.status_code}


def test() -> str:
    user_id = str(uuid.uuid4())

    threads = []
    for i in range(2):
        threads.append(threading.Thread(
            target=(lambda idx: lambda: submit_answer(RUSTEST_ID, user_id, idx))(i)
        ))

    for th in threads:
        th.start()

    for th in threads:
        th.join()

    final = get_question(RUSTEST_ID, user_id)
    return final['final_state']['result']


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


def main():
    token = login_user('lololozhkin', 'penis228')
    for i in range(200):
        rustest = create_rustest(
            token,
            dict(
                name=f'Velikiy test {i + 1}',
                description=f'Velikiy test {i + 1} pozvolayet proverit russ li polzovatel ili zhe yascher',
                reward=f'Course{{velikaya_nagrada_{i + 1}}}',
                questions=[],
            )
        )

        print(rustest)
    #
    # for i in range(2, 40):
    #     token = register_user(f'user{i}', f'password_{i}')
    #     print(token)
    # win_cnt = 0
    # lose_cnt = 0
    # for i in range(100):
    #     res = test()
    #     if res == 'Win':
    #         win_cnt += 1
    #     else:
    #         lose_cnt += 1

    # print(f'win: {win_cnt}')
    # print(f'lose: {lose_cnt}')


if __name__ == '__main__':
    main()
