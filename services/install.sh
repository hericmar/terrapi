#!/bin/sh

# Alpine Linux specific!
SERVICE_PATH="/etc/init.d/terra"
BIN_PATH="/usr/bin/terra"
STATIC_ROOT="/usr/share/webapps/terra"

if [ -e "broker/terra" ]; then
    cp broker/terra "${BIN_PATH}"

    mkdir -p "${STATIC_ROOT}"
    cp "web/public" "${STATIC_ROOT}"
    
    echo "Instalation completed, configure NGINX manually and then run the service."
else
    echo "Webapp is not build, run 'make' first."
    exit 1
fi
