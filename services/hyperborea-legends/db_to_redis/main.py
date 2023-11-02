import psycopg2
import redis
import os
import json
import uuid

CACHE_EXPIRE_SECONDS = 60


class Transfer:
    def __init__(self, db_user: str, db_password: str, db_host: str, redis_host: str) -> None:
        self.db = psycopg2.connect(user=db_user, password=db_password, host=db_host, port='5432')
        self.redis = redis.Redis(host=redis_host, decode_responses=True)

    def transfer_data(self) -> None:
        cur = self.db.cursor()
        cur.execute('SELECT u.id, u.username, u.password, u.species, u.directAncestors, count(a.id) as ancestors_count FROM users u LEFT JOIN ancestors a ON a.ownerId = u.id GROUP BY u.id')
        for record in cur.fetchall():
            val = {
                'Id': record[0],
                'Username': record[1],
                'Password': record[2],
                'Species': record[3],
                'DirectAncestors': [str(uuid.UUID(bytes=bytes(a))) for a in record[4]],
                'AncestorsCount': record[5],
            }
            self.redis.set(f"user:{record[0]}", json.dumps(val))
            self.redis.expire(f"user:{record[0]}", CACHE_EXPIRE_SECONDS)


if __name__ == '__main__':
    print("Transfer some data from DB to Redis")

    transfer = Transfer(
        db_user=os.getenv('POSTGRES_USER'),
        db_password=os.getenv('POSTGRES_PASSWORD'),
        db_host=os.getenv('POSTGRES_HOST'),
        redis_host=os.getenv('REDIS_HOST')
    )
    transfer.transfer_data()
