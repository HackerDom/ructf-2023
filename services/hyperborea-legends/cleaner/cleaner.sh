#!/bin/sh
while true; do
	date -uR
	psql "host=postgres dbname=postgres user=postgres password=postgres" -c "DELETE FROM users WHERE created_at < NOW() - INTERVAL '30 MINUTE';"
	sleep $((15 * 60))
done
