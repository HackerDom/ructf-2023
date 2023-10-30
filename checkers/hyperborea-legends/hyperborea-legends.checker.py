#!/usr/bin/env python3.11

import base64
import traceback
import uuid

from gornilo import CheckRequest, Verdict, NewChecker, VulnChecker, GetRequest, PutRequest

import utils
from client.grpc_client import Client
from data_generator import DataGenerator

PORT = 42424

checker = NewChecker()
data_gen = DataGenerator()


@checker.define_check
def check_service(request: CheckRequest) -> Verdict:
    user, ancestor = data_gen.random_data()

    try:
        with Client(host=request.hostname, port=PORT) as client:
            print('>>Checking user registration...')
            resp = client.register_user(username=user.username, password=user.password, species_type=user.species_type)
            if not resp or not resp.success:
                utils.raise_wrong_data_exc('RegisterResponse', resp)

            print('>>Checking user login...')
            resp = client.login_user(username=user.username, password=user.password)
            if not resp or not resp.access_token:
                utils.raise_wrong_data_exc('LoginResponse', resp)
            token = resp.access_token

            print('>>Checking ancestor creating...')
            resp = client.create_ancestor(
                id=ancestor.id,
                name=ancestor.name,
                description=ancestor.description,
                ancestor_type=ancestor.species_type,
                burial_place=ancestor.burial_place,
                token=token,
            )
            if not resp or not resp.id:
                utils.raise_wrong_data_exc('CreateAncestorResponse', resp)
            ancestor_id = uuid.UUID(bytes=resp.id)

            print('>>Checking ancestor get...')
            resp = client.get_ancestor(id=ancestor_id, token=token)
            if not resp:
                utils.raise_wrong_data_exc('GetAncestorResponse', resp)
            if resp.id != ancestor_id.bytes or resp.name != ancestor.name or resp.description != ancestor.description or resp.ancestor_type != ancestor.species_type or resp.burial_place != ancestor.burial_place:
                utils.raise_wrong_data_exc('GetAncestorResponse', resp)

            if user.species_type == 'FilthyLizard':
                print('>>Checking add direct ancestor...')
                resp = client.add_direct_ancestor(ancestor_id=ancestor_id, token=token)
                if not resp or not resp.success:
                    utils.raise_wrong_data_exc('AddDirectAncestorResponse', resp)

            return Verdict.OK()

    except utils.VerdictException as e:
        print(e)
        return e.verdict
    except Exception as e:
        traceback.print_exc()
        return Verdict.MUMBLE('Something went wrong in CHECK')


@checker.define_vuln('flag_id is ancestor id in base64')
class HyperboreaLegendsChecker(VulnChecker):
    @staticmethod
    def get(request: GetRequest) -> Verdict:
        flag = request.flag

        try:
            if not request.flag_id or not request.public_flag_id:
                return Verdict.CORRUPT('Flag is missing!')
            token = request.flag_id
            ancestor_id = uuid.UUID(bytes=base64.b64decode(request.public_flag_id))
            with Client(host=request.hostname, port=PORT) as client:
                print(f'>>Get ancestor by public flag id {ancestor_id}')
                resp = client.get_ancestor(id=ancestor_id, token=token)
                if resp.burial_place == flag:
                    print(f'>>Successfully got flag "{flag}"')
                    return Verdict.OK()
                print("Ancestor burial place doesn't contains a correct flag")
                print(f'expected: "{flag}" but got "{resp.burial_place}')
                return Verdict.CORRUPT('Flag is missing!')
        except utils.VerdictException as e:
            print(e)
            return e.verdict
        except Exception as e:
            traceback.print_exc()
            return Verdict.MUMBLE("Couldn't get flag!")

    @staticmethod
    def put(request: PutRequest) -> Verdict:
        flag = request.flag
        user, ancestor = data_gen.random_data()

        try:
            with Client(host=request.hostname, port=PORT) as client:
                print(f'Register and login user: {user.username}')
                client.register_user(username=user.username, password=user.password, species_type='GreatRuss')
                token = client.login_user(username=user.username, password=user.password).access_token
                flag_id = token

                print(f'Create ancestor: {ancestor.name}')
                resp = client.create_ancestor(
                    id=ancestor.id,
                    name=ancestor.name,
                    description=ancestor.description,
                    ancestor_type='GreatRuss',
                    burial_place=flag,
                    token=token,
                )

                public_flag_id = base64.b64encode(resp.id).decode()
                return Verdict.OK_WITH_FLAG_ID(public_flag_id, flag_id)
        except utils.VerdictException as e:
            print(e)
            return e.verdict
        except Exception as e:
            traceback.print_exc()
            return Verdict.MUMBLE("Couldn't put flag!")


if __name__ == '__main__':
    checker.run()