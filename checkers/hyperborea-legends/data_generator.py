import dataclasses
import json
import random
import uuid

from transliterate import translit


@dataclasses.dataclass
class User:
    username: str
    password: str
    species_type: str


@dataclasses.dataclass
class Ancestor:
    id: uuid.UUID
    name: str
    description: str
    species_type: str
    burial_place: str


class DataGenerator:
    def __init__(self, data_filename: str = 'data.json'):
        self.data = None
        with open(data_filename) as f:
            self.data = json.load(f)
        self.first_part_male = self.data['first_part_male']
        self.first_part_female = self.data['first_part_female']
        self.second_part_male = self.data['second_part_male']
        self.second_part_female = self.data['second_part_female']

    def random_data(self) -> (User, Ancestor):
        if random.choice(['male', 'female']) == 'male':
            first_part, second_part = random.choice(self.first_part_male), random.choice(self.second_part_male)
            name = first_part['name'] + second_part['name']
            description = first_part['description'] + ', ' + second_part['description']
        else:
            first_part, second_part = random.choice(self.first_part_female), random.choice(self.second_part_female)
            name = first_part['name'] + second_part['name']
            description = first_part['description'] + ', ' + second_part['description']

        species_type = random.choice(['GreatRuss', 'FilthyLizard'])
        return (
            User(
                username=str(uuid.uuid4()),
                password=str(uuid.uuid4()),
                species_type=species_type,
            ),
            Ancestor(
                id=uuid.uuid4(),
                name=name if species_type == 'GreatRuss' else translit(name, language_code='ru', reversed=True),
                description=description if species_type == 'GreatRuss' else translit(description, language_code='ru',
                                                                                     reversed=True),
                species_type=species_type,
                burial_place=str(uuid.uuid4()),
            )
        )


if __name__ == '__main__':
    gen = DataGenerator()
    for i in range(10):
        print(gen.random_data()[1])
