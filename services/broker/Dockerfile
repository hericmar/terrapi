FROM alpine

RUN apk add --no-cache mosquitto

COPY entrypoint.sh /opt/entrypoint.sh

CMD ["/opt/entrypoint.sh"]
