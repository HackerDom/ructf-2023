from uuid import uuid4
from words import ADJECTIVES, ITEMS, ACTIONS, available_replacement, NAMES
from random import choice, randint, sample


def random_lowercase(string: str):
    lower_indexes = set(sample(range(len(string)), k=randint(1, len(string) // 2)))
    chrs_array = []
    for i, ch in enumerate(string):
        if i in lower_indexes:
            chrs_array.append(ch.lower())
        else:
            chrs_array.append(ch)
    return ''.join(chrs_array)


def do_replacement(string):
    for rep in sample(available_replacement, k=randint(0, 4)):
        from_ch = next(iter(rep))
        to_ch = rep[from_ch]
        string = string.replace(from_ch, to_ch)
    return string


def generate_name():
    adjective = choice(ADJECTIVES).upper()
    action = choice(ACTIONS).upper()
    item = choice(ITEMS).upper()
    from_name = choice(NAMES).upper()
    name = f"{adjective} {action} {item} FROM {from_name} SON OF {uuid4().hex[:15]} GOD"
    name = do_replacement(name)
    name = random_lowercase(name)
    return name


def generate_while_i():
    name = generate_name()
    while 'i' not in name and 'I' not in name:
        name = generate_name()
    return name


if __name__ == '__main__':
    print(generate_while_i())
