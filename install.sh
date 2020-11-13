#!/bin/sh

ERR_NO_BIN=1
ERR_NO_ROOT=2

if [ -e bin/TerraPi ]
then
    if [ "$(id -u)" -ne 0 ]
    then
        echo "This script must be run as root."
        exit $ERR_NO_ROOT
    fi

    echo "Copying configuration to new directory /etc/terrapi."
    if [ -e etc/terrapi ]; then
        echo "directory already /etc/terrapi exists."
    else
        cp -r conf_test /etc/terrapi
    fi

    echo "Copying systemd terrapi.service configuration file."
    cp terrapi.service /etc/systemd/system/terrapi.service

    echo "Enabling daemon."
    systemctl enable terrapi

    systemctl start terrapi
    echo "Service started!"

else
    echo "Source is not compiled. Run 'make' please."
    exit $ERR_NO_BIN
fi
