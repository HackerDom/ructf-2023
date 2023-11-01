FROM python:3.10-alpine

WORKDIR /code

COPY . .

RUN chmod +x ./entrypoint.sh

RUN pip install --no-cache-dir -r requirements.txt

ENTRYPOINT ["./entrypoint.sh"]
