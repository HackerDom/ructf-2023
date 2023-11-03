import random
import base64
import string
from typing import Optional


def __gen_str(min_str_len: int, max_str_len: int) -> str:
    str_len = random.randint(min_str_len, max_str_len)
    str_bytes = random.randbytes(str_len)

    s = base64.b32encode(str_bytes).decode().lower().strip('=')
    return s[:str_len]


def __gen_word(min_word_len: int, max_word_len: int) -> str:
    word_len = random.randint(min_word_len, max_word_len)
    return ''.join(random.choice(string.ascii_lowercase) for _ in range(word_len))


def gen_login() -> str:
    return __gen_str(5, 10)


def gen_password() -> str:
    return __gen_str(10, 20)


def gen_test_description():
    sentences_amount = random.randint(1, 4)
    sentences = []
    for _ in range(sentences_amount):
        sentence = ' '.join(__gen_word(3, 8) for _ in range(sentences_amount))
        sentences.append(sentence.capitalize())

    description = '. '.join(sentences)
    return f'{description}.'


def gen_question(answers_num: int) -> dict:
    question = ' '.join(__gen_word(3, 8) for _ in range(random.randint(3, 10)))
    question = f'{question.capitalize()}?'

    answers = []
    for i in range(answers_num):
        answer = ' '.join(__gen_word(3, 8) for _ in range(random.randint(4, 8))).capitalize()
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
    name = ' '.join(__gen_word(3, 8) for _ in range(random.randint(5, 10))).capitalize()
    if flag is None:
        flag = gen_reward()

    return dict(
        name=name,
        questions=questions,
        description=description,
        reward=flag
    )
