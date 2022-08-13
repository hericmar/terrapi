#!/bin/sh

# Alpine Linux specific!
SERVICE_PATH="/etc/init.d/terrapi"
BIN_PATH="/usr/bin/terrapi"
STATIC_ROOT="/usr/share/webapps/terrapi"

if [ -e "broker/terrapi" ]; then
	mkdir -p "/etc/terrapi"
	mkdir -p "${STATIC_ROOT}"

	if [ ! -e "/etc/terrapi/env" ]; then
		cp "etc/terrapi/env.example" "/etc/terrapi/env"
		chown -R root:terra "/etc/terrapi"
		chmod 550 "/etc/terrapi"
		chmod 440 "/etc/terrapi/env"
	fi
    
	cp "broker/terrapi" "${BIN_PATH}"
	chown root:terra "${BIN_PATH}"
 
	cp -R "web/public/." "${STATIC_ROOT}"
	chmod -R 555 "${STATIC_ROOT}"

	cp "etc/init.d/terrapi" "${SERVICE_PATH}"
	chmod 755 "${SERVICE_PATH}"

	echo "Instalation completed, configure NGINX manually and then run the service."
else
	echo "Webapp is not build, run 'make' first."
	exit 1
fi
