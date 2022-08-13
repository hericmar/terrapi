#!/bin/sh

# Alpine Linux specific!
SERVICE_PATH="/etc/init.d/terrapi"
BIN_PATH="/usr/bin/terrapi"
STATIC_ROOT="/usr/share/webapps/terrapi"

if [ -e "broker/terrapi" ]; then
	mkdir -p "/etc/terrapi"
	mkdir -p "${STATIC_ROOT}"
    
	cp broker/terra "${BIN_PATH}"

	cp "web/public" "${STATIC_ROOT}"

	cp "etc/init.d/terrapi" "${SERVICE_PATH}"
    
	echo "Instalation completed, configure NGINX manually and then run the service."
else
	echo "Webapp is not build, run 'make' first."
	exit 1
fi
