TEMPLATE = lib 
TARGET = passwordplugin
CONFIG += plugin
QT = core

equals(QT_MAJOR_VERSION, 4) {
    target.path += $$QMF_INSTALL_ROOT/lib/qmf/plugins/ssoauth
    LIBS += -lqmfclient
}
equals(QT_MAJOR_VERSION, 5) {
    target.path += $$QMF_INSTALL_ROOT/lib/qmf/plugins5/ssoauth
    LIBS += -lqmfclient5
}

DEPENDPATH += .

INCLUDEPATH += . ../../../libraries/qmfclient \
               ../../../libraries/qmfclient/support

LIBS += -L../../../libraries/qmfclient/build
macx:LIBS += -F../../../libraries/qmfclient/build

HEADERS += passwordplugin.h

SOURCES += passwordplugin.cpp

include(../../../../common.pri)
