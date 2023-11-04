import random
import string

ALPHA = string.ascii_letters + string.digits


def gen_string(min_size=20, max_size=20, alpha=ALPHA):
    return ''.join(random.choice(alpha) for _ in range(random.randint(min_size, max_size)))


def gen_username():
    return gen_string()
