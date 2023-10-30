FROM python:3.10-alpine

WORKDIR /code

COPY . .
RUN pip install --no-cache-dir -r requirements.txt

RUN crontab cron

CMD ["crond", "-f"]