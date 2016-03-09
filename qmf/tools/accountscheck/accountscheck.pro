
TEMPLATE = app
TARGET = qmf-accountscheck
QT -= gui

target.path += /usr/bin

CONFIG += link_pkgconfig
PKGCONFIG += accounts-qt5
LIBS += -lqmfclient5
LIBS += -L../../libraries/qmfclient/build

SOURCES += accountscheck.cpp

# Target to install systemd service file
systemd.files = ../systemd/messageserver5.service \
		../systemd/messageserver5-accounts-check.service
systemd.path= /usr/lib/systemd/user/

INSTALLS+= target systemd
