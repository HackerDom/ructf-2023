FROM joseluisq/mysql-client
WORKDIR /rusi_trainer/clean
COPY cleaner.sh .
RUN chmod +x cleaner.sh
ENTRYPOINT ["/rusi_trainer/clean/cleaner.sh"]