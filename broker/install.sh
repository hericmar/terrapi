#!/bin/sh

# Alpine Linux specific!
SERVICE_PATH="/etc/init.d/terrapi-broker"
BIN_PATH="/usr/bin/terrapi-broker"
STATIC_ROOT="/usr/share/webapps/terrapi-broker"
TERRAPI_BROKER_USER="terra"

if [ ! -f "web/dist/index.html" ]; then
  	echo "Webapp is not build, run 'make' first."
  	exit 1
fi

if [ ! -f "target/release/terrapi-broker" ]; then
    echo "terrapi-broker is not build, run 'make' first."
    exit 1
fi

if [ ! -e "/etc/terrapi-broker/config" ]; then
    mkdir -p "/etc/terrapi-broker"
    cp "etc/terrapi-broker/config" "/etc/terrapi-broker/config"
    chown -R root:$TERRAPI_BROKER_USER "/etc/terrapi-broker"
    chmod 550 "/etc/terrapi-broker"
    chmod 640 "/etc/terrapi-broker/config"
fi

cp -R "web/dist/." "${STATIC_ROOT}"
chown root:$TERRAPI_BROKER_USER "${STATIC_ROOT}/index.html"
chmod -R 775 "${STATIC_ROOT}"

cp "target/release/terrapi-broker" "${BIN_PATH}"
chown root:terra "${BIN_PATH}"

cp "etc/init.d/terrapi-broker" "${SERVICE_PATH}"
chmod 755 "${SERVICE_PATH}"

echo "Installation completed, configure NGINX manually and then run the service."
