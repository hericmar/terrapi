#!/bin/sh

ERR_NO_BIN=1
ERR_NO_ROOT=2

if [ -e "target/bin/terrapi" ]; then
  if [ "$(id -u)" -ne 0 ]; then
    echo "This script must be run as root."
    exit $ERR_NO_ROOT
  fi

  cp target/bin/terrapi /usr/bin/terrapi

  mkdir -p /etc/terrapi
  if [ -e /etc/terrapi/config ]; then
    cp etc/terrapi/config /etc/terrapi/config.new
  else
    echo "Creating default config."
    cp etc/terrapi/config /etc/terrapi/config
  fi

  echo "Copying systemd terrapi.service configuration file."
  cp etc/terrapi.service /etc/systemd/system/terrapi.service

  echo "Enabling daemon."
  systemctl enable terrapi

  echo "Run \"systemctl start terrapi\" to start daemon."
else
  echo "Source is not compiled."
  exit $ERR_NO_BIN
fi