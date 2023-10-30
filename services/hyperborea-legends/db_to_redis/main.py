import psycopg2
import redis
import os
import json
import uuid


class Transfer:
    def __init__(self, db_user: str, db_password: str, db_host: str, redis_host: str) -> None:
        self.db = psycopg2.connect(user=db_user, password=db_password, host=db_host, port='5432')
        self.redis = redis.Redis(host=redis_host, decode_responses=True)

    def transfer_data(self) -> None:
        cur = self.db.cursor()
        cur.execute('SELECT id, username, password, species, directAncestors FROM users')
        for record in cur.fetchall():
            print(record[4])
            val = {
                'Username': record[1],
                'Password': record[2],
                'Species': record[3],
                'DirectAncestors': [str(uuid.UUID(bytes=bytes(a))) for a in record[4]]
            }
            print(json.dumps(val))
            self.redis.set(f"user:{record[0]}", json.dumps(val))


if __name__ == '__main__':
    print("Transfer some data from DB to Redis")

    transfer = Transfer(
        db_user=os.getenv('POSTGRES_USER'),
        db_password=os.getenv('POSTGRES_PASSWORD'),
        db_host=os.getenv('POSTGRES_HOST'),
        redis_host=os.getenv('REDIS_HOST')
    )
    transfer.transfer_data()
