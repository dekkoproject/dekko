TEMPLATE = subdirs
SUBDIRS = \
    messageservices/imap \
    messageservices/pop \
    messageservices/smtp \
    messageservices/qmfsettings \
    contentmanagers/qmfstoragemanager

packagesExist(accounts-qt5) {
    SUBDIRS += ssoauth/password
}

