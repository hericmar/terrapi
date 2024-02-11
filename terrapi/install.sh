#!/bin/sh -e

if [ ! -e "target/bin/terrapi" ]; then
  echo "Source is not compiled."
  exit 1
fi

if [ "$(id -u)" -ne 0 ]; then
  echo "This script must be run as root."
  exit 1
fi

cp target/bin/terrapi /usr/bin/terrapi

mkdir -p /etc/terrapi
if [ -e /etc/terrapi/config.toml ]; then
  cp etc/terrapi/config.toml /etc/terrapi/config.toml.new
else
  echo "Creating default config."
  cp etc/terrapi/config.toml /etc/terrapi/config.toml
fi
chmod -R 750 /etc/terrapi

echo "Copying systemd terrapi.service configuration file."
cp -f etc/terrapi.service /etc/systemd/system/terrapi.service

echo "Enabling daemon."
systemctl enable terrapi

echo "Run \"systemctl start terrapi\" to start daemon."
