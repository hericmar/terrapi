#!/bin/sh
# set -e

touch /etc/mosquitto/pwfile
mosquitto_passwd -b /etc/mosquitto/pwfile user "${BROKER_PASSWORD}"
mosquitto -c /etc/mosquitto/mosquitto.conf
