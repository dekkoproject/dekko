TEMPLATE = subdirs
SUBDIRS = messageserver

packagesExist(accounts-qt5) {
    SUBDIRS += accountscheck
}
