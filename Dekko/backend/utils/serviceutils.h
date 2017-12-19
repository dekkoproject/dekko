#ifndef SERVICEUTILS_H
#define SERVICEUTILS_H

#include <QByteArray>
#include <QDataStream>
#include <qmailaccountkey.h>
#include <qmailaccountsortkey.h>
#include <qmailfolderkey.h>
#include <qmailfoldersortkey.h>
#include <qmailmessagekey.h>
#include <qmailmessagesortkey.h>

inline QMailMessageKey to_msg_key(const QByteArray &key) {
    QMailMessageKey mKey;
    QByteArray m = key;
    QDataStream keystream(&m, QIODevice::ReadWrite);
    mKey.deserialize(keystream);
    return mKey;
}

inline QByteArray msg_key_bytes(const QMailMessageKey &key) {
    QByteArray msgKey;
    QDataStream keystream(&msgKey, QIODevice::WriteOnly);

    key.serialize(keystream);
    return msgKey;
}

inline QMailMessageSortKey to_msg_sort_key(const QByteArray &key) {
    QMailMessageSortKey sKey;
    QByteArray s = key;
    QDataStream skeystream(&s, QIODevice::ReadWrite);
    sKey.deserialize(skeystream);
    return sKey;
}

inline QByteArray msg_sort_key_bytes(const QMailMessageSortKey &key) {
    QByteArray sortKey;
    QDataStream keystream(&sortKey, QIODevice::WriteOnly);

    key.serialize(keystream);

    return sortKey;
}

inline QMailMessageIdList from_dbus_msglist(const QList<quint64> &ids) {
    QMailMessageIdList list;
    foreach(const quint64 &id, ids) {
        QMailMessageId msgId(id);
        list << msgId;
    }
    return list;
}

inline QList<quint64> to_dbus_msglist(const QMailMessageIdList &ids) {
    QList<quint64> list;
    foreach(const QMailMessageId &id, ids) {
        list << id.toULongLong();
    }
    return list;
}

inline QMailFolderKey to_folder_key(const QByteArray &key) {
    QMailFolderKey mKey;
    QByteArray m = key;
    QDataStream keystream(&m, QIODevice::ReadWrite);
    mKey.deserialize(keystream);
    return mKey;
}

inline QByteArray folder_key_bytes(const QMailFolderKey &key) {
    QByteArray msgKey;
    QDataStream keystream(&msgKey, QIODevice::WriteOnly);
    key.serialize(keystream);
    return msgKey;
}

inline QMailFolderSortKey to_folder_sort_key(const QByteArray &key) {
    QMailFolderSortKey sKey;
    QByteArray s = key;
    QDataStream skeystream(&s, QIODevice::ReadWrite);
    sKey.deserialize(skeystream);
    return sKey;
}

inline QByteArray folder_sort_key_bytes(const QMailFolderSortKey &key) {
    QByteArray sortKey;
    QDataStream keystream(&sortKey, QIODevice::WriteOnly);
    key.serialize(keystream);
    return sortKey;
}

inline QMailFolderIdList from_dbus_folderlist(const QList<quint64> &ids) {
    QMailFolderIdList list;
    foreach(const quint64 &id, ids) {
        QMailFolderId folderId(id);
        list << folderId;
    }
    return list;
}

inline QList<quint64> to_dbus_folderlist(const QMailFolderIdList &ids) {
    QList<quint64> list;
    foreach(const QMailFolderId &id, ids) {
        list << id.toULongLong();
    }
    return list;
}

inline QMailAccountKey to_account_key(const QByteArray &key) {
    QMailAccountKey mKey;
    QByteArray m = key;
    QDataStream keystream(&m, QIODevice::ReadWrite);
    mKey.deserialize(keystream);
    return mKey;
}

inline QByteArray account_key_bytes(const QMailAccountKey &key) {
    QByteArray accountKey;
    QDataStream keystream(&accountKey, QIODevice::WriteOnly);
    key.serialize(keystream);
    return accountKey;
}

inline QMailAccountSortKey to_account_sort_key(const QByteArray &key) {
    QMailAccountSortKey sKey;
    QByteArray s = key;
    QDataStream skeystream(&s, QIODevice::ReadWrite);
    sKey.deserialize(skeystream);
    return sKey;
}

inline QByteArray account_sort_key_bytes(const QMailAccountSortKey &key) {
    QByteArray sortKey;
    QDataStream keystream(&sortKey, QIODevice::WriteOnly);
    key.serialize(keystream);
    return sortKey;
}

inline QMailAccountIdList from_dbus_accountlist(const QList<quint64> &ids) {
    QMailAccountIdList list;
    foreach(const quint64 &id, ids) {
        QMailAccountId aId(id);
        list << aId;
    }
    return list;
}

inline QList<quint64> to_dbus_accountlist(const QMailAccountIdList &ids) {
    QList<quint64> list;
    foreach(const QMailAccountId &id, ids) {
        list << id.toULongLong();
    }
    return list;
}



#endif // SERVICEUTILS_H
