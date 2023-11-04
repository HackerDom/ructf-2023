import random
import base64
import string
import pyjokes
from typing import Optional
import logins
import words


def __gen_str(min_str_len: int, max_str_len: int) -> str:
    str_len = random.randint(min_str_len, max_str_len)
    str_bytes = random.randbytes(str_len)

    s = base64.b32encode(str_bytes).decode().lower().strip('=')
    return s[:str_len]


def __gen_word() -> str:
    return random.choice(words.words)


def gen_login() -> str:
    login_pool = logins.female_logins if random.randint(0, 1) else logins.male_logins
    login = random.choice(login_pool)
    login = f'{login}{random.randint(1, 10000)}'

    return login


def gen_password() -> str:
    return __gen_str(10, 20)


def gen_test_description():
    return pyjokes.get_joke(language='en', category='all')


def gen_question(answers_num: int) -> dict:
    question = ' '.join(__gen_word() for _ in range(random.randint(3, 10)))
    question = f'{question.capitalize()}?'

    answers = []
    for i in range(answers_num):
        answer = ' '.join(__gen_word() for _ in range(random.randint(4, 8))).capitalize()
        answers.append(answer)

    correct_idx = random.randint(0, answers_num - 1)

    return dict(
        question=question,
        allowed_answers=answers,
        correct_idx=correct_idx,
    )


def gen_reward():
    return __gen_str(5, 10)


def gen_test(questions_num: int, answers_num: int, flag: Optional[str] = None) -> dict:
    description = gen_test_description()
    questions = [gen_question(answers_num) for _ in range(questions_num)]
    name = ' '.join(__gen_word() for _ in range(random.randint(5, 10))).capitalize()
    if flag is None:
        flag = gen_reward()

    return dict(
        name=name,
        questions=questions,
        description=description,
        reward=flag
    )


if __name__ == '__main__':
    print(gen_test_description())
