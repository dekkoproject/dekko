#!/bin/bash

set -e

export QT_SELECT=qt5

ROOT_DIR=$PWD
BACKEND_DIR=$ROOT_DIR/Dekko/backend
MAIL_DIR=$BACKEND_DIR/mail/service
ACCOUNT_DIR=$BACKEND_DIR/accounts/service

echo "Generating Mail Service"
cd $MAIL_DIR
qdbusxml2cpp $MAIL_DIR/dbus/service_worker.xml -N -a MailServiceAdaptor -c MailServiceAdaptor
qdbusxml2cpp $MAIL_DIR/dbus/service_worker.xml -N -p MailServiceInterface -c MailServiceInterface

echo "Generating Account Service"
cd $ACCOUNT_DIR
qdbusxml2cpp $ACCOUNT_DIR/dbus/account_service.xml -N -a AccountServiceAdaptor -c AccountServiceAdaptor
qdbusxml2cpp $ACCOUNT_DIR/dbus/account_service.xml -N -p AccountServiceInterface  -c AccountServiceInterface

echo "Done!!"
