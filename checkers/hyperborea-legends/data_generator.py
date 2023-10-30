import dataclasses
import json
import random
import uuid


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

    def random_data(self) -> (User, Ancestor):
        elem = random.choice(self.data)

        species_type = random.choice(['GreatRuss', 'FilthyLizard'])
        return (
            User(
                username=str(uuid.uuid4()),
                password=str(uuid.uuid4()),
                species_type=species_type,
            ),
            Ancestor(
                id=uuid.uuid4(),
                name=elem['ancestor_name'],
                description=elem['ancestor_description'],
                species_type=species_type,
                burial_place=str(uuid.uuid4()),
            )
        )
