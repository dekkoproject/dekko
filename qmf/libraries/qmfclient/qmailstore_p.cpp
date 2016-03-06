/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmailstore_p.h"
#include "locks_p.h"
#include "qmailcontentmanager.h"
#include "qmailmessageremovalrecord.h"
#include "qmailtimestamp.h"
#include "qmailnamespace.h"
#include "qmaillog.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QTextCodec>

#ifdef USE_ACCOUNTS_QT
#include <QSharedPointer>
#endif

#if defined(Q_OS_LINUX)
#include <malloc.h>
#endif

#define Q_USE_SQLITE

// When using GCC 4.1.1 on ARM, TR1 functional cannot be included when RTTI
// is disabled, since it automatically instantiates some code using typeid().
//#include <tr1/functional>
//using std::tr1::bind;
//using std::tr1::cref;
#include "bind_p.h"

using nonstd::tr1::bind;
using nonstd::tr1::cref;

#define MAKE_APPEND_UNIQUE(Type) \
    static inline void APPEND_UNIQUE(Type ## List *all_messageIds, const Type &id) {\
        if (!all_messageIds->contains(id))\
            (*all_messageIds) << id;\
    }\
    static inline void APPEND_UNIQUE(Type ## List *all_messageIds, Type ## List *messageIds) {\
        foreach (const Type &id, *messageIds) {\
            APPEND_UNIQUE(all_messageIds, id);\
        }\
    }

MAKE_APPEND_UNIQUE(QMailMessageId)
MAKE_APPEND_UNIQUE(QMailFolderId)
MAKE_APPEND_UNIQUE(QMailThreadId)
MAKE_APPEND_UNIQUE(QMailAccountId)

#undef MAKE_APPEND_UNIQUE

class QMailStorePrivate::Key
{
    enum Type {
        Account = 0,
        AccountSort,
        Folder,
        FolderSort,
        Message,
        MessageSort,
        Text,
        Thread,
        ThreadSort
    };

    Type m_type;
    const void* m_key;
    const QString* m_alias;
    const QString* m_field;

    static QString s_null;

    template<typename NonKeyType>
    bool isType(NonKeyType) const { return false; }

    bool isType(QMailAccountKey*) const { return (m_type == Account); }
    bool isType(QMailAccountSortKey*) const { return (m_type == AccountSort); }
    bool isType(QMailFolderKey*) const { return (m_type == Folder); }
    bool isType(QMailFolderSortKey*) const { return (m_type == FolderSort); }
    bool isType(QMailThreadKey*) const { return (m_type == Thread); }
    bool isType(QMailThreadSortKey*) const { return (m_type == ThreadSort); }
    bool isType(QMailMessageKey*) const { return (m_type == Message); }
    bool isType(QMailMessageSortKey*) const { return (m_type == MessageSort); }
    bool isType(QString*) const { return (m_type == Text); }

    const QMailAccountKey &key(QMailAccountKey*) const { return *reinterpret_cast<const QMailAccountKey*>(m_key); }
    const QMailAccountSortKey &key(QMailAccountSortKey*) const { return *reinterpret_cast<const QMailAccountSortKey*>(m_key); }
    const QMailFolderKey &key(QMailFolderKey*) const { return *reinterpret_cast<const QMailFolderKey*>(m_key); }
    const QMailFolderSortKey &key(QMailFolderSortKey*) const { return *reinterpret_cast<const QMailFolderSortKey*>(m_key); }
    const QMailMessageKey &key(QMailMessageKey*) const { return *reinterpret_cast<const QMailMessageKey*>(m_key); }
    const QMailMessageSortKey &key(QMailMessageSortKey*) const { return *reinterpret_cast<const QMailMessageSortKey*>(m_key); }
    const QMailThreadKey &key(QMailThreadKey*) const { return *reinterpret_cast<const QMailThreadKey*>(m_key); }
    const QMailThreadSortKey &key(QMailThreadSortKey*) const { return *reinterpret_cast<const QMailThreadSortKey*>(m_key); }
    const QString &key(QString*) const { return *m_alias; }

public:
    explicit Key(const QMailAccountKey &key, const QString &alias = QString()) : m_type(Account), m_key(&key), m_alias(&alias), m_field(0) {}
    Key(const QString &field, const QMailAccountKey &key, const QString &alias = QString()) : m_type(Account), m_key(&key), m_alias(&alias), m_field(&field) {}
    explicit Key(const QMailAccountSortKey &key, const QString &alias = QString()) : m_type(AccountSort), m_key(&key), m_alias(&alias), m_field(0) {}

    explicit Key(const QMailFolderKey &key, const QString &alias = QString()) : m_type(Folder), m_key(&key), m_alias(&alias), m_field(0) {}
    Key(const QString &field, const QMailFolderKey &key, const QString &alias = QString()) : m_type(Folder), m_key(&key), m_alias(&alias), m_field(&field) {}
    explicit Key(const QMailFolderSortKey &key, const QString &alias = QString()) : m_type(FolderSort), m_key(&key), m_alias(&alias), m_field(0) {}

    explicit Key(const QMailThreadKey &key, const QString &alias = QString()) : m_type(Thread), m_key(&key), m_alias(&alias), m_field(0) {}
    Key(const QString &field, const QMailThreadKey &key, const QString &alias = QString()) : m_type(Thread), m_key(&key), m_alias(&alias), m_field(&field) {}
    explicit Key(const QMailThreadSortKey &key, const QString &alias = QString()) : m_type(ThreadSort), m_key(&key), m_alias(&alias), m_field(0) {}


    explicit Key(const QMailMessageKey &key, const QString &alias = QString()) : m_type(Message), m_key(&key), m_alias(&alias), m_field(0) {}
    Key(const QString &field, const QMailMessageKey &key, const QString &alias = QString()) : m_type(Message), m_key(&key), m_alias(&alias), m_field(&field) {}
    explicit Key(const QMailMessageSortKey &key, const QString &alias = QString()) : m_type(MessageSort), m_key(&key), m_alias(&alias), m_field(0) {}

    explicit Key(const QString &text) : m_type(Text), m_key(0), m_alias(&text), m_field(0) {}

    template<typename KeyType>
    bool isType() const { return isType(reinterpret_cast<KeyType*>(0)); }

    template<typename KeyType>
    const KeyType &key() const { return key(reinterpret_cast<KeyType*>(0)); }

    const QString &alias() const { return *m_alias; }

    const QString &field() const { return (m_field ? *m_field : s_null); }
};

QString QMailStorePrivate::Key::s_null;


namespace { // none of this code is externally visible:

//using namespace QMailDataComparator;
using namespace QMailKey;

// We allow queries to be specified by supplying a list of message IDs against
// which candidates will be matched; this list can become too large to be
// expressed directly in SQL.  Instead, we will build a temporary table to
// match against when required...
// The most IDs we can include in a query is currently 999; set an upper limit
// below this to allow for other variables in the same query, bearing in mind
// that there may be more than one clause containing this number of IDs in the
// same query...
const int IdLookupThreshold = 256;

// Note on retry logic - it appears that SQLite3 will return a SQLITE_BUSY error (5)
// whenever there is contention on file locks or mutexes, and that these occurrences
// are not handled by the handler installed by either sqlite3_busy_timeout or
// sqlite3_busy_handler.  Furthermore, the comments for sqlite3_step state that if
// the SQLITE_BUSY error is returned whilst in a transaction, the transaction should
// be rolled back.  Therefore, it appears that we must handle this error by retrying
// at the QMailStore level, since this is the level where we perform transactions.
const int Sqlite3BusyErrorNumber = 5;

const int Sqlite3ConstraintErrorNumber = 19;

const uint pid = static_cast<uint>(QCoreApplication::applicationPid() & 0xffffffff);

// Helper class for automatic unlocking
template<typename Mutex>
class Guard
{
    Mutex &mutex;
    bool locked;

public:

    Guard(Mutex& m)
        : mutex(m),
          locked(false)
    {
    }

    ~Guard()
    {
        unlock();
    }

    void lock()
    {
        if (!locked) {
            mutex.lock();
            locked = true;
        }
    }

    void unlock()
    {
        if (locked) {
            mutex.unlock();
            locked = false; 
        }
    }
};

typedef Guard<ProcessMutex> MutexGuard;


QString escape(const QString &original, const QChar &escapee, const QChar &escaper = '\\')
{
    QString result(original);
    return result.replace(escapee, QString(escaper) + escapee);
}

QString unescape(const QString &original, const QChar &escapee, const QChar &escaper = '\\')
{
    QString result(original);
    return result.replace(QString(escaper) + escapee, escapee);
}

QString contentUri(const QString &scheme, const QString &identifier)
{
    if (scheme.isEmpty())
        return QString();

    // Formulate a URI from the content scheme and identifier
    return escape(scheme, ':') + ':' + escape(identifier, ':');
}

QString contentUri(const QMailMessageMetaData &message)
{
    return contentUri(message.contentScheme(), message.contentIdentifier());
}

QPair<QString, QString> extractUriElements(const QString &uri)
{
    int index = uri.indexOf(':');
    while ((index != -1) && (uri.at(index - 1) == '\\'))
        index = uri.indexOf(':', index + 1);

    return qMakePair(unescape(uri.mid(0, index), ':'), unescape(uri.mid(index + 1), ':'));
}

QString identifierValue(const QString &str)
{
    QStringList identifiers(QMail::messageIdentifiers(str));
    if (!identifiers.isEmpty()) {
        return identifiers.first();
    }

    return QString();
}

QStringList identifierValues(const QString &str)
{
    return QMail::messageIdentifiers(str);
}

template<typename ValueContainer>
class MessageValueExtractor;

// Class to extract QMailMessageMetaData properties to QVariant form
template<>
class MessageValueExtractor<QMailMessageMetaData>
{
    const QMailMessageMetaData &_data;
    
public:
    MessageValueExtractor(const QMailMessageMetaData &d) : _data(d) {}

    QVariant id() const { return _data.id().toULongLong(); }

    QVariant messageType() const { return static_cast<int>(_data.messageType()); }

    QVariant parentFolderId() const { return _data.parentFolderId().toULongLong(); }

    QVariant from() const { return _data.from().toString(); }

    QVariant to() const { return QMailAddress::toStringList(_data.recipients()).join(","); }

    QVariant copyServerUid() const { return _data.copyServerUid(); }

    QVariant restoreFolderId() const { return _data.restoreFolderId().toULongLong(); }

    QVariant listId() const { return _data.listId(); }

    QVariant rfcId() const { return _data.rfcId(); }

    QVariant subject() const { return _data.subject(); }

    QVariant date() const { return _data.date().toUTC(); }

    QVariant receivedDate() const { return _data.receivedDate().toUTC(); }

    // Don't record the value of the UnloadedData flag:
    QVariant status() const { return (_data.status() & ~QMailMessage::UnloadedData); }

    QVariant parentAccountId() const { return _data.parentAccountId().toULongLong(); }

    QVariant serverUid() const { return _data.serverUid(); }

    QVariant size() const { return _data.size(); }

    QVariant content() const { return static_cast<int>(_data.content()); }

    QVariant previousParentFolderId() const { return _data.previousParentFolderId().toULongLong(); }

    QVariant contentScheme() const { return _data.contentScheme(); }

    QVariant contentIdentifier() const { return _data.contentIdentifier(); }

    QVariant inResponseTo() const { return _data.inResponseTo().toULongLong(); }

    QVariant responseType() const { return static_cast<int>(_data.responseType()); }

    QVariant preview() const { return _data.preview(); }

    QVariant parentThreadId() const { return _data.parentThreadId().toULongLong(); }
};

// Class to extract QMailMessageMetaData properties from QVariant object
template<>
class MessageValueExtractor<QVariant>
{
    const QVariant &_value;
    
public:
    MessageValueExtractor(const QVariant &v) : _value(v) {}

    QMailMessageId id() const { return QMailMessageId(QMailStorePrivate::extractValue<quint64>(_value)); }

    QMailMessage::MessageType messageType() const { return static_cast<QMailMessage::MessageType>(QMailStorePrivate::extractValue<int>(_value)); }

    QMailFolderId parentFolderId() const { return QMailFolderId(QMailStorePrivate::extractValue<quint64>(_value)); }

    QMailAddress from() const { return QMailAddress(QMailStorePrivate::extractValue<QString>(_value)); }

    QList<QMailAddress> to() const { return QMailAddress::fromStringList(QMailStorePrivate::extractValue<QString>(_value)); }

    QString subject() const { return QMailStorePrivate::extractValue<QString>(_value); }

    QMailTimeStamp date() const { return QMailTimeStamp(QMailStorePrivate::extractValue<QDateTime>(_value)); }

    QMailTimeStamp receivedDate() const { return QMailTimeStamp(QMailStorePrivate::extractValue<QDateTime>(_value)); }

    quint64 status() const { return QMailStorePrivate::extractValue<quint64>(_value); }

    QMailAccountId parentAccountId() const { return QMailAccountId(QMailStorePrivate::extractValue<quint64>(_value)); }

    QString serverUid() const { return QMailStorePrivate::extractValue<QString>(_value); }

    int size() const { return QMailStorePrivate::extractValue<int>(_value); }

    QMailMessage::ContentType content() const { return static_cast<QMailMessage::ContentType>(QMailStorePrivate::extractValue<int>(_value)); }

    QMailFolderId previousParentFolderId() const { return QMailFolderId(QMailStorePrivate::extractValue<quint64>(_value)); }

    QString contentUri() const { return QMailStorePrivate::extractValue<QString>(_value); }

    QMailMessageId inResponseTo() const { return QMailMessageId(QMailStorePrivate::extractValue<quint64>(_value)); }

    QMailMessage::ResponseType responseType() const { return static_cast<QMailMessage::ResponseType>(QMailStorePrivate::extractValue<int>(_value)); }

    QString copyServerUid() const { return QMailStorePrivate::extractValue<QString>(_value); }

    QMailFolderId restoreFolderId() const { return QMailFolderId(QMailStorePrivate::extractValue<quint64>(_value)); }

    QString listId() const { return QMailStorePrivate::extractValue<QString>(_value); }

    QString rfcId() const { return QMailStorePrivate::extractValue<QString>(_value); }

    QString preview() const { return QMailStorePrivate::extractValue<QString>(_value); }

    QMailThreadId parentThreadId() const { return QMailThreadId(QMailStorePrivate::extractValue<quint64>(_value)); }
};

// Properties of the mailmessages table
static QMailStorePrivate::MessagePropertyMap messagePropertyMap()
{
    QMailStorePrivate::MessagePropertyMap map; 

    map.insert(QMailMessageKey::Id,"id");
    map.insert(QMailMessageKey::Type,"type");
    map.insert(QMailMessageKey::ParentFolderId,"parentfolderid");
    map.insert(QMailMessageKey::Sender,"sender");
    map.insert(QMailMessageKey::Recipients,"recipients");
    map.insert(QMailMessageKey::Subject,"subject");
    map.insert(QMailMessageKey::TimeStamp,"stamp");
    map.insert(QMailMessageKey::ReceptionTimeStamp,"receivedstamp");
    map.insert(QMailMessageKey::Status,"status");
    map.insert(QMailMessageKey::ParentAccountId,"parentaccountid");
    map.insert(QMailMessageKey::ServerUid,"serveruid");
    map.insert(QMailMessageKey::Size,"size");
    map.insert(QMailMessageKey::ContentType,"contenttype");
    map.insert(QMailMessageKey::PreviousParentFolderId,"previousparentfolderid");
    map.insert(QMailMessageKey::ContentScheme,"mailfile");
    map.insert(QMailMessageKey::ContentIdentifier,"mailfile");
    map.insert(QMailMessageKey::InResponseTo,"responseid");
    map.insert(QMailMessageKey::ResponseType,"responsetype");
    map.insert(QMailMessageKey::Conversation,"parentthreadid");
    map.insert(QMailMessageKey::CopyServerUid, "copyserveruid");
    map.insert(QMailMessageKey::RestoreFolderId, "restorefolderid");
    map.insert(QMailMessageKey::ListId, "listid");
    map.insert(QMailMessageKey::RfcId, "rfcid");
    map.insert(QMailMessageKey::Preview, "preview");
    map.insert(QMailMessageKey::ParentThreadId, "parentthreadid");
    return map;
}

static QString messagePropertyName(QMailMessageKey::Property property)
{
    static const QMailStorePrivate::MessagePropertyMap map(messagePropertyMap());

    QMailStorePrivate::MessagePropertyMap::const_iterator it = map.find(property);
    if (it != map.end())
        return it.value();

    if ((property != QMailMessageKey::AncestorFolderIds) &&
        (property != QMailMessageKey::Custom))
        qWarning() << "Unknown message property:" << property;
    
    return QString();
}

static bool caseInsensitiveProperty(QMailMessageKey::Property property)
{
    return ((property == QMailMessageKey::Sender) ||
            (property == QMailMessageKey::Recipients) ||
            (property == QMailMessageKey::Subject));
}

typedef QMap<QMailAccountKey::Property, QString> AccountPropertyMap;

// Properties of the mailaccounts table
static AccountPropertyMap accountPropertyMap() 
{
    AccountPropertyMap map; 

    map.insert(QMailAccountKey::Id,"id");
    map.insert(QMailAccountKey::Name,"name");
    map.insert(QMailAccountKey::MessageType,"type");
    map.insert(QMailAccountKey::FromAddress,"emailaddress");
    map.insert(QMailAccountKey::Status,"status");
    map.insert(QMailAccountKey::LastSynchronized, "lastsynchronized");
    map.insert(QMailAccountKey::IconPath,"iconpath");

    return map;
}

static QString accountPropertyName(QMailAccountKey::Property property)
{
    static const AccountPropertyMap map(accountPropertyMap());

    AccountPropertyMap::const_iterator it = map.find(property);
    if (it != map.end())
        return it.value();

    if (property != QMailAccountKey::Custom)
        qWarning() << "Unknown account property:" << property;

    return QString();
}

static bool caseInsensitiveProperty(QMailAccountKey::Property property)
{
    return ((property == QMailAccountKey::Name) ||
            (property == QMailAccountKey::FromAddress));
}

typedef QMap<QMailFolderKey::Property, QString> FolderPropertyMap;

// Properties of the mailfolders table
static FolderPropertyMap folderPropertyMap() 
{
    FolderPropertyMap map; 

    map.insert(QMailFolderKey::Id,"id");
    map.insert(QMailFolderKey::Path,"name");
    map.insert(QMailFolderKey::ParentFolderId,"parentid");
    map.insert(QMailFolderKey::ParentAccountId,"parentaccountid");
    map.insert(QMailFolderKey::DisplayName,"displayname");
    map.insert(QMailFolderKey::Status,"status");
    map.insert(QMailFolderKey::ServerCount,"servercount");
    map.insert(QMailFolderKey::ServerUnreadCount,"serverunreadcount");
    map.insert(QMailFolderKey::ServerUndiscoveredCount,"serverundiscoveredcount");

    return map;
}

static QString folderPropertyName(QMailFolderKey::Property property)
{
    static const FolderPropertyMap map(folderPropertyMap());

    FolderPropertyMap::const_iterator it = map.find(property);
    if (it != map.end())
        return it.value();

    if ((property != QMailFolderKey::AncestorFolderIds) &&
        (property != QMailFolderKey::Custom))
        qWarning() << "Unknown folder property:" << property;

    return QString();
}

static bool caseInsensitiveProperty(QMailFolderKey::Property property)
{
    return ((property == QMailFolderKey::Path) ||
            (property == QMailFolderKey::DisplayName));
}

typedef QMap<QMailThreadKey::Property, QString> ThreadPropertyMap;

// Properties of the mailthreads table
static ThreadPropertyMap threadPropertyMap()
{
    ThreadPropertyMap map;

    map.insert(QMailThreadKey::Id,"id");
    map.insert(QMailThreadKey::MessageCount, "messagecount");
    map.insert(QMailThreadKey::UnreadCount, "unreadcount");
    map.insert(QMailThreadKey::ServerUid, "serveruid");
    map.insert(QMailThreadKey::Includes, "id");
    map.insert(QMailThreadKey::ParentAccountId, "parentaccountid");
    map.insert(QMailThreadKey::Subject, "subject");
    map.insert(QMailThreadKey::Senders, "senders");
    map.insert(QMailThreadKey::LastDate, "lastDate");
    map.insert(QMailThreadKey::StartedDate, "startedDate");
    map.insert(QMailThreadKey::Status, "status");
    map.insert(QMailThreadKey::Preview, "preview");
    return map;
}

static QString threadPropertyName(QMailThreadKey::Property property)
{
    static const ThreadPropertyMap map(threadPropertyMap());

    ThreadPropertyMap::const_iterator it = map.find(property);
    if (it != map.end())
        return it.value();

   qWarning() << "Unknown thread property:" << property;

    return QString();
}

static bool caseInsensitiveProperty(QMailThreadKey::Property property)
{
    return ((property == QMailThreadKey::Subject) ||
            (property == QMailThreadKey::Senders));
}


// Build lists of column names from property values

static QString qualifiedName(const QString &name, const QString &alias)
{
    if (alias.isEmpty())
        return name;

    return (alias + '.' + name);
}

template<typename PropertyType>
QString fieldName(PropertyType property, const QString &alias);

template<>
QString fieldName<QMailMessageKey::Property>(QMailMessageKey::Property property, const QString& alias)
{
    return qualifiedName(messagePropertyName(property), alias);
}

template<>
QString fieldName<QMailFolderKey::Property>(QMailFolderKey::Property property, const QString& alias)
{
    return qualifiedName(folderPropertyName(property), alias);
}

template<>
QString fieldName<QMailThreadKey::Property>(QMailThreadKey::Property property, const QString &alias)
{
    return qualifiedName(threadPropertyName(property), alias);
}

template<>
QString fieldName<QMailAccountKey::Property>(QMailAccountKey::Property property, const QString& alias)
{
    return qualifiedName(accountPropertyName(property), alias);
}

template<typename SourceType, typename TargetType>
TargetType matchingProperty(SourceType source);

static QMap<QMailMessageSortKey::Property, QMailMessageKey::Property> messageSortMapInit()
{
    QMap<QMailMessageSortKey::Property, QMailMessageKey::Property> map;

    // Provide a mapping of sort key properties to the corresponding filter key
    map.insert(QMailMessageSortKey::Id, QMailMessageKey::Id);
    map.insert(QMailMessageSortKey::Type, QMailMessageKey::Type);
    map.insert(QMailMessageSortKey::ParentFolderId, QMailMessageKey::ParentFolderId);
    map.insert(QMailMessageSortKey::Sender, QMailMessageKey::Sender);
    map.insert(QMailMessageSortKey::Recipients, QMailMessageKey::Recipients);
    map.insert(QMailMessageSortKey::Subject, QMailMessageKey::Subject);
    map.insert(QMailMessageSortKey::TimeStamp, QMailMessageKey::TimeStamp);
    map.insert(QMailMessageSortKey::ReceptionTimeStamp, QMailMessageKey::ReceptionTimeStamp);
    map.insert(QMailMessageSortKey::Status, QMailMessageKey::Status);
    map.insert(QMailMessageSortKey::ParentAccountId, QMailMessageKey::ParentAccountId);
    map.insert(QMailMessageSortKey::ServerUid, QMailMessageKey::ServerUid);
    map.insert(QMailMessageSortKey::Size, QMailMessageKey::Size);
    map.insert(QMailMessageSortKey::ContentType, QMailMessageKey::ContentType);
    map.insert(QMailMessageSortKey::PreviousParentFolderId, QMailMessageKey::PreviousParentFolderId);
    map.insert(QMailMessageSortKey::CopyServerUid, QMailMessageKey::CopyServerUid);
    map.insert(QMailMessageSortKey::ListId, QMailMessageKey::ListId);
    map.insert(QMailMessageSortKey::RestoreFolderId, QMailMessageKey::RestoreFolderId);
    map.insert(QMailMessageSortKey::RfcId, QMailMessageKey::RfcId);
    map.insert(QMailMessageSortKey::ParentThreadId, QMailMessageKey::ParentThreadId);

    return map;
}

template<>
QMailMessageKey::Property matchingProperty<QMailMessageSortKey::Property, QMailMessageKey::Property>(QMailMessageSortKey::Property source)
{
    static QMap<QMailMessageSortKey::Property, QMailMessageKey::Property> map(messageSortMapInit());
    return map.value(source);
}

static bool caseInsensitiveProperty(QMailMessageSortKey::Property property)
{
    return caseInsensitiveProperty(matchingProperty<QMailMessageSortKey::Property, QMailMessageKey::Property>(property));
}

static QMap<QMailFolderSortKey::Property, QMailFolderKey::Property> folderSortMapInit()
{
    QMap<QMailFolderSortKey::Property, QMailFolderKey::Property> map;

    // Provide a mapping of sort key properties to the corresponding filter key
    map.insert(QMailFolderSortKey::Id, QMailFolderKey::Id);
    map.insert(QMailFolderSortKey::Path, QMailFolderKey::Path);
    map.insert(QMailFolderSortKey::ParentFolderId, QMailFolderKey::ParentFolderId);
    map.insert(QMailFolderSortKey::ParentAccountId, QMailFolderKey::ParentAccountId);
    map.insert(QMailFolderSortKey::DisplayName, QMailFolderKey::DisplayName);
    map.insert(QMailFolderSortKey::Status, QMailFolderKey::Status);
    map.insert(QMailFolderSortKey::ServerCount, QMailFolderKey::ServerCount);
    map.insert(QMailFolderSortKey::ServerUnreadCount, QMailFolderKey::ServerUnreadCount);
    map.insert(QMailFolderSortKey::ServerUndiscoveredCount, QMailFolderKey::ServerUndiscoveredCount);

    return map;
}

template<>
QMailFolderKey::Property matchingProperty<QMailFolderSortKey::Property, QMailFolderKey::Property>(QMailFolderSortKey::Property source)
{
    static QMap<QMailFolderSortKey::Property, QMailFolderKey::Property> map(folderSortMapInit());
    return map.value(source);
}

static bool caseInsensitiveProperty(QMailFolderSortKey::Property property)
{
    return caseInsensitiveProperty(matchingProperty<QMailFolderSortKey::Property, QMailFolderKey::Property>(property));
}

static QMap<QMailThreadSortKey::Property, QMailThreadKey::Property> threadSortMapInit()
{
    QMap<QMailThreadSortKey::Property, QMailThreadKey::Property> map;

    // Provide a mapping of sort key properties to the corresponding filter key
    map.insert(QMailThreadSortKey::Id, QMailThreadKey::Id);
    map.insert(QMailThreadSortKey::ParentAccountId, QMailThreadKey::ParentAccountId);
    map.insert(QMailThreadSortKey::ServerUid, QMailThreadKey::ServerUid);
    map.insert(QMailThreadSortKey::UnreadCount, QMailThreadKey::UnreadCount);
    map.insert(QMailThreadSortKey::MessageCount, QMailThreadKey::MessageCount);
    map.insert(QMailThreadSortKey::Subject, QMailThreadKey::Subject);
    map.insert(QMailThreadSortKey::Preview, QMailThreadKey::Preview);
    map.insert(QMailThreadSortKey::Senders, QMailThreadKey::Senders);
    map.insert(QMailThreadSortKey::LastDate, QMailThreadKey::LastDate);
    map.insert(QMailThreadSortKey::StartedDate, QMailThreadKey::StartedDate);
    map.insert(QMailThreadSortKey::Status, QMailThreadKey::Status);

    return map;
}

template<>
QMailThreadKey::Property matchingProperty<QMailThreadSortKey::Property, QMailThreadKey::Property>(QMailThreadSortKey::Property source)
{
    static QMap<QMailThreadSortKey::Property, QMailThreadKey::Property> map(threadSortMapInit());
    return map.value(source);
}

static bool caseInsensitiveProperty(QMailThreadSortKey::Property property)
{
    return caseInsensitiveProperty(matchingProperty<QMailThreadSortKey::Property, QMailThreadKey::Property>(property));
}


static QMap<QMailAccountSortKey::Property, QMailAccountKey::Property> accountSortMapInit()
{
    QMap<QMailAccountSortKey::Property, QMailAccountKey::Property> map;

    // Provide a mapping of sort key properties to the corresponding filter key
    map.insert(QMailAccountSortKey::Id, QMailAccountKey::Id);
    map.insert(QMailAccountSortKey::Name, QMailAccountKey::Name);
    map.insert(QMailAccountSortKey::MessageType, QMailAccountKey::MessageType);
    map.insert(QMailAccountSortKey::Status, QMailAccountKey::Status);
    map.insert(QMailAccountSortKey::LastSynchronized, QMailAccountKey::LastSynchronized);
    map.insert(QMailAccountSortKey::IconPath, QMailAccountKey::IconPath);

    return map;
}

template<>
QMailAccountKey::Property matchingProperty<QMailAccountSortKey::Property, QMailAccountKey::Property>(QMailAccountSortKey::Property source)
{
    static QMap<QMailAccountSortKey::Property, QMailAccountKey::Property> map(accountSortMapInit());
    return map.value(source);
}

static bool caseInsensitiveProperty(QMailAccountSortKey::Property property)
{
    return caseInsensitiveProperty(matchingProperty<QMailAccountSortKey::Property, QMailAccountKey::Property>(property));
}

template<>
QString fieldName<QMailMessageSortKey::Property>(QMailMessageSortKey::Property property, const QString &alias)
{
    return qualifiedName(messagePropertyName(matchingProperty<QMailMessageSortKey::Property, QMailMessageKey::Property>(property)), alias);
}

template<>
QString fieldName<QMailFolderSortKey::Property>(QMailFolderSortKey::Property property, const QString &alias)
{
    return qualifiedName(folderPropertyName(matchingProperty<QMailFolderSortKey::Property, QMailFolderKey::Property>(property)), alias);
}

template<>
QString fieldName<QMailThreadSortKey::Property>(QMailThreadSortKey::Property property, const QString &alias)
{
    return qualifiedName(threadPropertyName(matchingProperty<QMailThreadSortKey::Property, QMailThreadKey::Property>(property)), alias);
}

template<>
QString fieldName<QMailAccountSortKey::Property>(QMailAccountSortKey::Property property, const QString &alias)
{
    return qualifiedName(accountPropertyName(matchingProperty<QMailAccountSortKey::Property, QMailAccountKey::Property>(property)), alias);
}

template<typename PropertyType>
QString fieldNames(const QList<PropertyType> &properties, const QString &separator, const QString &alias)
{
    QStringList fields;
    foreach (const PropertyType &property, properties)
        fields.append(fieldName(property, alias));

    return fields.join(separator);
}

template<typename ArgumentType>
void appendWhereValues(const ArgumentType &a, QVariantList &values);

template<typename KeyType>
QVariantList whereClauseValues(const KeyType& key)
{
    QVariantList values;

    foreach (const typename KeyType::ArgumentType& a, key.arguments())
        ::appendWhereValues(a, values);

    foreach (const KeyType& subkey, key.subKeys())
        values += ::whereClauseValues<KeyType>(subkey);

    return values;
}

template <typename Key, typename Argument = typename Key::ArgumentType>
class ArgumentExtractorBase
{
protected:
    const Argument &arg;
    
    ArgumentExtractorBase(const Argument &a) : arg(a) {}

    QString minimalString(const QString &s) const
    {
        // If the argument is a phone number, ensure it is in minimal form
        QMailAddress address(s);
        if (address.isPhoneNumber()) {
            QString minimal(address.minimalPhoneNumber());

            // Rather than compare exact numbers, we will only use the trailing
            // digits to compare phone numbers - otherwise, slightly different 
            // forms of the same number will not be matched
            static const int significantDigits = 8;

            int extraneous = minimal.length() - significantDigits;
            if (extraneous > 0)
                minimal.remove(0, extraneous);

            return minimal;
        }

        return s;
    }

    QString submatchString(const QString &s, bool valueMinimalised) const
    {
        if (!s.isEmpty()) {
            // Delimit data for sql "LIKE" operator
            if (((arg.op == Includes) || (arg.op == Excludes)) || (((arg.op == Equal) || (arg.op == NotEqual)) && valueMinimalised))
                return QString('%' + s + '%');
        } else if ((arg.op == Includes) || (arg.op == Excludes)) {
            return QString('%');
        }

        return s;
    }

    QString addressStringValue() const
    {
        return submatchString(minimalString(QMailStorePrivate::extractValue<QString>(arg.valueList.first())), true);
    }

    QString stringValue() const
    {
        return submatchString(QMailStorePrivate::extractValue<QString>(arg.valueList.first()), false);
    }

    QVariantList stringValues() const
    {
        QVariantList values;

        if (arg.valueList.count() == 1) {
            values.append(stringValue());
        } else {
            // Includes/Excludes is not a pattern match with multiple values
            foreach (const QVariant &item, arg.valueList)
                values.append(QMailStorePrivate::extractValue<QString>(item));
        }

        return values;
    }

    template<typename ID>
    quint64 idValue() const
    {
        return QMailStorePrivate::extractValue<ID>(arg.valueList.first()).toULongLong(); 
    }

    template<typename ClauseKey>
    QVariantList idValues() const
    {
        const QVariant& var = arg.valueList.first();

        if (var.canConvert<ClauseKey>()) {
            return ::whereClauseValues(var.value<ClauseKey>());
        } else {
            QVariantList values;

            foreach (const QVariant &item, arg.valueList)
                values.append(QMailStorePrivate::extractValue<typename ClauseKey::IdType>(item).toULongLong());

            return values;
        }
    }

    int intValue() const
    {
        return QMailStorePrivate::extractValue<int>(arg.valueList.first());
    }

    QVariantList intValues() const
    {
        QVariantList values;

        foreach (const QVariant &item, arg.valueList)
            values.append(QMailStorePrivate::extractValue<int>(item));

        return values;
    }

    int quint64Value() const
    {
        return QMailStorePrivate::extractValue<quint64>(arg.valueList.first());
    }

    QVariantList customValues() const
    {
        QVariantList values;

        QStringList constraints = QMailStorePrivate::extractValue<QStringList>(arg.valueList.first());
        // Field name required for existence or value test
        values.append(constraints.takeFirst());

        if (!constraints.isEmpty()) {
            // For a value test, we need the comparison value also
            values.append(submatchString(constraints.takeFirst(), false));
        }

        return values;
    }
};


template<typename PropertyType, typename BitmapType = int>
class RecordExtractorBase
{
protected:
    const QSqlRecord &record;
    const BitmapType bitmap;

    RecordExtractorBase(const QSqlRecord &r, BitmapType b = 0) : record(r), bitmap(b) {}
    virtual ~RecordExtractorBase() {}
    
    template<typename ValueType>
    ValueType value(const QString &field, const ValueType &defaultValue = ValueType()) const 
    { 
        int index(fieldIndex(field, bitmap));

        if (record.isNull(index))
            return defaultValue;
        else
            return QMailStorePrivate::extractValue<ValueType>(record.value(index), defaultValue);
    }
    
    template<typename ValueType>
    ValueType value(PropertyType p, const ValueType &defaultValue = ValueType()) const 
    { 
        return value(fieldName(p, QString()), defaultValue);
    }

    virtual int fieldIndex(const QString &field, BitmapType b) const = 0;

    int mappedFieldIndex(const QString &field, BitmapType bitmap, QMap<BitmapType, QMap<QString, int> > &fieldIndex) const
    {
        typename QMap<BitmapType, QMap<QString, int> >::iterator it = fieldIndex.find(bitmap);
        if (it == fieldIndex.end()) {
            it = fieldIndex.insert(bitmap, QMap<QString, int>());
        }

        QMap<QString, int> &fields(it.value());

        QMap<QString, int>::iterator fit = fields.find(field);
        if (fit != fields.end())
            return fit.value();

        int index = record.indexOf(field);
        fields.insert(field, index);
        return index;
    }
};


// Class to extract data from records of the mailmessages table
class MessageRecord : public RecordExtractorBase<QMailMessageKey::Property, QMailMessageKey::Properties>
{
public:
    MessageRecord(const QSqlRecord &r, QMailMessageKey::Properties props) 
        : RecordExtractorBase<QMailMessageKey::Property, QMailMessageKey::Properties>(r, props) {}

    QMailMessageId id() const { return QMailMessageId(value<quint64>(QMailMessageKey::Id)); }

    QMailMessage::MessageType messageType() const { return static_cast<QMailMessage::MessageType>(value<int>(QMailMessageKey::Type, QMailMessage::None)); }

    QMailFolderId parentFolderId() const { return QMailFolderId(value<quint64>(QMailMessageKey::ParentFolderId)); }

    QMailAddress from() const { return QMailAddress(value<QString>(QMailMessageKey::Sender)); }

    QList<QMailAddress> to() const { return QMailAddress::fromStringList(value<QString>(QMailMessageKey::Recipients)); }

    QString subject() const { return value<QString>(QMailMessageKey::Subject); }

    QMailTimeStamp date() const
    {
        //Database date/time is in UTC format
        QDateTime tmp = value<QDateTime>(QMailMessageKey::TimeStamp);
        tmp.setTimeSpec(Qt::UTC);
        return QMailTimeStamp(tmp);
    }

    QMailTimeStamp receivedDate() const
    {
        //Database date/time is in UTC format
        QDateTime tmp = value<QDateTime>(QMailMessageKey::ReceptionTimeStamp);
        tmp.setTimeSpec(Qt::UTC);
        return QMailTimeStamp(tmp);
    }

    quint64 status() const { return value<quint64>(QMailMessageKey::Status, 0); }

    QMailAccountId parentAccountId() const { return QMailAccountId(value<quint64>(QMailMessageKey::ParentAccountId)); }

    QString serverUid() const { return value<QString>(QMailMessageKey::ServerUid); }

    int size() const { return value<int>(QMailMessageKey::Size); }

    QMailMessage::ContentType content() const { return static_cast<QMailMessage::ContentType>(value<int>(QMailMessageKey::ContentType, QMailMessage::UnknownContent)); }

    QMailFolderId previousParentFolderId() const { return QMailFolderId(value<quint64>(QMailMessageKey::PreviousParentFolderId)); }

    QString contentScheme() const 
    { 
        if (_uriElements.first.isNull()) 
            _uriElements = extractUriElements(value<QString>(QMailMessageKey::ContentScheme)); 

        return _uriElements.first;
    }

    QString contentIdentifier() const 
    { 
        if (_uriElements.first.isNull()) 
            _uriElements = extractUriElements(value<QString>(QMailMessageKey::ContentIdentifier)); 

        return _uriElements.second;
    }

    QMailMessageId inResponseTo() const { return QMailMessageId(value<quint64>(QMailMessageKey::InResponseTo)); }

    QMailMessage::ResponseType responseType() const { return static_cast<QMailMessage::ResponseType>(value<int>(QMailMessageKey::ResponseType, QMailMessage::NoResponse)); }


    QString copyServerUid() const { return value<QString>(QMailMessageKey::CopyServerUid); }

    QMailFolderId restoreFolderId() const { return QMailFolderId(value<quint64>(QMailMessageKey::RestoreFolderId)); }

    QString listId() const { return value<QString>(QMailMessageKey::ListId); }

    QString rfcId() const { return value<QString>(QMailMessageKey::RfcId); }

    QString preview() const { return value<QString>(QMailMessageKey::Preview); }

    QMailThreadId parentThreadId() const { return QMailThreadId(value<quint64>(QMailMessageKey::ParentThreadId)); }

private:
    int fieldIndex(const QString &field, QMailMessageKey::Properties props) const
    {
        return mappedFieldIndex(field, props, _fieldIndex);
    }

    mutable QPair<QString, QString> _uriElements;

    static QMap<QMailMessageKey::Properties, QMap<QString, int> > _fieldIndex;
};

QMap<QMailMessageKey::Properties, QMap<QString, int> > MessageRecord::_fieldIndex;


// Class to convert QMailMessageKey argument values to SQL bind values
class MessageKeyArgumentExtractor : public ArgumentExtractorBase<QMailMessageKey>
{
public:
    MessageKeyArgumentExtractor(const QMailMessageKey::ArgumentType &a) 
        : ArgumentExtractorBase<QMailMessageKey>(a) {}

    QVariantList id() const { return idValues<QMailMessageKey>(); }

    QVariant messageType() const { return intValue(); }

    QVariantList parentFolderId() const { return idValues<QMailFolderKey>(); }

    QVariantList ancestorFolderIds() const {  return idValues<QMailFolderKey>(); }

    QVariantList sender() const { return stringValues(); }

    QVariant recipients() const { return addressStringValue(); }

    QVariantList subject() const { return stringValues(); }

    QVariant date() const { return QMailStorePrivate::extractValue<QDateTime>(arg.valueList.first()); }

    QVariant receivedDate() const { return QMailStorePrivate::extractValue<QDateTime>(arg.valueList.first()); }

    QVariant status() const
    {
        // The UnloadedData flag has no meaningful persistent value
        return (QMailStorePrivate::extractValue<quint64>(arg.valueList.first()) & ~QMailMessage::UnloadedData);
    }

    QVariantList parentAccountId() const { return idValues<QMailAccountKey>(); }

    QVariantList serverUid() const { return stringValues(); }

    QVariant size() const { return intValue(); }

    QVariantList content() const { return intValues(); }

    QVariantList previousParentFolderId() const { return idValues<QMailFolderKey>(); }

    QVariant contentScheme() const 
    { 
        // Any colons in the field will be stored in escaped format
        QString value(escape(QMailStorePrivate::extractValue<QString>(arg.valueList.first()), ':')); 

        if ((arg.op == Includes) || (arg.op == Excludes)) {
            value.prepend('%').append('%');
        } else if ((arg.op == Equal) || (arg.op == NotEqual)) {
            value.append(":%");
        }
        return value;
    }

    QVariant contentIdentifier() const 
    { 
        // Any colons in the field will be stored in escaped format
        QString value(escape(QMailStorePrivate::extractValue<QString>(arg.valueList.first()), ':')); 

        if ((arg.op == Includes) || (arg.op == Excludes)) {
            value.prepend('%').append('%');
        } else if ((arg.op == Equal) || (arg.op == NotEqual)) {
            value.prepend("%:");
        }
        return value;
    }

    QVariantList inResponseTo() const { return idValues<QMailMessageKey>(); }

    QVariantList responseType() const { return intValues(); }

    QVariantList conversation() const { return idValues<QMailMessageKey>(); }

    QVariantList custom() const { return customValues(); }

    QVariantList copyServerUid() const { return stringValues(); }

    QVariantList listId() const { return stringValues(); }

    QVariantList restoreFolderId() const { return idValues<QMailFolderKey>(); }

    QVariantList rfcId() const { return stringValues(); }

    QVariantList preview() const { return stringValues(); }

    QVariantList parentThreadId() const { return idValues<QMailThreadKey>(); }
};

template<>
void appendWhereValues<QMailMessageKey::ArgumentType>(const QMailMessageKey::ArgumentType &a, QVariantList &values)
{
    const MessageKeyArgumentExtractor extractor(a);

    switch (a.property)
    { 
    case QMailMessageKey::Id:
        if (a.valueList.count() < IdLookupThreshold) {
            values += extractor.id();
        } else {
            // This value match has been replaced by a table lookup
        }
        break;

    case QMailMessageKey::Type:
        values += extractor.messageType();
        break;

    case QMailMessageKey::ParentFolderId:
        values += extractor.parentFolderId();
        break;

    case QMailMessageKey::AncestorFolderIds:
        values += extractor.ancestorFolderIds();
        break;

    case QMailMessageKey::Sender:
        values += extractor.sender();
        break;

    case QMailMessageKey::Recipients:
        values += extractor.recipients();
        break;

    case QMailMessageKey::Subject:
        values += extractor.subject();
        break;

    case QMailMessageKey::TimeStamp:
        values += extractor.date();
        break;

    case QMailMessageKey::ReceptionTimeStamp:
        values += extractor.receivedDate();
        break;

    case QMailMessageKey::Status:
        values += extractor.status();
        break;

    case QMailMessageKey::ParentAccountId:
        values += extractor.parentAccountId();
        break;

    case QMailMessageKey::ServerUid:
        if (a.valueList.count() < IdLookupThreshold) {
            values += extractor.serverUid();
        } else {
            // This value match has been replaced by a table lookup
        }
        break;

    case QMailMessageKey::Size:
        values += extractor.size();
        break;

    case QMailMessageKey::ContentType:
        values += extractor.content();
        break;

    case QMailMessageKey::PreviousParentFolderId:
        values += extractor.previousParentFolderId();
        break;

    case QMailMessageKey::ContentScheme:
        values += extractor.contentScheme();
        break;

    case QMailMessageKey::ContentIdentifier:
        values += extractor.contentIdentifier();
        break;

    case QMailMessageKey::InResponseTo:
        values += extractor.inResponseTo();
        break;

    case QMailMessageKey::ResponseType:
        values += extractor.responseType();
        break;

    case QMailMessageKey::Conversation:
        values += extractor.conversation();
        break;

    case QMailMessageKey::Custom:
        values += extractor.custom();
        break;

    case QMailMessageKey::CopyServerUid:
        values += extractor.copyServerUid();
        break;

    case QMailMessageKey::ListId:
        values += extractor.listId();
        break;

    case QMailMessageKey::RestoreFolderId:
        values += extractor.restoreFolderId();
        break;

    case QMailMessageKey::RfcId:
        values += extractor.rfcId();
        break;

    case QMailMessageKey::Preview:
        values += extractor.preview();
        break;

    case QMailMessageKey::ParentThreadId:
        values += extractor.parentThreadId();
        break;
    }
}


// Class to extract data from records of the mailaccounts table
class AccountRecord : public RecordExtractorBase<QMailAccountKey::Property>
{
public:
    AccountRecord(const QSqlRecord &r) 
        : RecordExtractorBase<QMailAccountKey::Property>(r) {}

    QMailAccountId id() const { return QMailAccountId(value<quint64>(QMailAccountKey::Id)); }

    QString name() const { return value<QString>(QMailAccountKey::Name); }

    QMailMessage::MessageType messageType() const { return static_cast<QMailMessage::MessageType>(value<int>(QMailAccountKey::MessageType, -1)); }

    QString fromAddress() const { return value<QString>(QMailAccountKey::FromAddress); }

    quint64 status() const { return value<quint64>(QMailAccountKey::Status); }

    QString signature() const { return value<QString>("signature"); }

    QMailTimeStamp lastSynchronized() const { return QMailTimeStamp(value<QDateTime>(QMailAccountKey::LastSynchronized)); }

    QString iconPath() const { return value<QString>(QMailAccountKey::IconPath); }
private:
    int fieldIndex(const QString &field, int props) const
    {
        return mappedFieldIndex(field, props, _fieldIndex);
    }

    static QMap<int, QMap<QString, int> > _fieldIndex;
};

QMap<int, QMap<QString, int> > AccountRecord::_fieldIndex;


// Class to convert QMailAccountKey argument values to SQL bind values
class AccountKeyArgumentExtractor : public ArgumentExtractorBase<QMailAccountKey>
{
public:
    AccountKeyArgumentExtractor(const QMailAccountKey::ArgumentType &a)
        : ArgumentExtractorBase<QMailAccountKey>(a) {}

    QVariantList id() const { return idValues<QMailAccountKey>(); }

    QVariantList name() const { return stringValues(); }

    QVariant messageType() const { return intValue(); }

    QVariant fromAddress() const 
    { 
        QString value(QMailStorePrivate::extractValue<QString>(arg.valueList.first()));

        // This test will be converted to a LIKE test, for all comparators
        if (arg.op == Equal || arg.op == NotEqual) {
            // Ensure exact match by testing for address delimiters
            value.prepend('<').append('>');
        }

        return value.prepend('%').append('%');
    }

    QVariant status() const { return quint64Value(); }

    QVariantList custom() const { return customValues(); }

    QVariant lastSynchronized() const { return QMailStorePrivate::extractValue<QDateTime>(arg.valueList.first()); }

    QVariantList iconPath() const { return stringValues(); }

};

template<>
void appendWhereValues<QMailAccountKey::ArgumentType>(const QMailAccountKey::ArgumentType &a, QVariantList &values)
{
    const AccountKeyArgumentExtractor extractor(a);

    switch (a.property)
    {
    case QMailAccountKey::Id:
        values += extractor.id();
        break;

    case QMailAccountKey::Name:
        values += extractor.name();
        break;

    case QMailAccountKey::MessageType:
        values += extractor.messageType();
        break;

    case QMailAccountKey::FromAddress:
        values += extractor.fromAddress();
        break;

    case QMailAccountKey::Status:
        values += extractor.status();
        break;

    case QMailAccountKey::Custom:
        values += extractor.custom();
        break;

    case QMailAccountKey::LastSynchronized:
        values += extractor.lastSynchronized();
        break;

    case QMailAccountKey::IconPath:
        values +=extractor.iconPath();
    }
}


// Class to extract data from records of the mailfolders table
class FolderRecord : public RecordExtractorBase<QMailFolderKey::Property>
{
public:
    FolderRecord(const QSqlRecord &r)
        : RecordExtractorBase<QMailFolderKey::Property>(r) {}

    QMailFolderId id() const { return QMailFolderId(value<quint64>(QMailFolderKey::Id)); }

    QString path() const { return value<QString>(QMailFolderKey::Path); }

    QMailFolderId parentFolderId() const { return QMailFolderId(value<quint64>(QMailFolderKey::ParentFolderId)); }

    QMailAccountId parentAccountId() const { return QMailAccountId(value<quint64>(QMailFolderKey::ParentAccountId)); }

    QString displayName() const { return value<QString>(QMailFolderKey::DisplayName); }

    quint64 status() const { return value<quint64>(QMailFolderKey::Status); }

    uint serverCount() const { return value<uint>(QMailFolderKey::ServerCount); }

    uint serverUnreadCount() const { return value<uint>(QMailFolderKey::ServerUnreadCount); }

    uint serverUndiscoveredCount() const { return value<uint>(QMailFolderKey::ServerUndiscoveredCount); }

private:
    int fieldIndex(const QString &field, int props) const
    {
        return mappedFieldIndex(field, props, _fieldIndex);
    }

    static QMap<int, QMap<QString, int> > _fieldIndex;
};

QMap<int, QMap<QString, int> > FolderRecord::_fieldIndex;


// Class to convert QMailFolderKey argument values to SQL bind values
class FolderKeyArgumentExtractor : public ArgumentExtractorBase<QMailFolderKey>
{
public:
    FolderKeyArgumentExtractor(const QMailFolderKey::ArgumentType &a)
        : ArgumentExtractorBase<QMailFolderKey>(a) {}

    QVariantList id() const { return idValues<QMailFolderKey>(); }

    QVariantList path() const { return stringValues(); }

    QVariantList parentFolderId() const { return idValues<QMailFolderKey>(); }

    QVariantList ancestorFolderIds() const {  return idValues<QMailFolderKey>(); }

    QVariantList parentAccountId() const { return idValues<QMailAccountKey>(); }

    QVariantList displayName() const { return stringValues(); }

    QVariant status() const { return quint64Value(); }

    QVariant serverCount() const { return intValue(); }

    QVariant serverUnreadCount() const { return intValue(); }

    QVariant serverUndiscoveredCount() const { return intValue(); }

    QVariantList custom() const { return customValues(); }
};

template<>
void appendWhereValues<QMailFolderKey::ArgumentType>(const QMailFolderKey::ArgumentType &a, QVariantList &values)
{
    const FolderKeyArgumentExtractor extractor(a);

    switch (a.property)
    {
    case QMailFolderKey::Id:
        values += extractor.id();
        break;

    case QMailFolderKey::Path:
        values += extractor.path();
        break;

    case QMailFolderKey::ParentFolderId:
        values += extractor.parentFolderId();
        break;

    case QMailFolderKey::AncestorFolderIds:
        values += extractor.ancestorFolderIds();
        break;

    case QMailFolderKey::ParentAccountId:
        values += extractor.parentAccountId();
        break;

    case QMailFolderKey::DisplayName:
        values += extractor.displayName();
        break;

    case QMailFolderKey::Status:
        values += extractor.status();
        break;

    case QMailFolderKey::ServerCount:
        values += extractor.serverCount();
        break;

    case QMailFolderKey::ServerUnreadCount:
        values += extractor.serverUnreadCount();
        break;

    case QMailFolderKey::ServerUndiscoveredCount:
        values += extractor.serverUndiscoveredCount();
        break;

    case QMailFolderKey::Custom:
        values += extractor.custom();
        break;
    }
}


// Class to extract data from records of the mailfolders table
class ThreadRecord : public RecordExtractorBase<QMailThreadKey::Property>
{
public:
    ThreadRecord(const QSqlRecord &r)
        : RecordExtractorBase<QMailThreadKey::Property>(r) {}

    QMailThreadId id() const { return QMailThreadId(value<quint64>(QMailThreadKey::Id)); }

    QString serverUid() const { return value<QString>(QMailThreadKey::ServerUid); }

    uint messageCount() const { return value<uint>(QMailThreadKey::MessageCount); }

    uint unreadCount() const { return value<uint>(QMailThreadKey::UnreadCount); }

    QMailAccountId parentAccountId() const { return QMailAccountId(value<quint64>(QMailThreadKey::ParentAccountId)); }

    QString subject() const { return value<QString>(QMailThreadKey::Subject); }
    QString preview() const { return value<QString>(QMailThreadKey::Preview); }
    QMailAddressList senders() const { return QMailAddress::fromStringList(value<QString>(QMailThreadKey::Senders)); }
    QMailTimeStamp lastDate() const
    {
        //Database date/time is in UTC format
        QDateTime tmp = value<QDateTime>(QMailThreadKey::LastDate);
        tmp.setTimeSpec(Qt::UTC);
        return QMailTimeStamp(tmp);
    }
    QMailTimeStamp startedDate() const
    {
        //Database date/time is in UTC format
        QDateTime tmp = value<QDateTime>(QMailThreadKey::StartedDate);
        tmp.setTimeSpec(Qt::UTC);
        return QMailTimeStamp(tmp);
    }
    quint64 status() const { return value<quint64>(QMailThreadKey::Status); }

private:
    int fieldIndex(const QString &field, int props) const
    {
        return mappedFieldIndex(field, props, _fieldIndex);
    }

    static QMap<int, QMap<QString, int> > _fieldIndex;
};

QMap<int, QMap<QString, int> > ThreadRecord::_fieldIndex;

// Class to convert QMailFolderKey argument values to SQL bind values
class ThreadKeyArgumentExtractor : public ArgumentExtractorBase<QMailThreadKey>
{
public:
    ThreadKeyArgumentExtractor(const QMailThreadKey::ArgumentType &a)
        : ArgumentExtractorBase<QMailThreadKey>(a) {}

    QVariantList id() const { return idValues<QMailThreadKey>(); }

    QVariantList serverUid() const { return stringValues(); }

    QVariant messageCount() const { return intValue(); }

    QVariant unreadCount() const { return intValue(); }

    QVariantList custom() const { return customValues(); }

    QVariantList includes() const { return idValues<QMailMessageKey>(); }

    QVariantList parentAccountId() const { return idValues<QMailAccountKey>(); }

    QVariant subject() const { return stringValue(); }

    QVariant preview() const { return stringValue(); }

    // Why MessageKeyArgumentExtractor::sender returns QVariantList of stringValues?
    // Senders: %1 = Names of the message senders, latest message sender as
    // first. Names are separated with comma (,)
    QVariant senders() const { return stringValue(); }

    QVariant lastDate() const { return QMailStorePrivate::extractValue<QDateTime>(arg.valueList.first()); }

    QVariant startedDate() const { return QMailStorePrivate::extractValue<QDateTime>(arg.valueList.first()); }

    QVariant status() const { return quint64Value(); }
};

template<>
void appendWhereValues<QMailThreadKey::ArgumentType>(const QMailThreadKey::ArgumentType &a, QVariantList &values)
{
    const ThreadKeyArgumentExtractor extractor(a);

    switch (a.property)
    {
    case QMailThreadKey::Id:
        values += extractor.id();
        break;

    case QMailThreadKey::ServerUid:
        values += extractor.serverUid();
        break;

    case QMailThreadKey::UnreadCount:
        values += extractor.unreadCount();
        break;

    case QMailThreadKey::MessageCount:
        values += extractor.messageCount();
        break;

    case QMailThreadKey::Custom:
        values += extractor.custom();
        break;

    case QMailThreadKey::Includes:
        values += extractor.includes();
        break;

    case QMailThreadKey::ParentAccountId:
        values += extractor.parentAccountId();
        break;

    case QMailThreadKey::Subject:
        values += extractor.subject();
        break;
    case QMailThreadKey::Senders:
        values += extractor.senders();
        break;
    case QMailThreadKey::Preview:
        values += extractor.preview();
        break;
    case QMailThreadKey::LastDate:
        values += extractor.lastDate();
        break;
    case QMailThreadKey::StartedDate:
    case QMailThreadKey::Status:
        Q_ASSERT (false);
        break;
    }
}


// Class to extract data from records of the deletedmessages table
class MessageRemovalRecord : public RecordExtractorBase<int>
{
public:
    MessageRemovalRecord(const QSqlRecord &r)
        : RecordExtractorBase<int>(r) {}

    quint64 id() const { return value<quint64>("id"); }

    QMailAccountId parentAccountId() const { return QMailAccountId(value<quint64>("parentaccountid")); }

    QString serverUid() const { return value<QString>("serveruid"); }

    QMailFolderId parentFolderId() const { return QMailFolderId(value<quint64>("parentfolderid")); }

private:
    int fieldIndex(const QString &field, int props) const
    {
        return mappedFieldIndex(field, props, _fieldIndex);
    }

    static QMap<int, QMap<QString, int> > _fieldIndex;
};

QMap<int, QMap<QString, int> > MessageRemovalRecord::_fieldIndex;


static QString incrementAlias(const QString &alias)
{
    QRegExp aliasPattern("([a-z]+)([0-9]+)");
    if (aliasPattern.exactMatch(alias)) {
        return aliasPattern.cap(1) + QString::number(aliasPattern.cap(2).toInt() + 1);
    }

    return QString();
}

template<typename ArgumentListType>
QString buildOrderClause(const ArgumentListType &list, const QString &alias)
{
    if (list.isEmpty())
        return QString();

    QStringList sortColumns;
    foreach (typename ArgumentListType::const_reference arg, list) {
        QString field(fieldName(arg.property, alias));
        if (arg.mask) {
            field = QString("(%1 & %2)").arg(field).arg(QString::number(arg.mask));
        }
        if (caseInsensitiveProperty(arg.property)) {
	    sortColumns.append("ltrim(" + field + ",'\\\"') COLLATE NOCASE " + (arg.order == Qt::AscendingOrder ? "ASC" : "DESC"));
        } else {
	    sortColumns.append(field + " " + (arg.order == Qt::AscendingOrder ? "ASC" : "DESC"));
        }
    }

    return QString(" ORDER BY ") + sortColumns.join(",");
}


QString operatorString(QMailKey::Comparator op, bool multipleArgs = false, bool patternMatch = false, bool bitwiseMultiples = false)
{
    switch (op) 
    {
    case Equal:
        return (multipleArgs ? " IN " : (patternMatch ? " LIKE " : " = "));

    case NotEqual:
        return (multipleArgs ? " NOT IN " : (patternMatch ? " NOT LIKE " : " <> "));

    case LessThan:
        return " < ";

    case LessThanEqual:
        return " <= ";

    case GreaterThan:
        return " > ";

    case GreaterThanEqual:
        return " >= ";

    case Includes:
    case Present:
        return (multipleArgs ? " IN " : (bitwiseMultiples ? " & " : " LIKE "));

    case Excludes:
    case Absent:
        // Note: the result is not correct in the bitwiseMultiples case!
        return (multipleArgs ? " NOT IN " : (bitwiseMultiples ? " & " : " NOT LIKE "));
    }

    return QString();
}

QString combineOperatorString(QMailKey::Combiner op)
{
    switch (op) 
    {
    case And:
        return " AND ";

    case Or:
        return " OR ";

    case None:
        break;
    }

    return QString();
}

QString columnExpression(const QString &column, QMailKey::Comparator op, const QString &value, bool multipleArgs = false, bool patternMatch = false, bool bitwiseMultiples = false, bool noCase = false)
{
    QString result;

    QString operation(operatorString(op, multipleArgs, patternMatch, bitwiseMultiples));

    QString expression(column + operation);

    // Special case handling:
    if (bitwiseMultiples && (op == QMailKey::Excludes)) {
        if (!value.isEmpty()) {
            result = "0 = (" + expression + value + ")";
        } else {
            result = "0 = " + expression;
        }
    } else {
        result = expression + value;
    }

    if (noCase && !operation.contains("LIKE")) {
        // LIKE is already case-insensitive by default
        result.append(" COLLATE NOCASE");
    }

    return result;
}

QString columnExpression(const QString &column, QMailKey::Comparator op, const QVariantList &valueList, bool patternMatch = false, bool bitwiseMultiples = false, bool noCase = false)
{
    QString value(QMailStorePrivate::expandValueList(valueList)); 

    return columnExpression(column, op, value, (valueList.count() > 1), patternMatch, bitwiseMultiples, noCase);
}

QString baseExpression(const QString &column, QMailKey::Comparator op, bool multipleArgs = false, bool patternMatch = false, bool bitwiseMultiples = false, bool noCase = false)
{
    return columnExpression(column, op, QString(), multipleArgs, patternMatch, bitwiseMultiples, noCase);
}


template<typename Key>
QString whereClauseItem(const Key &key, const typename Key::ArgumentType &arg, const QString &alias, const QString &field, const QMailStorePrivate &store);

template<>
QString whereClauseItem<QMailAccountKey>(const QMailAccountKey &, const QMailAccountKey::ArgumentType &a, const QString &alias, const QString &field, const QMailStorePrivate &store)
{
    QString item;
    {
        QTextStream q(&item);

        QString columnName;
        if (!field.isEmpty()) {
            columnName = qualifiedName(field, alias);
        } else {
            columnName = fieldName(a.property, alias);
        }

        bool bitwise((a.property == QMailAccountKey::Status) || (a.property == QMailAccountKey::MessageType));
        bool patternMatching(a.property == QMailAccountKey::FromAddress);
        bool noCase(caseInsensitiveProperty(a.property));

        QString expression = columnExpression(columnName, a.op, a.valueList, patternMatching, bitwise, noCase);
#ifdef USE_ACCOUNTS_QT
        Q_UNUSED (store);
        // Only ID could be queried here,
        // All other properties will be queried
        // from Accounts subsystem
        Q_ASSERT(a.property == QMailAccountKey::Id);
        q << expression;
#else

        switch(a.property)
        {
        case QMailAccountKey::Id:
            if (a.valueList.first().canConvert<QMailAccountKey>()) {
                QMailAccountKey subKey = a.valueList.first().value<QMailAccountKey>();
                QString nestedAlias(incrementAlias(alias));

                // Expand comparison to sub-query result
                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailaccounts " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(subKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailAccountKey::Custom:
            // Match on custom field
            {
                QString nestedAlias(incrementAlias(alias));

                // Is this an existence test or a value test?
                if ((a.op == QMailKey::Present) || (a.op == QMailKey::Absent)) {
                    q << qualifiedName("id", alias) << operatorString(a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias);
                    q << " FROM mailaccountcustom " << nestedAlias << " WHERE name=? COLLATE NOCASE )";
                } else {
                    q << qualifiedName("id", alias) << " IN ( SELECT " << qualifiedName("id", nestedAlias); q << " FROM mailaccountcustom " << nestedAlias;
                    q << " WHERE " << qualifiedName("name", nestedAlias) << "=? COLLATE NOCASE AND " 
                      << qualifiedName("value", nestedAlias) << operatorString(a.op, false) << "? COLLATE NOCASE )";
                }
            }
            break;

        case QMailAccountKey::Status:
        case QMailAccountKey::MessageType:
        case QMailAccountKey::Name:
        case QMailAccountKey::FromAddress:
        case QMailAccountKey::LastSynchronized:
        case QMailAccountKey::IconPath:
            q << expression;
            break;
        }  
#endif
    }
    return item;
}

template<>
QString whereClauseItem<QMailMessageKey>(const QMailMessageKey &key, const QMailMessageKey::ArgumentType &a, const QString &alias, const QString &field, const QMailStorePrivate &store)
{
#ifndef USE_ACCOUNTS_QT
    Q_UNUSED(key);
#endif
    QString item;
    {
        QTextStream q(&item);

        QString columnName;
        if (!field.isEmpty()) {
            columnName = qualifiedName(field, alias);
        } else {
            columnName = fieldName(a.property, alias);
        }

        bool bitwise((a.property == QMailMessageKey::Type) || (a.property == QMailMessageKey::Status));
        bool patternMatching((a.property == QMailMessageKey::Sender) || (a.property == QMailMessageKey::Recipients) ||
                             (a.property == QMailMessageKey::ContentScheme) || (a.property == QMailMessageKey::ContentIdentifier));
        bool noCase(caseInsensitiveProperty(a.property));

        QString expression = columnExpression(columnName, a.op, a.valueList, patternMatching, bitwise, noCase);
        
        switch(a.property)
        {
        case QMailMessageKey::Id:
            if (a.valueList.count() >= IdLookupThreshold) {
                q << baseExpression(columnName, a.op, true) << "( SELECT id FROM " << QMailStorePrivate::temporaryTableName(a) << ")";
            } else {
                if (a.valueList.first().canConvert<QMailMessageKey>()) {
                    QMailMessageKey subKey = a.valueList.first().value<QMailMessageKey>();
                    QString nestedAlias(incrementAlias(alias));

                    // Expand comparison to sub-query result
                    q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailmessages " << nestedAlias;
                    q << store.buildWhereClause(QMailStorePrivate::Key(subKey, nestedAlias)) << ")";
                } else {
                    q << expression;
                }
            }
            break;

        case QMailMessageKey::ParentFolderId:
        case QMailMessageKey::PreviousParentFolderId:
        case QMailMessageKey::RestoreFolderId:
            if(a.valueList.first().canConvert<QMailFolderKey>()) {
                QMailFolderKey parentFolderKey = a.valueList.first().value<QMailFolderKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailfolders " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(parentFolderKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailMessageKey::ParentThreadId:
            if (a.valueList.first().canConvert<QMailThreadKey>()) {
                QMailThreadKey parentThreadKey = a.valueList.first().value<QMailThreadKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailthreads " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(parentThreadKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailMessageKey::AncestorFolderIds:
            if (a.valueList.first().canConvert<QMailFolderKey>()) {
                QMailFolderKey folderSubKey = a.valueList.first().value<QMailFolderKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(fieldName(QMailMessageKey::ParentFolderId, alias), a.op, true);
                q << "( SELECT DISTINCT descendantid FROM mailfolderlinks WHERE id IN ( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailfolders" << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(folderSubKey, nestedAlias)) << ") )";
            } else {
                q << baseExpression(fieldName(QMailMessageKey::ParentFolderId, alias), a.op, true) << "( SELECT DISTINCT descendantid FROM mailfolderlinks WHERE id";
                if (a.valueList.count() > 1) {
                    q << " IN " << QMailStorePrivate::expandValueList(a.valueList) << ")";
                } else {
                    q << "=? )";
                }
            }
            break;

        case QMailMessageKey::ParentAccountId:
            if(a.valueList.first().canConvert<QMailAccountKey>()) {
                QMailAccountKey parentAccountKey = a.valueList.first().value<QMailAccountKey>();
#ifdef USE_ACCOUNTS_QT
                QMailAccountIdList parentAccountIdList = store.searchSSOAccounts(parentAccountKey);

                // Rewrite argument and execute builder again
                QMailMessageKey::ArgumentType &arg = const_cast<QMailMessageKey::ArgumentType&>(a);
                arg = QMailMessageKey::parentAccountId(parentAccountIdList, QMailDataComparator::Includes).arguments().first();
                return whereClauseItem(key, arg, alias, field, store);
#else

                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailaccounts " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(parentAccountKey, nestedAlias)) << ")";
#endif
            } else {
                q << expression;
            }
            break;

        case QMailMessageKey::Custom:
            // Match on custom field
            {
                QString nestedAlias(incrementAlias(alias));

                // Is this an existence test or a value test?
                if ((a.op == QMailKey::Present) || (a.op == QMailKey::Absent)) {
                    q << qualifiedName("id", alias) << operatorString(a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias);
                    q << " FROM mailmessagecustom " << nestedAlias << " WHERE name=? COLLATE NOCASE )";
                } else {
                    q << qualifiedName("id", alias) << " IN ( SELECT " << qualifiedName("id", nestedAlias); q << " FROM mailmessagecustom " << nestedAlias;
                    q << " WHERE " << qualifiedName("name", nestedAlias) << "=? COLLATE NOCASE AND " 
                      << qualifiedName("value", nestedAlias) << operatorString(a.op, false) << "? COLLATE NOCASE )";
                }
            }
            break;

        case QMailMessageKey::InResponseTo:
            if (a.valueList.first().canConvert<QMailMessageKey>()) {
                QMailMessageKey messageKey = a.valueList.first().value<QMailMessageKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailmessages " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(messageKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailMessageKey::Conversation:
            if (a.valueList.first().canConvert<QMailMessageKey>()) {
                QMailMessageKey messageKey = a.valueList.first().value<QMailMessageKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("parentthreadid", nestedAlias) << " FROM mailmessages " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(messageKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;
        case QMailMessageKey::ServerUid:
        case QMailMessageKey::CopyServerUid:
            if (a.valueList.count() >= IdLookupThreshold) {
                q << baseExpression(columnName, a.op, true) << "( SELECT id FROM " << QMailStorePrivate::temporaryTableName(a) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailMessageKey::Type:
        case QMailMessageKey::Status:
        case QMailMessageKey::Sender:
        case QMailMessageKey::Recipients:
        case QMailMessageKey::Subject:
        case QMailMessageKey::TimeStamp:
        case QMailMessageKey::ReceptionTimeStamp:
        case QMailMessageKey::Size:
        case QMailMessageKey::ContentType:
        case QMailMessageKey::ContentScheme:
        case QMailMessageKey::ContentIdentifier:
        case QMailMessageKey::ResponseType:
        case QMailMessageKey::ListId:
        case QMailMessageKey::RfcId:
        case QMailMessageKey::Preview:
            q << expression;
            break;
        }
    }
    return item;
}

template<>
QString whereClauseItem<QMailFolderKey>(const QMailFolderKey &key, const QMailFolderKey::ArgumentType &a, const QString &alias, const QString &field, const QMailStorePrivate &store)
{
#ifndef USE_ACCOUNTS_QT
    Q_UNUSED(key);
#endif
    QString item;
    {
        QTextStream q(&item);

        QString columnName;
        if (!field.isEmpty()) {
            columnName = qualifiedName(field, alias);
        } else {
            columnName = fieldName(a.property, alias);
        }

        bool bitwise(a.property == QMailFolderKey::Status);
        bool noCase(caseInsensitiveProperty(a.property));

        QString expression = columnExpression(columnName, a.op, a.valueList, false, bitwise, noCase);
        
        switch (a.property)
        {
        case QMailFolderKey::Id:
            if (a.valueList.first().canConvert<QMailFolderKey>()) {
                QMailFolderKey subKey = a.valueList.first().value<QMailFolderKey>();
                QString nestedAlias(incrementAlias(alias));

                // Expand comparison to sub-query result
                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailfolders " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(subKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailFolderKey::ParentFolderId:
            if(a.valueList.first().canConvert<QMailFolderKey>()) {
                QMailFolderKey folderSubKey = a.valueList.first().value<QMailFolderKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailfolders " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(folderSubKey, nestedAlias)) << ")";                
            } else {
                q << expression;
            }
            break;

        case QMailFolderKey::AncestorFolderIds:
            if (a.valueList.first().canConvert<QMailFolderKey>()) {
                QMailFolderKey folderSubKey = a.valueList.first().value<QMailFolderKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(fieldName(QMailFolderKey::Id, alias), a.op, true);
                q << "( SELECT DISTINCT descendantid FROM mailfolderlinks WHERE id IN ( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailfolders" << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(folderSubKey, nestedAlias)) << ") )";
            } else {
                q << baseExpression(fieldName(QMailFolderKey::Id, alias), a.op, true) << "( SELECT DISTINCT descendantid FROM mailfolderlinks WHERE id";
                if (a.valueList.count() > 1) {
                    q << " IN " << QMailStorePrivate::expandValueList(a.valueList) << ")";
                } else {
                    q << "=? )";
                }
            }
            break;

        case QMailFolderKey::ParentAccountId:
            if(a.valueList.first().canConvert<QMailAccountKey>()) {
                QMailAccountKey accountSubKey = a.valueList.first().value<QMailAccountKey>();
#ifdef USE_ACCOUNTS_QT
                QMailAccountIdList acountIdList = store.searchSSOAccounts(accountSubKey);

                // Rewrite argument and execute builder again
                QMailFolderKey::ArgumentType &arg = const_cast<QMailFolderKey::ArgumentType&>(a);
                arg = QMailFolderKey::parentAccountId(acountIdList, QMailDataComparator::Includes).arguments().first();
                return whereClauseItem(key, arg, alias, field, store);
#else
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailaccounts " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(accountSubKey, nestedAlias)) << ")";
#endif
            } else {
                q << expression;
            }
            break;

        case QMailFolderKey::Custom:
            // Match on custom field
            {
                QString nestedAlias(incrementAlias(alias));

                // Is this an existence test or a value test?
                if ((a.op == QMailKey::Present) || (a.op == QMailKey::Absent)) {
                    q << qualifiedName("id", alias) << operatorString(a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias);
                    q << " FROM mailfoldercustom " << nestedAlias << " WHERE name=? COLLATE NOCASE )";
                } else {
                    q << qualifiedName("id", alias) << " IN ( SELECT " << qualifiedName("id", nestedAlias); q << " FROM mailfoldercustom " << nestedAlias;
                    q << " WHERE " << qualifiedName("name", nestedAlias) << "=? COLLATE NOCASE AND " 
                      << qualifiedName("value", nestedAlias) << operatorString(a.op, false) << "? COLLATE NOCASE )";
                }
            }
            break;

        case QMailFolderKey::Status:
        case QMailFolderKey::Path:
        case QMailFolderKey::DisplayName:
        case QMailFolderKey::ServerCount:
        case QMailFolderKey::ServerUnreadCount:
        case QMailFolderKey::ServerUndiscoveredCount:

            q << expression;
            break;
        }
    }
    return item;
}


template<>
QString whereClauseItem<QMailThreadKey>(const QMailThreadKey &, const QMailThreadKey::ArgumentType &a, const QString &alias, const QString &field, const QMailStorePrivate &store)
{
    QString item;
    {
        QTextStream q(&item);

        QString columnName;
        if (!field.isEmpty()) {
            columnName = qualifiedName(field, alias);
        } else {
            columnName = fieldName(a.property, alias);
        }

        bool noCase(caseInsensitiveProperty(a.property));

        QString expression = columnExpression(columnName, a.op, a.valueList, false, false, noCase);

        switch (a.property)
        {
        case QMailThreadKey::Id:
            if (a.valueList.first().canConvert<QMailThreadKey>()) {
                QMailThreadKey subKey = a.valueList.first().value<QMailThreadKey>();
                QString nestedAlias(incrementAlias(alias));

                // Expand comparison to sub-query result
                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailthreads " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(subKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailThreadKey::Includes:
            if(a.valueList.first().canConvert<QMailMessageKey>()) {
                QMailMessageKey messageSubKey = a.valueList.first().value<QMailMessageKey>();
                QString nestedAlias(incrementAlias(alias));

                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("parentthreadid", nestedAlias) << " FROM mailmessages " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(messageSubKey, nestedAlias)) << ")";
            } else {
                Q_ASSERT(false);
                q << expression;
            }
            break;

        case QMailThreadKey::ParentAccountId:
            if (a.valueList.first().canConvert<QMailAccountKey>()) {
                QMailAccountKey subKey = a.valueList.first().value<QMailAccountKey>();
                QString nestedAlias(incrementAlias(alias));

                // Expand comparison to sub-query result
                q << baseExpression(columnName, a.op, true) << "( SELECT " << qualifiedName("id", nestedAlias) << " FROM mailaccounts " << nestedAlias;
                q << store.buildWhereClause(QMailStorePrivate::Key(subKey, nestedAlias)) << ")";
            } else {
                q << expression;
            }
            break;

        case QMailThreadKey::Custom:
            Q_ASSERT(false);
        case QMailThreadKey::ServerUid:
        case QMailThreadKey::MessageCount:
        case QMailThreadKey::UnreadCount:
            q << expression;
            break;

        case QMailThreadKey::Subject:
        case QMailThreadKey::Senders:
        case QMailThreadKey::Preview:
            Q_ASSERT (false);
            break;

        case QMailThreadKey::LastDate:
            q << expression;
            break;

        case QMailThreadKey::StartedDate:
        case QMailThreadKey::Status:
            Q_ASSERT (false);
            break;
        }
    }
    return item;
}

template<typename KeyType, typename ArgumentListType, typename KeyListType, typename CombineType>
QString buildWhereClause(const KeyType &key, 
                         const ArgumentListType &args, 
                         const KeyListType &subKeys, 
                         CombineType combine, 
                         bool negated, 
                         bool nested,
                         bool firstClause,
                         const QString &alias, 
                         const QString &field, 
                         const QMailStorePrivate& store)
{
    QString whereClause;
    QString logicalOpString(combineOperatorString(combine));

    if (!key.isEmpty()) {
        QTextStream s(&whereClause);

        QString op(' ');
        foreach (typename ArgumentListType::const_reference a, args) {
            s << op << whereClauseItem(key, a, alias, field, store);
            op = logicalOpString;
        }

        // subkeys
        s.flush();
        if (whereClause.isEmpty())
            op = ' ';

        foreach (typename KeyListType::const_reference subkey, subKeys) {
            QString nestedWhere(store.buildWhereClause(QMailStorePrivate::Key(subkey, alias), true));
            if (!nestedWhere.isEmpty()) 
                s << op << " (" << nestedWhere << ") ";

            op = logicalOpString;
        }       
    }       

    // Finalise the where clause
    if (!whereClause.isEmpty()) {
        if (negated) {
            whereClause = " NOT (" + whereClause + ")";
        }
        if (!nested) {
            whereClause.prepend(firstClause ? " WHERE " : " AND ");
        }
    }

    return whereClause;
}

QPair<QString, qint64> tableInfo(const QString &name, qint64 version)
{
    return qMakePair(name, version);
}

QMailContentManager::DurabilityRequirement durability(bool commitOnSuccess)
{
    return (commitOnSuccess ? QMailContentManager::EnsureDurability : QMailContentManager::DeferDurability);
}
#ifdef USE_ACCOUNTS_QT
// Forward declaration
bool SSOAccountSatisfyTheKey(Accounts::Account* ssoAccount, const QMailAccountKey& key);

template <typename Property>
bool SSOAccountCompareProperty(Accounts::Account* ssoAccount, Property value, QMailKey::Comparator op, const QMailAccountKey::ArgumentType::ValueList& arguments)
{
    // Argument list should not be empty.
    // Otherwise we have nothing to compare.
    Q_ASSERT(arguments.count());

    if (arguments.count() == 1) {
        if (!arguments.front().canConvert<Property>()) {
            if (arguments.front().canConvert<QMailAccountKey>()) {
                QMailAccountKey accountKey = arguments.front().value<QMailAccountKey>();
                return SSOAccountSatisfyTheKey(ssoAccount, accountKey);
            }

            qMailLog(Messaging) << "Failed to convert argument";
            return false;
        }

        Property argument = arguments.front().value<Property>();
        switch (op) {
            case QMailKey::Equal:
                return value == argument;

            case QMailKey::NotEqual:
                return value != argument;

            case QMailKey::Includes:
            case QMailKey::Present:
            case QMailKey::Excludes:
            case QMailKey::Absent:
            case QMailKey::LessThan:
            case QMailKey::LessThanEqual:
            case QMailKey::GreaterThan:
            case QMailKey::GreaterThanEqual:
            default:
                qMailLog(Messaging) << "This comparator is not supported";
                Q_ASSERT(false);
                break;
        }

    } else {
        switch (op) {
            case QMailKey::Includes:
            case QMailKey::Present:
                foreach (const QVariant& argument, arguments) {
                    if (argument.canConvert<Property>() && argument.value<Property>() == value)
                        return true;
                }
                return false;

            case QMailKey::Excludes:
            case QMailKey::Absent:
                foreach (const QVariant& argument, arguments) {
                    if (argument.canConvert<Property>() && argument.value<Property>() == value)
                        return false;
                }
                return true;


            case QMailKey::LessThan:
            case QMailKey::LessThanEqual:
            case QMailKey::GreaterThan:
            case QMailKey::GreaterThanEqual:
            case QMailKey::Equal:
            case QMailKey::NotEqual:
            default:
                qMailLog(Messaging) << "This comparator is not supported";
                Q_ASSERT(false);
                break;
        }

    }

    Q_ASSERT(false);
    return false;
}

template <>
bool SSOAccountCompareProperty(Accounts::Account*, quint64 value, QMailKey::Comparator op, const QMailAccountKey::ArgumentType::ValueList& arguments)
{
    // Argument list should not be empty.
    // Otherwise we have nothing to compare.
    Q_ASSERT(arguments.count());

    if (arguments.count() == 1) {
        bool ok = false;
        quint64 argument = arguments.front().toULongLong(&ok);
        if (!ok) {
            qMailLog(Messaging) << "Failed to convert to quing64";
            return false;
        }

        switch (op) {
            case QMailKey::LessThan:
                return value < argument;

            case QMailKey::LessThanEqual:
                return value <= argument;

            case QMailKey::GreaterThan:
                return value > argument;

            case QMailKey::GreaterThanEqual:
                return value >= argument;

            case QMailKey::Equal:
                return value == argument;

            case QMailKey::NotEqual:
                return value != argument;

            case QMailKey::Includes:
            case QMailKey::Present:
                return ((value & argument) == argument);

            case QMailKey::Excludes:
            case QMailKey::Absent:
                return !(value & argument);

            default:
                Q_ASSERT(false);
                break;
        }

    } else {

        switch (op) {
            case QMailKey::LessThan:
            case QMailKey::LessThanEqual:
            case QMailKey::GreaterThan:
            case QMailKey::GreaterThanEqual:
            case QMailKey::Equal:
            case QMailKey::NotEqual:
                // This comparator is not supported for multiple integer arguments
                Q_ASSERT(false);
                break;

            case QMailKey::Includes:
            case QMailKey::Present:
                foreach (const QVariant& argument, arguments) {
                    if (value == argument.toULongLong())
                        return true;
                }
                return false;

            case QMailKey::Excludes:
            case QMailKey::Absent:
                foreach (const QVariant& argument, arguments) {
                    if (value == argument.toULongLong())
                        return false;
                }
                return true;

            default:
                Q_ASSERT(false);
                break;
        }

    }
    Q_ASSERT(false);
    return false;
}

template <>
bool SSOAccountCompareProperty(Accounts::Account*, const QString& value, QMailKey::Comparator op, const QMailAccountKey::ArgumentType::ValueList& arguments)
{
    // Argument list should not be empty.
    // Otherwise we have nothing to compare.
    Q_ASSERT(arguments.count());

    if (arguments.count() == 1) {
        if (!arguments.front().canConvert<QString>()) {
            qMailLog(Messaging) << "Failed to convert to string";
            return false;
        }

        QString argument = arguments.front().toString();
        switch (op) {
            case QMailKey::LessThan:
                return value < argument;

            case QMailKey::LessThanEqual:
                return value <= argument;

            case QMailKey::GreaterThan:
                return value > argument;

            case QMailKey::GreaterThanEqual:
                return value >= argument;

            case QMailKey::Equal:
                return value == argument;

            case QMailKey::NotEqual:
                return value != argument;

            case QMailKey::Includes:
            case QMailKey::Present:
                return value.contains(argument);

            case QMailKey::Excludes:
            case QMailKey::Absent:
                return !value.contains(argument);

            default:
                Q_ASSERT(false);
                break;
        }

    } else {

        switch (op) {
            case QMailKey::LessThan:
            case QMailKey::LessThanEqual:
            case QMailKey::GreaterThan:
            case QMailKey::GreaterThanEqual:
            case QMailKey::Equal:
            case QMailKey::NotEqual:
                // This comparator is not supported for multiple integer arguments
                Q_ASSERT(false);
                break;

            case QMailKey::Includes:
            case QMailKey::Present:
                foreach (const QVariant& argument, arguments) {
                    if (value == argument.toString())
                        return true;
                }
                return false;

            case QMailKey::Excludes:
            case QMailKey::Absent:
                foreach (const QVariant& argument, arguments) {
                    if (value == argument.toString())
                        return false;
                }
                return true;

            default:
                Q_ASSERT(false);
                break;
        }

    }
    Q_ASSERT(false);
    return false;
}

bool SSOAccountCompareProperty(Accounts::Account* ssoAccount, QMailKey::Comparator op, const QMailAccountKey::ArgumentType::ValueList& arguments)
{
    // Argument list should not be empty.
    // Otherwise we have nothing to compare.
    Q_ASSERT(arguments.count() == 1);

    QStringList argument = arguments.front().toStringList();

    QString key   = argument.front();
    QString value = argument.count() == 2 ? argument.back() : QString();

    ssoAccount->beginGroup("customFields");

    bool result = false;
    switch (op) {
        case QMailKey::LessThan:
        case QMailKey::LessThanEqual:
        case QMailKey::GreaterThan:
        case QMailKey::GreaterThanEqual:
            // This comparator is not supported for custom fields
            Q_ASSERT(false);
            break;

        case QMailKey::Equal:
            result = ssoAccount->contains(key) && (ssoAccount->valueAsString(key) == value);
            break;

        case QMailKey::NotEqual:
            result = !ssoAccount->contains(key) || (ssoAccount->valueAsString(key) != value);
            break;

        case QMailKey::Includes:
        case QMailKey::Present:
            result = ssoAccount->contains(key) &&  ssoAccount->valueAsString(key).contains(value);
            break;

        case QMailKey::Excludes:
        case QMailKey::Absent:
            result = !(ssoAccount->contains(key) && ssoAccount->valueAsString(key).contains(value));
            break;

        default:
            Q_ASSERT(false);
            break;
    }
    ssoAccount->endGroup();
    return result;
}

bool SSOAccountSatisfyTheProperty(Accounts::Account* ssoAccount, const QMailAccountKey::ArgumentType& argument)
{
    Q_ASSERT(ssoAccount);

    switch (argument.property) {
        case QMailAccountKey::Id:
            return SSOAccountCompareProperty<QMailAccountId>(ssoAccount, QMailAccountId(ssoAccount->id()), argument.op, argument.valueList);

        case QMailAccountKey::Name:
            return SSOAccountCompareProperty<const QString&>(ssoAccount, ssoAccount->displayName(), argument.op, argument.valueList);

        case QMailAccountKey::MessageType:
            return SSOAccountCompareProperty<quint64>(ssoAccount, ssoAccount->valueAsInt("type"), argument.op, argument.valueList);

        case QMailAccountKey::FromAddress:
            return SSOAccountCompareProperty<const QString&>(ssoAccount, QMailAddress(ssoAccount->valueAsString("emailaddress")).address(), argument.op, argument.valueList);

        case QMailAccountKey::Status: {
            Accounts::Service service = ssoAccount->selectedService();

            ssoAccount->selectService();
            const bool& enabled = ssoAccount->enabled();
            ssoAccount->selectService(service);

            quint64 status = ssoAccount->valueAsUInt64("status");
            status &= (~QMailAccount::Enabled);
            status |= enabled?(QMailAccount::Enabled):0;

            bool canTransmit = ssoAccount->valueAsBool("canTransmit", true);
            status &= (~QMailAccount::CanTransmit);
            status |= canTransmit?(QMailAccount::CanTransmit):0;

            bool appendSignature = ssoAccount->valueAsBool("signatureEnabled", true);
            status &= (~QMailAccount::AppendSignature);
            status |= appendSignature?(QMailAccount::AppendSignature):0;

            bool hasPersistentConnection = ssoAccount->valueAsBool("hasPersistentConnection", false);
            status &= (~QMailAccount::HasPersistentConnection);
            status |= hasPersistentConnection?(QMailAccount::HasPersistentConnection):0;

            return SSOAccountCompareProperty<quint64>(ssoAccount,
                                                      status,
                                                      argument.op, argument.valueList);
        }

        case QMailAccountKey::Custom:
            return SSOAccountCompareProperty(ssoAccount, argument.op, argument.valueList);

        default:
            Q_ASSERT(false);
            break;
    }
    return false;
}

bool SSOAccountSatisfyTheKey(Accounts::Account* ssoAccount, const QMailAccountKey& key)
{
    Q_ASSERT(ssoAccount);

    if (key.isNonMatching())
        return false;

    if (key.isEmpty())
        return true;

    // In case of it is not compound key and has got a list of arguments
    // follow the list of arguments and compare
    if (!key.arguments().isEmpty()) {
        typedef QList<QMailAccountKey::ArgumentType> ListOfArguments;
        ListOfArguments::const_iterator it = key.arguments().begin();

        bool result = SSOAccountSatisfyTheProperty(ssoAccount, *it);
        while (++it != key.arguments().end()) {
            switch (key.combiner()) {
                case QMailKey::And:
                    result = result && SSOAccountSatisfyTheProperty(ssoAccount, *it);
                    break;
                case QMailKey::Or:
                    result = result || SSOAccountSatisfyTheProperty(ssoAccount, *it);
                    break;
                default:
                    Q_ASSERT(false);
                    break;
            }
        }

        // Return negated value if key was negeated
        return key.isNegated() ? !result : result;
    }

    // In case of compound key, process each subkey separatelly
    if (!key.subKeys().isEmpty()) {
        typedef QList<QMailAccountKey> ListOfKeys;
        ListOfKeys::const_iterator it = key.subKeys().begin();

        bool result = SSOAccountSatisfyTheKey(ssoAccount, *it);
        while (++it != key.subKeys().end()) {
            switch (key.combiner()) {
                case QMailKey::And:
                    result = result && SSOAccountSatisfyTheKey(ssoAccount, *it);
                    break;
                case QMailKey::Or:
                    result = result || SSOAccountSatisfyTheKey(ssoAccount, *it);
                    break;
                default:
                    Q_ASSERT(false);
                    break;
            }
        }

        // Return negated value if key was negeated
        return key.isNegated() ? !result : result;
    }

    // This key is not empty and has neither subkeys nor arguments.
    Q_ASSERT(false);
    return false;
}

void SSOHandleError(const Accounts::Error& error)
{
    switch (error.type()) {
    case Accounts::Error::NoError:
        break;
    case Accounts::Error::Deleted:
    case Accounts::Error::AccountNotFound:
        qWarning() << "Accounts:" << error.message();
        break;
    case Accounts::Error::Unknown:
    case Accounts::Error::Database:
    case Accounts::Error::DatabaseLocked:
        qCritical() << "Accounts:" << error.message();
        Q_ASSERT (false);
        break;
    default:
        Q_ASSERT (false);
    }
}
#endif

} // namespace


// We need to support recursive locking, per-process
static volatile int mutexLockCount = 0;


class QMailStorePrivate::Transaction
{
    QMailStorePrivate *m_d;
    bool m_initted;
    bool m_committed;

public:
    Transaction(QMailStorePrivate *);
    ~Transaction();

    bool commit();

    bool committed() const;
};

QMailStorePrivate::Transaction::Transaction(QMailStorePrivate* d)
    : m_d(d), 
      m_initted(false),
      m_committed(false)
{
    if (mutexLockCount > 0) {
        // Increase lock recursion depth
        ++mutexLockCount;
        m_initted = true;
    } else {
        // This process does not yet have a mutex lock
        m_d->databaseMutex().lock();
        if (m_d->transaction()) {
            ++mutexLockCount;
            m_initted = true;
        } else {
            m_d->databaseMutex().unlock();
        }
    }
}

QMailStorePrivate::Transaction::~Transaction()
{
    if (m_initted && !m_committed) {
        m_d->rollback();

        --mutexLockCount;
        if (mutexLockCount == 0)
            m_d->databaseMutex().unlock();
    }
}

bool QMailStorePrivate::Transaction::commit()
{
    if (m_initted && !m_committed) {
        m_committed = m_d->commit();
        if (m_committed) {
            --mutexLockCount;
            if (mutexLockCount == 0)
                m_d->databaseMutex().unlock();
        }
    }

    return m_committed;
}

bool QMailStorePrivate::Transaction::committed() const
{
    return m_committed;
}


struct QMailStorePrivate::ReadLock
{
    ReadLock(QMailStorePrivate *){};
};


template<typename FunctionType>
QMailStorePrivate::AttemptResult evaluate(QMailStorePrivate::WriteAccess, FunctionType func, QMailStorePrivate::Transaction &t)
{
    // Use the supplied transaction, and do not commit
    return func(t, false);
}

template<typename FunctionType>
QMailStorePrivate::AttemptResult evaluate(QMailStorePrivate::ReadAccess, FunctionType, QMailStorePrivate::Transaction &)
{
    return QMailStorePrivate::Failure;
}

template<typename FunctionType>
QMailStorePrivate::AttemptResult evaluate(QMailStorePrivate::WriteAccess, FunctionType func, const QString& description, QMailStorePrivate* d)
{
    QMailStorePrivate::Transaction t(d);

    // Perform the task and commit the transaction
    QMailStorePrivate::AttemptResult result = func(t, true);

    // Ensure that the transaction was committed
    if ((result == QMailStorePrivate::Success) && !t.committed()) {
        qWarning() << pid << "Failed to commit successful" << qPrintable(description) << "!";
    }

    return result;
}

template<typename FunctionType>
QMailStorePrivate::AttemptResult evaluate(QMailStorePrivate::ReadAccess, FunctionType func, const QString&, QMailStorePrivate* d)
{
    QMailStorePrivate::ReadLock l(d);

    return func(l);
}


QMailStore::ErrorCode errorType(QMailStorePrivate::ReadAccess)
{
    return QMailStore::InvalidId;
}

QMailStore::ErrorCode errorType(QMailStorePrivate::WriteAccess)
{
    return QMailStore::ConstraintFailure;
}


const QMailMessageKey::Properties &QMailStorePrivate::updatableMessageProperties()
{
    static QMailMessageKey::Properties p = QMailMessageKey::ParentFolderId |
                                           QMailMessageKey::Type |
                                           QMailMessageKey::Sender |
                                           QMailMessageKey::Recipients |
                                           QMailMessageKey::Subject |
                                           QMailMessageKey::Status |
                                           QMailMessageKey::ParentAccountId |
                                           QMailMessageKey::ServerUid |
                                           QMailMessageKey::Size |
                                           QMailMessageKey::ContentType |
                                           QMailMessageKey::PreviousParentFolderId |
                                           QMailMessageKey::ContentScheme |
                                           QMailMessageKey::ContentIdentifier |
                                           QMailMessageKey::InResponseTo |
                                           QMailMessageKey::ResponseType |
                                           QMailMessageKey::CopyServerUid |
                                           QMailMessageKey::RestoreFolderId |
                                           QMailMessageKey::ListId |
                                           QMailMessageKey::RfcId |
                                           QMailMessageKey::Preview |
                                           QMailMessageKey::ParentThreadId;
    return p;
}

const QMailMessageKey::Properties &QMailStorePrivate::allMessageProperties()
{
    static QMailMessageKey::Properties p = QMailMessageKey::Id
                                           | QMailMessageKey::AncestorFolderIds
                                           | QMailMessageKey::TimeStamp             //were moved to here, since we use UTC format for keeping messages
                                           | QMailMessageKey::ReceptionTimeStamp    //so it doesn't need to update the time stamps.
                                           | updatableMessageProperties();
    return p;
}

const QMailStorePrivate::MessagePropertyMap& QMailStorePrivate::messagePropertyMap() 
{
    static const MessagePropertyMap map(::messagePropertyMap());
    return map;
}

const QMailStorePrivate::MessagePropertyList& QMailStorePrivate::messagePropertyList() 
{
    static const MessagePropertyList list(messagePropertyMap().keys());
    return list;
}

const QMailStorePrivate::ThreadPropertyMap& QMailStorePrivate::threadPropertyMap()
{
    static const ThreadPropertyMap map(::threadPropertyMap());
    return map;
}

const QMailStorePrivate::ThreadPropertyList& QMailStorePrivate::threadPropertyList()
{
    static const ThreadPropertyList list(threadPropertyMap().keys());
    return list;
}

const QString &QMailStorePrivate::defaultContentScheme()
{
    static QString scheme(QMailContentManagerFactory::defaultScheme());
    return scheme;
}

QString QMailStorePrivate::databaseIdentifier() const
{
    return database()->databaseName();
}

QSqlDatabase *QMailStorePrivate::database() const
{
    if (!databaseptr) {
        databaseptr = new QSqlDatabase(QMail::createDatabase());
    }
    databaseUnloadTimer.start(QMail::databaseAutoCloseTimeout());
    return databaseptr;
}

ProcessMutex* QMailStorePrivate::contentMutex = 0;

QMailStorePrivate::QMailStorePrivate(QMailStore* parent)
    : QMailStoreImplementation(parent),
      q_ptr(parent),
      databaseptr(0),
      messageCache(messageCacheSize),
      uidCache(uidCacheSize),
      folderCache(folderCacheSize),
      accountCache(accountCacheSize),
      threadCache(threadCacheSize),
      inTransaction(false),
      lastQueryError(0),
      mutex(0),
      globalLocks(0)
{
    ProcessMutex creationMutex(QDir::rootPath());
    MutexGuard guard(creationMutex);
    guard.lock();

    mutex = new ProcessMutex(databaseIdentifier(), 1);
    if (contentMutex == 0) {
        contentMutex = new ProcessMutex(databaseIdentifier(), 3);
    }
    connect(&databaseUnloadTimer, SIGNAL(timeout()), this, SLOT(unloadDatabase()));
#ifdef USE_ACCOUNTS_QT
    // manager to notify QMailStore about the changes
    connect(manager, SIGNAL(accountCreated(Accounts::AccountId)), this, SLOT(accountCreated(Accounts::AccountId)));
    connect(manager, SIGNAL(accountRemoved(Accounts::AccountId)), this, SLOT(accountRemoved(Accounts::AccountId)));
    connect(manager, SIGNAL(accountUpdated(Accounts::AccountId)), this, SLOT(accountUpdated(Accounts::AccountId)));
#endif
}

#ifdef USE_ACCOUNTS_QT
QSharedPointer<Accounts::Account> QMailStorePrivate::getEmailAccount(const Accounts::AccountId id)
{
    //get account from the manager
    QSharedPointer<Accounts::Account> ssoAccount(Accounts::Account::fromId(manager, id, this));

    if (!ssoAccount) {
        qWarning() << Q_FUNC_INFO << "Account with was not found" ;
        SSOHandleError(manager->lastError());
        return ssoAccount;
    }

    // check if it is an e-mail account
    Accounts::ServiceList services = ssoAccount->enabledServices();
    if (!services.count()) {
        qWarning() << Q_FUNC_INFO << "E-mail Services not found, make sure that *.service and *.provider files are properly installed and e-mail services are enabled.";
        ssoAccount = QSharedPointer<Accounts::Account>();
    }

    return ssoAccount;
}
#endif

QMailStorePrivate::~QMailStorePrivate()
{
    delete mutex;
    delete databaseptr;
}

ProcessMutex& QMailStorePrivate::databaseMutex(void) const
{
    return *mutex;
}

ProcessMutex& QMailStorePrivate::contentManagerMutex(void)
{
    return *contentMutex;
}

bool QMailStorePrivate::initStore()
{
    ProcessMutex creationMutex(QDir::rootPath());
    MutexGuard guard(creationMutex);
    guard.lock();

    if (!database()->isOpen()) {
        qWarning() << "Unable to open database in initStore!";
        return false;
    }

    {
        Transaction t(this);

        if (!ensureVersionInfo() ||
            !setupTables(QList<TableInfo>() << tableInfo("maintenancerecord", 100)
                                            << tableInfo("mailaccounts", 108)
                                            << tableInfo("mailaccountcustom", 100)
                                            << tableInfo("mailaccountconfig", 100)
                                            << tableInfo("mailaccountfolders", 100)
                                            << tableInfo("mailfolders", 106)
                                            << tableInfo("mailfoldercustom", 100)
                                            << tableInfo("mailfolderlinks", 100)
                                            << tableInfo("mailthreads", 102)
                                            << tableInfo("mailmessages", 114)
                                            << tableInfo("mailmessagecustom", 101)
                                            << tableInfo("mailstatusflags", 101)
                                            << tableInfo("mailmessageidentifiers", 101)
                                            << tableInfo("mailsubjects", 100)
                                            << tableInfo("mailthreadsubjects", 100)
                                            << tableInfo("missingancestors", 101)
                                            << tableInfo("missingmessages", 101)
                                            << tableInfo("deletedmessages", 101)
                                            << tableInfo("obsoletefiles", 100))) {
            return false;
        }
        /*static_*/Q_ASSERT(Success == 0);

        bool res = attemptRegisterStatusBit("SynchronizationEnabled", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::SynchronizationEnabled), t, false)
                || attemptRegisterStatusBit("Synchronized", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::Synchronized), t, false)
                || attemptRegisterStatusBit("AppendSignature", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::AppendSignature), t, false)
                || attemptRegisterStatusBit("UserEditable", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::UserEditable), t, false)
                || attemptRegisterStatusBit("UserRemovable", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::UserRemovable), t, false)
                || attemptRegisterStatusBit("PreferredSender", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::PreferredSender), t, false)
                || attemptRegisterStatusBit("MessageSource", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::MessageSource), t, false)
                || attemptRegisterStatusBit("CanRetrieve", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::CanRetrieve), t, false)
                || attemptRegisterStatusBit("MessageSink", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::MessageSink), t, false)
                || attemptRegisterStatusBit("CanTransmit", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::CanTransmit), t, false)
                || attemptRegisterStatusBit("Enabled", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::Enabled), t, false)
                || attemptRegisterStatusBit("CanReferenceExternalData", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::CanReferenceExternalData), t, false)
                || attemptRegisterStatusBit("CanTransmitViaReference", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::CanTransmitViaReference), t, false)
                || attemptRegisterStatusBit("CanCreateFolders", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::CanCreateFolders), t, false)
                || attemptRegisterStatusBit("UseSmartReply", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::UseSmartReply), t, false)
                || attemptRegisterStatusBit("CanSearchOnServer", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::CanSearchOnServer), t, false)
                || attemptRegisterStatusBit("HasPersistentConnection", "accountstatus", 63, true, const_cast<quint64 *>(&QMailAccount::HasPersistentConnection), t, false)
                || attemptRegisterStatusBit("SynchronizationEnabled", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::SynchronizationEnabled), t, false)
                || attemptRegisterStatusBit("Synchronized", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Synchronized), t, false)
                || attemptRegisterStatusBit("PartialContent", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::PartialContent), t, false)
                || attemptRegisterStatusBit("Removed", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Removed), t, false)
                || attemptRegisterStatusBit("Incoming", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Incoming), t, false)
                || attemptRegisterStatusBit("Outgoing", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Outgoing), t, false)
                || attemptRegisterStatusBit("Sent", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Sent), t, false)
                || attemptRegisterStatusBit("Trash", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Trash), t, false)
                || attemptRegisterStatusBit("Drafts", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Drafts), t, false)
                || attemptRegisterStatusBit("Junk", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Junk), t, false)
                || attemptRegisterStatusBit("ChildCreationPermitted", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::ChildCreationPermitted), t, false)
                || attemptRegisterStatusBit("RenamePermitted", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::RenamePermitted), t, false)
                || attemptRegisterStatusBit("DeletionPermitted", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::DeletionPermitted), t, false)
                || attemptRegisterStatusBit("NonMail", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::NonMail), t, false)
                || attemptRegisterStatusBit("MessagesPermitted", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::MessagesPermitted), t, false)
                || attemptRegisterStatusBit("ReadOnly", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::ReadOnly), t, false)
                || attemptRegisterStatusBit("Favourite", "folderstatus", 63, true, const_cast<quint64 *>(&QMailFolder::Favourite), t, false)
                || attemptRegisterStatusBit("Incoming", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Incoming), t, false)
                || attemptRegisterStatusBit("Outgoing", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Outgoing), t, false)
                || attemptRegisterStatusBit("Sent", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Sent), t, false)
                || attemptRegisterStatusBit("Replied", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Replied), t, false)
                || attemptRegisterStatusBit("RepliedAll", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::RepliedAll), t, false)
                || attemptRegisterStatusBit("Forwarded", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Forwarded), t, false)
                || attemptRegisterStatusBit("ContentAvailable", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::ContentAvailable), t, false)
                || attemptRegisterStatusBit("Read", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Read), t, false)
                || attemptRegisterStatusBit("Removed", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Removed), t, false)
                || attemptRegisterStatusBit("ReadElsewhere", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::ReadElsewhere), t, false)
                || attemptRegisterStatusBit("UnloadedData", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::UnloadedData), t, false)
                || attemptRegisterStatusBit("New", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::New), t, false)
                || attemptRegisterStatusBit("ReadReplyRequested", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::ReadReplyRequested), t, false)
                || attemptRegisterStatusBit("Trash", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Trash), t, false)
                || attemptRegisterStatusBit("PartialContentAvailable", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::PartialContentAvailable), t, false)
                || attemptRegisterStatusBit("HasAttachments", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::HasAttachments), t, false)
                || attemptRegisterStatusBit("HasReferences", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::HasReferences), t, false)
                || attemptRegisterStatusBit("HasUnresolvedReferences", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::HasUnresolvedReferences), t, false)
                || attemptRegisterStatusBit("Draft", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Draft), t, false)
                || attemptRegisterStatusBit("Outbox", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Outbox), t, false)
                || attemptRegisterStatusBit("Junk", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Junk), t, false)
                || attemptRegisterStatusBit("TransmitFromExternal", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::TransmitFromExternal), t, false)
                || attemptRegisterStatusBit("LocalOnly", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::LocalOnly), t, false)
                || attemptRegisterStatusBit("TemporaryFlag", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Temporary), t, false)
                || attemptRegisterStatusBit("ImportantElsewhere", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::ImportantElsewhere), t, false)
                || attemptRegisterStatusBit("Important", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Important), t, false)
                || attemptRegisterStatusBit("HighPriority", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::HighPriority), t, false)
                || attemptRegisterStatusBit("LowPriority", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::LowPriority), t, false)
                || attemptRegisterStatusBit("CalendarInvitation", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::CalendarInvitation), t, false)
                || attemptRegisterStatusBit("Todo", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::Todo), t, false)
                || attemptRegisterStatusBit("NoNotification", "messagestatus", 63, true, const_cast<quint64 *>(&QMailMessage::NoNotification), t, false);

        if (res) {
            qWarning() << "There was an error registering flags.";
            return false;
        }

        if ((countMessages(QMailMessageKey()) != 0)
            && (countThreads(QMailThreadKey()) == 0)) {
            if (!fullThreadTableUpdate())
                qWarning() << Q_FUNC_INFO << "Full thread's table update is not completed.";
        }

        if (!setupFolders(QList<FolderInfo>() << FolderInfo(QMailFolder::LocalStorageFolderId, tr("Local Storage"), QMailFolder::MessagesPermitted))) {
            qWarning() << "Error setting up folders";
            return false;
        }

        if (!t.commit()) {
            qWarning() << "Could not commit setup operation to database";
            return false;
        }

    }

#if defined(Q_USE_SQLITE)
    {
        QSqlQuery query( *database() );
        query.exec(QLatin1String("PRAGMA journal_mode=WAL;")); // enable write ahead logging
        if (query.next() && query.value(0).toString().toLower() != "wal") {
            qWarning() << "res" << query.value(0).toString().toLower();
            qWarning() << "INCORRECT DATABASE FORMAT!!! EXPECT SLOW DATABASE PERFORMANCE!!!";
            qWarning() << "WAL mode disabled. Please delete $QMF_DATA directory, and/or update sqlite to >= 3.7.";
        }
    }
    {
        // Reduce page cache from 2MB (2000 pages) to 1MB
        QSqlQuery query( *database() );
        if (!query.exec(QLatin1String("PRAGMA cache_size=1000;"))) {
            qWarning() << "Unable to reduce page cache size" << query.lastQuery().simplified();
        }
    }
#if defined(QMF_NO_DURABILITY) || defined(QMF_NO_SYNCHRONOUS_DB)
#if defined(QMF_NO_SYNCHRONOUS_DB)
    {
        // Use sqlite synchronous=OFF does not protect integrity of database does not ensure durability
        QSqlQuery query( *database() );
        qWarning() << "Disabling synchronous writes, database may become corrupted!";
        if (!query.exec(QLatin1String("PRAGMA synchronous=OFF;"))) {
            qWarning() << "Unable to set synchronous mode to OFF" << query.lastQuery().simplified();
        }
    }
#else
    {
        // Use sqlite synchronous=NORMAL protects integrity of database but does not ensure durability
        QSqlQuery query( *database() );
        if (!query.exec(QLatin1String("PRAGMA synchronous=NORMAL;"))) {
            qWarning() << "Unable to set synchronous mode to NORMAL" << query.lastQuery().simplified();
        }
    }
#endif
#endif
#endif

    if (!QMailContentManagerFactory::init()) {
        qWarning() << "Could not initialize content manager factory";
        return false;
    }

    if (!performMaintenance()) {
        return false;
    }

    // We are now correctly initialized
    return true;
}

void QMailStorePrivate::clearContent()
{
    // Clear all caches
    accountCache.clear();
    folderCache.clear();
    messageCache.clear();
    uidCache.clear();
    threadCache.clear();

    Transaction t(this);

#ifdef USE_ACCOUNTS_QT
    // Remove all SSO email accounts
    Accounts::AccountIdList accountIDList = manager->accountList("e-mail");

    // Populate all E-Mail accounts
    foreach (Accounts::AccountId accountID, accountIDList) {
        // Remove account
        QSharedPointer<Accounts::Account> ssoAccount = getEmailAccount(accountID);

        if (ssoAccount) {
            ssoAccount->remove();
            ssoAccount->syncAndBlock();
        } else
            SSOHandleError(manager->lastError());
    }
#endif

    // Drop all data
    foreach (const QString &table, database()->tables()) {
        if (table != "versioninfo" && table != "mailstatusflags") {
            QString sql("DELETE FROM %1");
            QSqlQuery query(*database());
            if (!query.exec(sql.arg(table))) {
                qWarning() << "Failed to delete from table - query:" << sql << "- error:" << query.lastError().text();
            }
        }
    }

    if (!t.commit()) {
        qWarning() << "Could not commit clearContent operation to database";
    }

    // Remove all content
    QMailContentManagerFactory::clearContent();
}

bool QMailStorePrivate::transaction(void)
{
    if (inTransaction) {
        qWarning() << "(" << pid << ")" << "Transaction already exists at begin!";
        qWarning() << "Transaction already exists at begin!";
    }

    clearQueryError();

    // Ensure any outstanding temp tables are removed before we begin this transaction
    destroyTemporaryTables();

    if (!database()->transaction()) {
        setQueryError(database()->lastError(), "Failed to initiate transaction");
        return false;
    }

    inTransaction = true;
    return true;
}

static QString queryText(const QString &query, const QList<QVariant> &values)
{
    static const QChar marker('?');
    static const QChar quote('\'');

    QString result(query);

    QList<QVariant>::const_iterator it = values.begin(), end = values.end();
    int index = result.indexOf(marker);
    while ((index != -1) && (it != end)) {
        QString substitute((*it).toString());
        if ((*it).type() == QVariant::String)
            substitute.prepend(quote).append(quote);

        result.replace(index, 1, substitute);

        ++it;
        index = result.indexOf(marker, index + substitute.length());
    }

    return result;
}

static QString queryText(const QSqlQuery &query)
{
    // Note: we currently only handle positional parameters
    return queryText(query.lastQuery().simplified(), query.boundValues().values());
}

QSqlQuery QMailStorePrivate::prepare(const QString& sql)
{
    if (!inTransaction) {
        // Ensure any outstanding temp tables are removed before we begin this query
        destroyTemporaryTables();
    }

    clearQueryError();

    // Create any temporary tables needed for this query
    while (!requiredTableKeys.isEmpty()) {
        QPair<const QMailMessageKey::ArgumentType *, QString> key(requiredTableKeys.takeFirst());
        const QMailMessageKey::ArgumentType *arg = key.first;
        if (!temporaryTableKeys.contains(arg)) {
            QString tableName = temporaryTableName(*arg);

            {
                QSqlQuery createQuery(*database());
                if (!createQuery.exec(QString("CREATE TEMP TABLE %1 ( id %2 PRIMARY KEY )").arg(tableName).arg(key.second))) { 
                    setQueryError(createQuery.lastError(), "Failed to create temporary table", queryText(createQuery));
                    qWarning() << "Unable to prepare query:" << sql;
                    return QSqlQuery();
                }
            }

            temporaryTableKeys.append(arg);

            QVariantList idValues;

            if (key.second == "INTEGER") {
                int type = 0;
                if (arg->valueList.first().canConvert<QMailMessageId>()) {
                    type = 1;
                } else if (arg->valueList.first().canConvert<QMailFolderId>()) {
                    type = 2;
                } else if (arg->valueList.first().canConvert<QMailAccountId>()) {
                    type = 3;
                }

                // Extract the ID values to INTEGER variants
                foreach (const QVariant &var, arg->valueList) {
                    quint64 id = 0;

                    switch (type) {
                    case 1:
                        id = var.value<QMailMessageId>().toULongLong(); 
                        break;
                    case 2:
                        id = var.value<QMailFolderId>().toULongLong(); 
                        break;
                    case 3:
                        id = var.value<QMailAccountId>().toULongLong(); 
                        break;
                    default:
                        qWarning() << "Unable to extract ID value from valuelist!";
                        qWarning() << "Unable to prepare query:" << sql;
                        return QSqlQuery();
                    }

                    idValues.append(QVariant(id));
                }

                // Add the ID values to the temp table
                {
                    QSqlQuery insertQuery(*database());
                    insertQuery.prepare(QString("INSERT INTO %1 VALUES (?)").arg(tableName));
                    insertQuery.addBindValue(idValues);
                    if (!insertQuery.execBatch()) { 
                        setQueryError(insertQuery.lastError(), "Failed to populate integer temporary table", queryText(insertQuery));
                        qWarning() << "Unable to prepare query:" << sql;
                        return QSqlQuery();
                    }
                }
            } else if (key.second == "VARCHAR") {
                foreach (const QVariant &var, arg->valueList) {
                    idValues.append(QVariant(var.value<QString>()));
                }

                {
                    QSqlQuery insertQuery(*database());
                    insertQuery.prepare(QString("INSERT INTO %1 VALUES (?)").arg(tableName));
                    insertQuery.addBindValue(idValues);
                    if (!insertQuery.execBatch()) { 
                        setQueryError(insertQuery.lastError(), "Failed to populate varchar temporary table", queryText(insertQuery));
                        qWarning() << "Unable to prepare query:" << sql;
                        return QSqlQuery();
                    }
                }
            }
        }
    }

    QSqlQuery query(*database());
    query.setForwardOnly(true);
    if (!query.prepare(sql)) {
        setQueryError(query.lastError(), "Failed to prepare query", queryText(query));
    }

    return query;
}

bool QMailStorePrivate::execute(QSqlQuery& query, bool batch)
{
    bool success = (batch ? query.execBatch() : query.exec());
    if (!success) {
        setQueryError(query.lastError(), "Failed to execute query", queryText(query));
        return false;
    }

#ifdef QMAILSTORE_LOG_SQL 
    qMailLog(Messaging) << "(" << pid << ")" << qPrintable(queryText(query));
#endif

    if (!inTransaction) {
        // We should be finished with these temporary tables
        expiredTableKeys = temporaryTableKeys;
        temporaryTableKeys.clear();
    }

    return true;
}

bool QMailStorePrivate::commit(void)
{
    if (!inTransaction) {
        qWarning() << "(" << pid << ")" << "Transaction does not exist at commit!";
        qWarning() << "Transaction does not exist at commit!";
    }
    
    if (!database()->commit()) {
        setQueryError(database()->lastError(), "Failed to commit transaction");
        return false;
    } else {
        inTransaction = false;

        // Expire any temporary tables we were using
        expiredTableKeys = temporaryTableKeys;
        temporaryTableKeys.clear();
    }

    return true;
}

void QMailStorePrivate::rollback(void)
{
    if (!inTransaction) {
        qWarning() << "(" << pid << ")" << "Transaction does not exist at rollback!";
        qWarning() << "Transaction does not exist at rollback!";
    }
    
    inTransaction = false;

    if (!database()->rollback()) {
        setQueryError(database()->lastError(), "Failed to rollback transaction");
    }
}

int QMailStorePrivate::queryError() const
{
    return lastQueryError;
}

void QMailStorePrivate::setQueryError(const QSqlError &error, const QString &description, const QString &statement)
{
    QString s;
    QTextStream ts(&s);

    lastQueryError = error.number();

    ts << qPrintable(description) << "; error:\"" << error.text() << '"';
    if (!statement.isEmpty())
        ts << "; statement:\"" << statement.simplified() << '"';

    qWarning() << "(" << pid << ")" << qPrintable(s);
    qWarning() << qPrintable(s);
}

void QMailStorePrivate::clearQueryError(void) 
{
    lastQueryError = QSqlError::NoError;
}

template<bool PtrSizeExceedsLongSize>
QString numericPtrValue(const void *ptr)
{
    return QString::number(reinterpret_cast<unsigned long long>(ptr), 16).rightJustified(16, '0');
}

template<>
QString numericPtrValue<false>(const void *ptr)
{
    return QString::number(reinterpret_cast<unsigned long>(ptr), 16).rightJustified(8, '0');;
}

QString QMailStorePrivate::temporaryTableName(const QMailMessageKey::ArgumentType& arg)
{
    const QMailMessageKey::ArgumentType *ptr = &arg;
    return QString("qmf_idmatch_%1").arg(numericPtrValue<(sizeof(void*) > sizeof(unsigned long))>(ptr));
}

void QMailStorePrivate::createTemporaryTable(const QMailMessageKey::ArgumentType& arg, const QString &dataType) const
{
    requiredTableKeys.append(qMakePair(&arg, dataType));
}

void QMailStorePrivate::destroyTemporaryTables()
{
    while (!expiredTableKeys.isEmpty()) {
        const QMailMessageKey::ArgumentType *arg = expiredTableKeys.takeFirst();
        QString tableName = temporaryTableName(*arg);

        QSqlQuery query(*database());
        if (!query.exec(QString("DROP TABLE %1").arg(tableName))) {
            QString sql = queryText(query);
            QString err = query.lastError().text();

            qWarning() << "(" << pid << ")" << "Failed to drop temporary table - query:" << qPrintable(sql) << "; error:" << qPrintable(err);
            qWarning() << "Failed to drop temporary table - query:" << qPrintable(sql) << "; error:" << qPrintable(err);
        }
    }
}

QMap<QString, QString> QMailStorePrivate::messageCustomFields(const QMailMessageId &id)
{
    Q_ASSERT(id.isValid());
    QMap<QString, QString> fields;
    AttemptResult res(customFields(id.toULongLong(), &fields, "mailmessagecustom"));
    if (res != Success)
        qWarning() << "Could not query custom fields for message id: " << id.toULongLong();

    return fields;
 }

bool QMailStorePrivate::idValueExists(quint64 id, const QString& table)
{
    QSqlQuery query(*database());
    QString sql = "SELECT id FROM " + table + " WHERE id=?";
    if(!query.prepare(sql)) {
        setQueryError(query.lastError(), "Failed to prepare idExists query", queryText(query));
        return false;
    }

    query.addBindValue(id);

    if(!query.exec()) {
        setQueryError(query.lastError(), "Failed to execute idExists query", queryText(query));
        return false;
    }

    return (query.first());
}

#ifdef USE_ACCOUNTS_QT
bool QMailStorePrivate::idExists(const QMailAccountId& id)
{
    QSharedPointer<Accounts::Account> ssoAccount = getEmailAccount(id.toULongLong());
    return (ssoAccount != NULL);
}
#else
bool QMailStorePrivate::idExists(const QMailAccountId& id, const QString& table)
{
    return idValueExists(id.toULongLong(), (table.isEmpty() ? "mailaccounts" : table));
}
#endif

bool QMailStorePrivate::idExists(const QMailFolderId& id, const QString& table)
{
    return idValueExists(id.toULongLong(), (table.isEmpty() ? "mailfolders" : table));
}

bool QMailStorePrivate::idExists(const QMailMessageId& id, const QString& table)
{
    return idValueExists(id.toULongLong(), (table.isEmpty() ? "mailmessages" : table));
}

bool QMailStorePrivate::messageExists(const QString &serveruid, const QMailAccountId &id)
{
    QSqlQuery query(*database());
    QString sql = "SELECT id FROM mailmessages WHERE serveruid=? AND parentaccountid=?";
    if(!query.prepare(sql)) {
        setQueryError(query.lastError(), "Failed to prepare messageExists query");
    }
    query.addBindValue(serveruid);
    query.addBindValue(id.toULongLong());

    if(!query.exec()) {
        setQueryError(query.lastError(), "Failed to execute messageExists");
    }

    return query.first();
}

#ifdef USE_ACCOUNTS_QT
QMailAccount QMailStorePrivate::extractAccount(const QSharedPointer<Accounts::Account>& ssoAccount)
{
    Q_ASSERT(ssoAccount);

    QMailAccount result;
    result.setId(QMailAccountId(ssoAccount->id()));
    QString name = ssoAccount->valueAsString("email/email_box_name");
    if (name.isEmpty())
        name = ssoAccount->displayName();
    result.setName(name);
    result.setMessageType(static_cast<QMailMessageMetaDataFwd::MessageType>(ssoAccount->valueAsInt("type")));
    result.setStatus(ssoAccount->valueAsUInt64("status"));

    Accounts::Service service = ssoAccount->selectedService();

    ssoAccount->selectService();
    const bool& enabled = ssoAccount->enabled();
    ssoAccount->selectService(service);
    const bool& isDefault = ssoAccount->valueAsBool("email/default");
    const bool& canTransmit = ssoAccount->valueAsBool("canTransmit", true);
    const bool& appendSignature = ssoAccount->valueAsBool("signatureEnabled", true);
    const bool& hasPersistentConnection = ssoAccount->valueAsBool("hasPersistentConnection", false);

    result.setStatus(QMailAccount::Enabled, enabled);
    result.setStatus(QMailAccount::PreferredSender, isDefault);
    result.setStatus(QMailAccount::CanTransmit, canTransmit);
    result.setStatus(QMailAccount::AppendSignature, appendSignature);
    result.setStatus(QMailAccount::HasPersistentConnection, hasPersistentConnection);

    result.setSignature(ssoAccount->valueAsString("signature"));
    result.setFromAddress(ssoAccount->contains("fullName")?
                          QMailAddress(ssoAccount->valueAsString("fullName"),ssoAccount->valueAsString("emailaddress")):
                          QMailAddress(ssoAccount->valueAsString("emailaddress")));

    if ((static_cast<uint>(ssoAccount->valueAsUInt64("lastSynchronized"))) == 0) {
            result.setLastSynchronized(QMailTimeStamp());
    }
    else {
        result.setLastSynchronized(QMailTimeStamp(QDateTime::fromTime_t(static_cast<uint>(ssoAccount->valueAsUInt64("lastSynchronized")))));
    }

    result.setIconPath(ssoAccount->valueAsString("iconPath"));

    return result;
}
#else
QMailAccount QMailStorePrivate::extractAccount(const QSqlRecord& r)
{
    const AccountRecord record(r);

    QMailAccount result;
    result.setId(record.id());
    result.setName(record.name());
    result.setMessageType(record.messageType());
    result.setStatus(record.status());
    result.setSignature(record.signature());
    result.setFromAddress(QMailAddress(record.fromAddress()));
    result.setLastSynchronized(record.lastSynchronized());
    result.setIconPath(record.iconPath());


    return result;
}
#endif

QMailThread QMailStorePrivate::extractThread(const QSqlRecord& r)
{
    const ThreadRecord record(r);

    QMailThread result;
    result.setId(record.id());
    result.setServerUid(record.serverUid());
    result.setMessageCount(record.messageCount());
    result.setUnreadCount(record.unreadCount());
    result.setParentAccountId(record.parentAccountId());
    result.setSubject(record.subject());
    result.setSenders(record.senders());
    result.setPreview(record.preview());
    result.setLastDate(record.lastDate());
    result.setStartedDate(record.startedDate());
    result.setStatus(record.status());
    return result;
}


QMailFolder QMailStorePrivate::extractFolder(const QSqlRecord& r)
{
    const FolderRecord record(r);

    QMailFolder result(record.path(), record.parentFolderId(), record.parentAccountId());
    result.setId(record.id());
    result.setDisplayName(record.displayName());
    result.setStatus(record.status());
    result.setServerCount(record.serverCount());
    result.setServerUnreadCount(record.serverUnreadCount());
    result.setServerUndiscoveredCount(record.serverUndiscoveredCount());
    return result;
}

void QMailStorePrivate::extractMessageMetaData(const QSqlRecord& r,
                                               QMailMessageKey::Properties recordProperties,
                                               const QMailMessageKey::Properties& properties,
                                               QMailMessageMetaData* metaData)
{
    // Record whether we have loaded all data for this message
    bool unloadedProperties = (properties != allMessageProperties());
    if (!unloadedProperties) {
        // If there is message content, mark the object as not completely loaded
        if (!r.value("mailfile").toString().isEmpty())
            unloadedProperties = true;
    }

    // Use wrapper to extract data items
    const MessageRecord messageRecord(r, recordProperties);

    foreach (QMailMessageKey::Property p, messagePropertyList()) {
        switch (properties & p)
        {
        case QMailMessageKey::Id:
            metaData->setId(messageRecord.id());
            break;

        case QMailMessageKey::Type:
            metaData->setMessageType(messageRecord.messageType());
            break;

        case QMailMessageKey::ParentFolderId:
            metaData->setParentFolderId(messageRecord.parentFolderId());
            break;

        case QMailMessageKey::Sender:
            metaData->setFrom(messageRecord.from());
            break;

        case QMailMessageKey::Recipients:
            metaData->setRecipients(messageRecord.to());
            break;

        case QMailMessageKey::Subject:
            metaData->setSubject(messageRecord.subject());
            break;

        case QMailMessageKey::TimeStamp:
            metaData->setDate(messageRecord.date());
            break;

        case QMailMessageKey::ReceptionTimeStamp:
            metaData->setReceivedDate(messageRecord.receivedDate());
            break;

        case QMailMessageKey::Status:
            metaData->setStatus(messageRecord.status());
            break;

        case QMailMessageKey::ParentAccountId:
            metaData->setParentAccountId(messageRecord.parentAccountId());
            break;

        case QMailMessageKey::ServerUid:
            metaData->setServerUid(messageRecord.serverUid());
            break;

        case QMailMessageKey::Size:
            metaData->setSize(messageRecord.size());
            break;

        case QMailMessageKey::ContentType:
            metaData->setContent(messageRecord.content());
            break;

        case QMailMessageKey::PreviousParentFolderId:
            metaData->setPreviousParentFolderId(messageRecord.previousParentFolderId());
            break;

        case QMailMessageKey::ContentScheme:
            metaData->setContentScheme(messageRecord.contentScheme());
            break;

        case QMailMessageKey::ContentIdentifier:
            metaData->setContentIdentifier(messageRecord.contentIdentifier());
            break;

        case QMailMessageKey::InResponseTo:
            metaData->setInResponseTo(messageRecord.inResponseTo());
            break;

        case QMailMessageKey::ResponseType:
            metaData->setResponseType(messageRecord.responseType());
            break;

        case QMailMessageKey::CopyServerUid:
            metaData->setCopyServerUid(messageRecord.copyServerUid());
            break;

        case QMailMessageKey::RestoreFolderId:
            metaData->setRestoreFolderId(messageRecord.restoreFolderId());
            break;

        case QMailMessageKey::ListId:
            metaData->setListId(messageRecord.listId());
            break;

        case QMailMessageKey::RfcId:
            metaData->setRfcId(messageRecord.rfcId());
            break;

        case QMailMessageKey::Preview:
            metaData->setPreview(messageRecord.preview());
            break;

        case QMailMessageKey::ParentThreadId:
            metaData->setParentThreadId(messageRecord.parentThreadId());
            break;
        }
    }
    
    if (unloadedProperties) {
        // This message is not completely loaded
        metaData->setStatus(QMailMessage::UnloadedData, true);
    }

    metaData->setUnmodified();
}

QMailMessageMetaData QMailStorePrivate::extractMessageMetaData(const QSqlRecord& r, QMailMessageKey::Properties recordProperties, const QMailMessageKey::Properties& properties)
{
    QMailMessageMetaData metaData;

    extractMessageMetaData(r, recordProperties, properties, &metaData);
    return metaData;
}

QMailMessageMetaData QMailStorePrivate::extractMessageMetaData(const QSqlRecord& r, const QMap<QString, QString> &customFields, const QMailMessageKey::Properties& properties)
{
    QMailMessageMetaData metaData;

    // Load the meta data items (note 'SELECT *' does not give the same result as 'SELECT expand(allMessageProperties())')
    extractMessageMetaData(r, QMailMessageKey::Properties(0), properties, &metaData);

    metaData.setCustomFields(customFields);
    metaData.setCustomFieldsModified(false);

    return metaData;
}

QMailMessage QMailStorePrivate::extractMessage(const QSqlRecord& r, const QMap<QString, QString> &customFields, const QMailMessageKey::Properties& properties)
{
    QMailMessage newMessage;

    // Load the meta data items (note 'SELECT *' does not give the same result as 'SELECT expand(allMessageProperties())')
    extractMessageMetaData(r, QMailMessageKey::Properties(0), properties, &newMessage);

    newMessage.setCustomFields(customFields);
    newMessage.setCustomFieldsModified(false);

    QString contentUri(r.value("mailfile").toString());
    if (!contentUri.isEmpty()) {
        QPair<QString, QString> elements(extractUriElements(contentUri));

        MutexGuard lock(contentManagerMutex());
        lock.lock();

        QMailContentManager *contentManager = QMailContentManagerFactory::create(elements.first);
        if (contentManager) {
            // Load the message content (manager should be able to access the metadata also)
            QMailStore::ErrorCode code = contentManager->load(elements.second, &newMessage);
            if (code != QMailStore::NoError) {
                setLastError(code);
                qWarning() << "Unable to load message content:" << contentUri;
                return QMailMessage();
            }
        } else {
            qWarning() << "Unable to create content manager for scheme:" << elements.first;
            return QMailMessage();
        }

        // Re-load the meta data items so that they take precedence over the loaded content
        extractMessageMetaData(r, QMailMessageKey::Properties(0), properties, &newMessage);

        newMessage.setCustomFields(customFields);
        newMessage.setCustomFieldsModified(false);
    }

    return newMessage;
}

QMailMessageRemovalRecord QMailStorePrivate::extractMessageRemovalRecord(const QSqlRecord& r)
{
    const MessageRemovalRecord record(r);

    QMailMessageRemovalRecord result(record.parentAccountId(), record.serverUid(), record.parentFolderId());
    return result;
}

QString QMailStorePrivate::buildOrderClause(const Key& key) const
{
    if (key.isType<QMailMessageSortKey>()) {
        const QMailMessageSortKey &sortKey(key.key<QMailMessageSortKey>());
        return ::buildOrderClause(sortKey.arguments(), key.alias());
    } else if (key.isType<QMailFolderSortKey>()) {
        const QMailFolderSortKey &sortKey(key.key<QMailFolderSortKey>());
        return ::buildOrderClause(sortKey.arguments(), key.alias());
    } else if (key.isType<QMailThreadSortKey>()) {
        const QMailThreadSortKey &sortKey(key.key<QMailThreadSortKey>());
        return ::buildOrderClause(sortKey.arguments(), key.alias());
    } else if (key.isType<QMailAccountSortKey>()) {
#ifdef USE_ACCOUNTS_QT
        Q_ASSERT(false);
#else
        const QMailAccountSortKey &sortKey(key.key<QMailAccountSortKey>());
        return ::buildOrderClause(sortKey.arguments(), key.alias());
#endif
    } 

    return QString();
}

QString QMailStorePrivate::buildWhereClause(const Key& key, bool nested, bool firstClause) const
{
    if (key.isType<QMailMessageKey>()) {
        const QMailMessageKey &messageKey(key.key<QMailMessageKey>());

        // See if we need to create any temporary tables to use in this query
        foreach (const QMailMessageKey::ArgumentType &a, messageKey.arguments()) {
            if (a.property == QMailMessageKey::Id && a.valueList.count() >= IdLookupThreshold) {
                createTemporaryTable(a, "INTEGER");
            } else if (a.property == QMailMessageKey::ServerUid && a.valueList.count() >= IdLookupThreshold) {
                createTemporaryTable(a, "VARCHAR");
            }
        }

        return ::buildWhereClause(messageKey, messageKey.arguments(), messageKey.subKeys(), messageKey.combiner(), messageKey.isNegated(), nested, firstClause, key.alias(), key.field(), *this);
    } else if (key.isType<QMailFolderKey>()) {
        const QMailFolderKey &folderKey(key.key<QMailFolderKey>());
        return ::buildWhereClause(folderKey, folderKey.arguments(), folderKey.subKeys(), folderKey.combiner(), folderKey.isNegated(), nested, firstClause, key.alias(), key.field(), *this);
    } else if (key.isType<QMailAccountKey>()) {
        const QMailAccountKey &accountKey(key.key<QMailAccountKey>());
        return ::buildWhereClause(accountKey, accountKey.arguments(), accountKey.subKeys(), accountKey.combiner(), accountKey.isNegated(), nested, firstClause, key.alias(), key.field(), *this);
    } else if (key.isType<QMailThreadKey>()) {
        const QMailThreadKey &threadKey(key.key<QMailThreadKey>());
        return ::buildWhereClause(threadKey, threadKey.arguments(), threadKey.subKeys(), threadKey.combiner(), threadKey.isNegated(), nested, firstClause, key.alias(), key.field(), *this);
    }

    return QString();
}

QVariantList QMailStorePrivate::whereClauseValues(const Key& key) const
{
    if (key.isType<QMailMessageKey>()) {
        const QMailMessageKey &messageKey(key.key<QMailMessageKey>());
        return ::whereClauseValues(messageKey);
    } else if (key.isType<QMailFolderKey>()) {
        const QMailFolderKey &folderKey(key.key<QMailFolderKey>());
        return ::whereClauseValues(folderKey);
    } else if (key.isType<QMailAccountKey>()) {
        const QMailAccountKey &accountKey(key.key<QMailAccountKey>());
        return ::whereClauseValues(accountKey);
    } else if (key.isType<QMailThreadKey>()) {
        const QMailThreadKey &threadKey(key.key<QMailThreadKey>());
        return ::whereClauseValues(threadKey);
    }

    return QVariantList();
}

QVariantList QMailStorePrivate::messageValues(const QMailMessageKey::Properties& prop, const QMailMessageMetaData& data)
{
    QVariantList values;

    const MessageValueExtractor<QMailMessageMetaData> extractor(data);

    // The ContentScheme and ContentIdentifier properties map to the same field
    QMailMessageKey::Properties properties(prop);
    if ((properties & QMailMessageKey::ContentScheme) && (properties & QMailMessageKey::ContentIdentifier))
        properties &= ~QMailMessageKey::ContentIdentifier;

    foreach (QMailMessageKey::Property p, messagePropertyList()) {
        switch (properties & p)
        {
            case QMailMessageKey::Id:
                values.append(extractor.id());
                break;

            case QMailMessageKey::Type:
                values.append(extractor.messageType());
                break;

            case QMailMessageKey::ParentFolderId:
                values.append(extractor.parentFolderId());
                break;

            case QMailMessageKey::Sender:
                values.append(extractor.from());
                break;

            case QMailMessageKey::Recipients:
                values.append(extractor.to());
                break;

            case QMailMessageKey::Subject:
                values.append(extractor.subject());
                break;

            case QMailMessageKey::TimeStamp:
                values.append(extractor.date());
                break;

            case QMailMessageKey::ReceptionTimeStamp:
                values.append(extractor.receivedDate());
                break;

            case QMailMessageKey::Status:
                values.append(extractor.status());
                break;

            case QMailMessageKey::ParentAccountId:
                values.append(extractor.parentAccountId());
                break;

            case QMailMessageKey::ServerUid:
                values.append(extractor.serverUid());
                break;

            case QMailMessageKey::Size:
                values.append(extractor.size());
                break;

            case QMailMessageKey::ContentType:
                values.append(extractor.content());
                break;

            case QMailMessageKey::PreviousParentFolderId:
                values.append(extractor.previousParentFolderId());
                break;

            case QMailMessageKey::ContentScheme:
            case QMailMessageKey::ContentIdentifier:
                // For either of these (there can be only one) we want to produce the entire URI
                values.append(::contentUri(extractor.contentScheme().toString(), extractor.contentIdentifier().toString()));
                break;

            case QMailMessageKey::InResponseTo:
                values.append(extractor.inResponseTo());
                break;

            case QMailMessageKey::ResponseType:
                values.append(extractor.responseType());
                break;

            case QMailMessageKey::CopyServerUid:
                values.append(extractor.copyServerUid());
                break;

            case QMailMessageKey::RestoreFolderId:
                values.append(extractor.restoreFolderId());
                break;

            case QMailMessageKey::ListId:
                values.append(extractor.listId());
                break;

            case QMailMessageKey::RfcId:
                values.append(extractor.rfcId());
                break;

            case QMailMessageKey::Preview:
                values.append(extractor.preview());
                break;

            case QMailMessageKey::ParentThreadId:
                values.append(extractor.parentThreadId());
        }
    }

    return values;
}

QVariantList QMailStorePrivate::threadValues(const QMailThreadKey::Properties &prop, const QMailThread &thread)
{
    QVariantList values;

    QMailThreadKey::Properties properties(prop);
    foreach (QMailThreadKey::Property p, threadPropertyList()) {
        switch (properties & p)
        {
            case QMailThreadKey::Id:
                values.append(thread.id().toULongLong());
                break;

            case QMailThreadKey::ServerUid:
                values.append(thread.serverUid());
                break;

            case QMailThreadKey::MessageCount:
                values.append(thread.messageCount());
                break;

            case QMailThreadKey::UnreadCount:
                values.append(thread.unreadCount());
                break;

            case QMailThreadKey::ParentAccountId:
                values.append(thread.parentAccountId().toULongLong());
                break;

            case QMailThreadKey::Subject:
                values.append(thread.subject());
                break;

            case QMailThreadKey::Senders:
                values.append(QMailAddress::toStringList(thread.senders()).join(","));
                break;

            case QMailThreadKey::LastDate:
                values.append(thread.lastDate().toUTC());
                break;

            case QMailThreadKey::StartedDate:
                values.append(thread.startedDate().toUTC());
                break;

            case QMailThreadKey::Status:
                values.append(thread.status());
                break;

            case QMailThreadKey::Preview:
                values.append(thread.preview());
                break;

        }
    }

    return values;
}

void QMailStorePrivate::updateMessageValues(const QMailMessageKey::Properties& properties, const QVariantList& values, const QMap<QString, QString>& customFields, QMailMessageMetaData& metaData)
{
    QPair<QString, QString> uriElements;
    QVariantList::const_iterator it = values.constBegin();

    foreach (QMailMessageKey::Property p, messagePropertyList()) {
        const MessageValueExtractor<QVariant> extractor(*it);
        bool valueConsumed(true);

        switch (properties & p)
        {
            case QMailMessageKey::Id:
                metaData.setId(extractor.id());
                break;

            case QMailMessageKey::Type:
                metaData.setMessageType(extractor.messageType());
                break;

            case QMailMessageKey::ParentFolderId:
                metaData.setParentFolderId(extractor.parentFolderId());
                break;

            case QMailMessageKey::Sender:
                metaData.setFrom(extractor.from());
                break;

            case QMailMessageKey::Recipients:
                metaData.setRecipients(extractor.to());
                break;

            case QMailMessageKey::Subject:
                metaData.setSubject(extractor.subject());
                break;

            case QMailMessageKey::TimeStamp:
                metaData.setDate(extractor.date());
                break;

            case QMailMessageKey::ReceptionTimeStamp:
                metaData.setReceivedDate(extractor.receivedDate());
                break;

            case QMailMessageKey::Status:
                metaData.setStatus(extractor.status());
                break;

            case QMailMessageKey::ParentAccountId:
                metaData.setParentAccountId(extractor.parentAccountId());
                break;

            case QMailMessageKey::ServerUid:
                metaData.setServerUid(extractor.serverUid());
                break;

            case QMailMessageKey::Size:
                metaData.setSize(extractor.size());
                break;

            case QMailMessageKey::ContentType:
                metaData.setContent(extractor.content());
                break;

            case QMailMessageKey::PreviousParentFolderId:
                metaData.setPreviousParentFolderId(extractor.previousParentFolderId());
                break;

            case QMailMessageKey::ContentScheme:
                if (uriElements.first.isEmpty()) {
                    uriElements = extractUriElements(extractor.contentUri());
                } else {
                    valueConsumed = false;
                }
                metaData.setContentScheme(uriElements.first);
                break;

            case QMailMessageKey::ContentIdentifier:
                if (uriElements.first.isEmpty()) {
                    uriElements = extractUriElements(extractor.contentUri());
                } else {
                    valueConsumed = false;
                }
                metaData.setContentIdentifier(uriElements.second);
                break;

            case QMailMessageKey::InResponseTo:
                metaData.setInResponseTo(extractor.inResponseTo());
                break;

            case QMailMessageKey::ResponseType:
                metaData.setResponseType(extractor.responseType());
                break;

            case QMailMessageKey::CopyServerUid:
                metaData.setCopyServerUid(extractor.copyServerUid());
                break;

            case QMailMessageKey::RestoreFolderId:
                metaData.setRestoreFolderId((extractor.restoreFolderId()));
                break;

            case QMailMessageKey::ListId:
                metaData.setListId(extractor.listId());
                break;

            case QMailMessageKey::RfcId:
                metaData.setRfcId(extractor.rfcId());
                break;

            case QMailMessageKey::Preview:
                metaData.setPreview(extractor.preview());
                break;

            case QMailMessageKey::ParentThreadId:
                metaData.setParentThreadId(extractor.parentThreadId());
                break;

            default:
                valueConsumed = false;
                break;
        }

        if (valueConsumed)
            ++it;
    }

    if (it != values.constEnd())
        qWarning() << QString("updateMessageValues: %1 values not consumed!").arg(values.constEnd() - it);

    // QMailMessageKey::Custom is a "special" kid.
    if ((properties & QMailMessageKey::Custom)) {
        metaData.setCustomFields(customFields);
    }

    // The target message is not completely loaded
    metaData.setStatus(QMailMessage::UnloadedData, true);
}

QMailStorePrivate::AttemptResult QMailStorePrivate::updateThreadsValues(const QMailThreadIdList& threadsToDelete,
                                                     const QMailThreadIdList& modifiedThreadsIds,
                                                     const ThreadUpdateData& updateData)
{
    // delete threads if necessary
    if (!threadsToDelete.isEmpty()) {
        QString sql("DELETE FROM mailthreads WHERE id IN %1");
        QVariantList bindValues;
        foreach (const QMailThreadId& threadId, threadsToDelete) {
            bindValues << threadId.toULongLong();
        }
        QVariantList bindValuesBatch;
        while (!bindValues.isEmpty()) {
            bindValuesBatch = bindValues.mid(0, 500);
            if (bindValues.count() > 500) {
                bindValues = bindValues.mid(500);
            } else {
                bindValues.clear();
            }
            QSqlQuery query = simpleQuery(sql.arg(expandValueList(bindValuesBatch)), bindValuesBatch, "updateThreads mailthreads delete");
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
    }
    if (modifiedThreadsIds.isEmpty())
        return Success;

    QVariantList bindValues;
    QString sql("UPDATE mailthreads SET ");
    bool firstProperty = true;

    if (updateData.mMessagesCount != 0) {
        if (firstProperty) {
            sql.append(QString("%1 = messagecount + (?)").arg(threadPropertyMap().value(QMailThreadKey::MessageCount)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = messagecount + (?)").arg(threadPropertyMap().value(QMailThreadKey::MessageCount)));
        }
        bindValues << updateData.mMessagesCount;
    }
    if (updateData.mReadMessagesCount != 0) {
        if (firstProperty) {
            sql.append(QString("%1 = unreadcount + (?)").arg(threadPropertyMap().value(QMailThreadKey::UnreadCount)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = unreadcount + (?)").arg(threadPropertyMap().value(QMailThreadKey::UnreadCount)));
        }
        bindValues << updateData.mReadMessagesCount;
    }
    if (!updateData.mNewSubject.isEmpty()) {
        if (firstProperty) {
            sql.append(QString("%1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::Subject)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::Subject)));
        }
        bindValues << updateData.mNewSubject;
    }
    if (!updateData.mNewSenders.isEmpty()) {
        if (firstProperty) {
            sql.append(QString("%1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::Senders)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::Senders)));
        }
        bindValues << updateData.mNewSenders;
    }
    if (!updateData.mNewLastDate.isNull()) {
        if (firstProperty) {
            sql.append(QString("%1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::LastDate)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::LastDate)));
        }
        bindValues << updateData.mNewLastDate.toUTC();
    }
    if (!updateData.mNewStartedDate.isNull()) {
        if (firstProperty) {
            sql.append(QString("%1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::StartedDate)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::StartedDate)));
        }
        bindValues << updateData.mNewStartedDate.toUTC();
    }
    if (!updateData.mNewPreview.isEmpty()) {
        if (firstProperty) {
            sql.append(QString("%1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::Preview)));
            firstProperty = false;
        } else {
            sql.append(QString(", %1 = (?)").arg(threadPropertyMap().value(QMailThreadKey::Preview)));
        }
        bindValues << updateData.mNewPreview;
    }
    if (updateData.mStatus > 0) {
        if (firstProperty) {
            sql.append(QString("status = (status | (?))"));
            firstProperty = false;
        } else {
            sql.append(QString(", status = (status | (?))"));
        }
        bindValues << updateData.mStatus;
    }
    if (updateData.mStatus < 0) {
        if (firstProperty) {
            sql.append(QString("status = (~((status|%1)& %1))&(status|%1)").arg(updateData.mStatus * (-1)));
            firstProperty = false;
        } else {
            sql.append(QString(", status = (~((status|%1)& %1))&(status|%1)").arg(updateData.mStatus * (-1)));
        }
    }

    if (firstProperty) {
        qWarning() << "QMailStorePrivate::updateThreadsValues(): nothing to update, looks like something is wrong!";
        return Success;
    }

    sql.append(" WHERE id IN %1");
    QVariantList threadsValuesList;
    foreach (const QMailThreadId& threadId, modifiedThreadsIds)
        threadsValuesList << threadId.toULongLong();
    bindValues.append(threadsValuesList);
    QSqlQuery query = simpleQuery(sql.arg(expandValueList(threadsValuesList)), bindValues, "updateThreads mailthreads update");
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;
    return Success;
}


bool QMailStorePrivate::executeFile(QFile &file)
{
    bool result(true);

    // read assuming utf8 encoding.
    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("utf8"));
    ts.setAutoDetectUnicode(true);
    
    QString sql = parseSql(ts);
    while (result && !sql.isEmpty()) {
        QSqlQuery query(*database());
        if (!query.exec(sql)) {
            qWarning() << "Failed to exec table creation SQL query:" << sql << "- error:" << query.lastError().text();
            result = false;
        }
        sql = parseSql(ts);
    }

    return result;
}

bool QMailStorePrivate::ensureVersionInfo()
{
    if (!database()->tables().contains("versioninfo", Qt::CaseInsensitive)) {
        // Use the same version scheme as dbmigrate, in case we need to cooperate later
        QString sql("CREATE TABLE versioninfo ("
                    "   tableName NVARCHAR (255) NOT NULL,"
                    "   versionNum INTEGER NOT NULL,"
                    "   lastUpdated NVARCHAR(20) NOT NULL,"
                    "   PRIMARY KEY(tableName, versionNum))");

        QSqlQuery query(*database());
        if (!query.exec(sql)) {
            qWarning() << "Failed to create versioninfo table - query:" << sql << "- error:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

qint64 QMailStorePrivate::tableVersion(const QString &name) const
{
    QString sql("SELECT COALESCE(MAX(versionNum), 0) FROM versioninfo WHERE tableName=?");

    QSqlQuery query(*database());
    query.prepare(sql);
    query.addBindValue(name);
    if (query.exec() && query.first())
        return query.value(0).value<qint64>();

    qWarning() << "Failed to query versioninfo - query:" << sql << "- error:" << query.lastError().text();
    return 0;
}

bool QMailStorePrivate::setTableVersion(const QString &name, qint64 version)
{
    QString sql("DELETE FROM versioninfo WHERE tableName=?");

    // Delete any existing entry for this table
    QSqlQuery query(*database());
    query.prepare(sql);
    query.addBindValue(name);

    if (!query.exec()) {
        qWarning() << "Failed to delete versioninfo - query:" << sql << "- error:" << query.lastError().text();
        return false;
    } else {
        sql = "INSERT INTO versioninfo (tablename,versionNum,lastUpdated) VALUES (?,?,?)";

        // Insert the updated info
        query = QSqlQuery(*database());
        query.prepare(sql);
        query.addBindValue(name);
        query.addBindValue(version);
        query.addBindValue(QDateTime::currentDateTime().toString());

        if (!query.exec()) {
            qWarning() << "Failed to insert versioninfo - query:" << sql << "- error:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

qint64 QMailStorePrivate::incrementTableVersion(const QString &name, qint64 current)
{
    qint64 next = current + 1;

    QString versionInfo("-" + QString::number(current) + "-" + QString::number(next));
    QString scriptName(":/QmfSql/" + database()->driverName() + '/' + name + versionInfo);

    QFile data(scriptName);
    if (!data.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load table upgrade resource:" << name;
    } else {
        if (executeFile(data)) {
            // Update the table version number
            if (setTableVersion(name, next))
                current = next;
        }
    }

    return current;
}

bool QMailStorePrivate::upgradeTimeStampToUtc()
{
    QMailMessageIdList allMessageIds = queryMessages(QMailMessageKey(), QMailMessageSortKey(), 0, 0);

    qMailLog(Messaging) << Q_FUNC_INFO << "Time stamp for " << allMessageIds.count() << " will be updated ";

    QMailMessageKey::Properties updateDateProperties = QMailMessageKey::TimeStamp | QMailMessageKey::ReceptionTimeStamp;
    foreach(const QMailMessageId &updateId, allMessageIds)
    {
        const QMailMessageMetaData m(updateId);
        const MessageValueExtractor<QMailMessageMetaData> extractor(m);
        QVariantList bindValues;
        bindValues << QDateTime((extractor.date()).value<QDateTime>()).toUTC();
        bindValues << QDateTime((extractor.receivedDate()).value<QDateTime>()).toUTC();
        bindValues << extractor.id();
        QString sql("UPDATE mailmessages SET %1 WHERE id=?");

        QSqlQuery query(simpleQuery(sql.arg(expandProperties(updateDateProperties, true)),
                                    bindValues,
                                    "updateMessage mailmessages update"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    return true;
}

bool QMailStorePrivate::upgradeTableVersion(const QString &name, qint64 current, qint64 final)
{
    while (current < final) {
        int newVersion = incrementTableVersion(name, current);
        if (newVersion == current) {
            qWarning() << "Failed to increment table version from:" << current << "(" << name << ")";
            break;
        } else {
            current = newVersion;
        }
    }

    return (current == final);
}

bool QMailStorePrivate::fullThreadTableUpdate()
{
    //Clear mailthreads table.
    {
        QSqlQuery query(simpleQuery("DELETE FROM mailthreads",
                                    "fullThreadTableUpdate clear mailthreads table query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    //Clear mailthread ids and responseid for all messages
    {
        QSqlQuery query(simpleQuery("UPDATE mailmessages SET parentthreadid = 0 , responseid = 0",
                                    "fullThreadTableUpdate clear mailmessages parentthreadid query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    //Clear missingancestors table
    {
        QSqlQuery query(simpleQuery("DELETE FROM missingancestors",
                                    "fullThreadTableUpdate clear missingancestors table query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    //Clear missingmessages table
    {
        QSqlQuery query(simpleQuery("DELETE FROM missingmessages",
                                    "fullThreadTableUpdate clear missingmessages table query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    //Clear mailsubjects table
    {
        QSqlQuery query(simpleQuery("DELETE FROM mailsubjects",
                                    "fullThreadTableUpdate clear mailsubjects table query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    //Clear mailthreadsubjects table
    {
        QSqlQuery query(simpleQuery("DELETE FROM mailthreadsubjects",
                                    "fullThreadTableUpdate clear mailthreadsubjects table query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
    //Get all messages
    QMailMessageMetaDataList messagesList;
    {
        QSqlQuery query(simpleQuery("SELECT id,sender,subject,stamp,status,parentaccountid,preview FROM mailmessages ORDER BY stamp",
                                    "fullThreadTableUpdate select all messages query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
        while (query.next()) {
            QMailMessageMetaData data;
            data.setId(QMailMessageId(query.value(0).toULongLong()));
            data.setFrom(QMailAddress(query.value(1).toString()));
            data.setSubject(query.value(2).toString());
            data.setDate(QMailTimeStamp(query.value(3).toDateTime()));
            data.setStatus(query.value(4).toULongLong());
            data.setParentAccountId(QMailAccountId(query.value(5).toULongLong()));
            data.setPreview(query.value(6).toString());
            messagesList.append(data);
        }
    }
    //Re-create all threads
    {
        QMailMessageMetaDataList::iterator it = messagesList.begin();
        QMailMessageMetaDataList::iterator end = messagesList.end();
        for ( ; it != end; ++it) {
            QMailMessageMetaData* metaData = &(*it);
            QMailMessage message(metaData->id());
            QString identifier(identifierValue(message.headerFieldText("Message-ID")));
            QStringList references(identifierValues(message.headerFieldText("References")));
            bool replyOrForward(false);
            QString baseSubject(QMail::baseSubject(metaData->subject(), &replyOrForward));
            QStringList missingReferences;
            bool missingAncestor(false);
            // Does this message have any references to resolve?
            AttemptResult result = messagePredecessor(metaData, references, baseSubject, replyOrForward, &missingReferences, &missingAncestor);
            if (result != Success)
                return false;
            //Hack. We are using sql query here, because
            //metaData->setParentThreadId(QMailMessageMetaData(predecessorMsgId).parentThreadId());
            // is not working properly in messagePredecessor()
            if (metaData->inResponseTo().toULongLong() != 0) {
                QSqlQuery query(simpleQuery(QString("SELECT parentthreadid FROM mailmessages WHERE id = %1").arg(metaData->inResponseTo().toULongLong()),
                                            "fullThreadTableUpdate select all messages query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return false;
                if (query.next())
                    metaData->setParentThreadId(QMailThreadId(query.value(0).toULongLong()));
                else
                    qWarning() << Q_FUNC_INFO << "there is no message with id" << metaData->inResponseTo().toULongLong();
            }

            //QMailThreadId.isValid() is not working properly here
            if (metaData->parentThreadId().toULongLong() != 0) {
                QMailThread thread(metaData->parentThreadId());
                QString senders = QMailAddress::toStringList(thread.senders()).join(",");
                const QString &newSender = metaData->from().toString();

                if (!senders.contains(newSender)) {
                    senders.append(QString(",") + newSender);
                }

                QString sql("UPDATE mailthreads SET messagecount = messagecount + 1, senders = (?), preview = (?), lastdate = (?)"
                            + ((metaData->status() & QMailMessage::Read) ? QString("") : QString(", unreadcount = unreadcount + 1 "))
                            + QString(", status = (status | %1)").arg(metaData->status()) + " WHERE id= (?)");
                QVariantList bindValues;
                bindValues << QVariant(senders)
                           << QVariant(metaData->preview())
                           << QVariant(metaData->date().toUTC())
                           << QVariant(metaData->parentThreadId().toULongLong());
                QSqlQuery query = simpleQuery(sql, bindValues, "fullThreadTableUpdate update thread");

                if (query.lastError().type() != QSqlError::NoError)
                    return false;
            } else {
                quint64 threadId = 0;

                // Add a new thread for this message
                QMap<QString, QVariant> values;
                values.insert("messagecount", 1);
                values.insert("unreadcount", ((metaData->status() & QMailMessage::Read) ? 0 : 1));
                values.insert("serveruid", "");
                values.insert("parentaccountid", metaData->parentAccountId().toULongLong());
                values.insert("subject", metaData->subject());
                values.insert("preview", metaData->preview());
                values.insert("senders", metaData->from().toString());
                values.insert("lastdate", metaData->date().toUTC());
                values.insert("starteddate", metaData->date().toUTC());
                values.insert("status", metaData->status());
                const QString &columns = QStringList(values.keys()).join(",");
                QSqlQuery query(simpleQuery(QString("INSERT INTO mailthreads (%1) VALUES %2").arg(columns).arg(expandValueList(values.count())),
                                            values.values(),
                                            "fullThreadTableUpdate mailthreads insert query"));

                if (query.lastError().type() != QSqlError::NoError)
                    return false;

                threadId = extractValue<quint64>(query.lastInsertId());

                Q_ASSERT(threadId != 0);
                metaData->setParentThreadId(QMailThreadId(threadId));
            }
            //Update message's values
            {
                QVariantList bindValues;
                bindValues << QVariant(metaData->parentThreadId().toULongLong())
                           << QVariant(metaData->inResponseTo().toULongLong())
                           << QVariant(metaData->id().toULongLong());
                QSqlQuery query = simpleQuery(QString("UPDATE mailmessages SET parentthreadid = ? , responseid = ? WHERE id = ?"),
                                              bindValues,
                                              "fullThreadTableUpdate mailmessages update query");

                if (query.lastError().type() != QSqlError::NoError)
                    return false;
            }
            if (!baseSubject.isEmpty()) {
                // Ensure that this subject is in the subjects table
                AttemptResult result = registerSubject(baseSubject, metaData->id().toULongLong(), metaData->inResponseTo(), missingAncestor);
                if (result != Success)
                    return false;
            }
            // See if this message resolves any missing message items
            QMailMessageIdList updatedMessageIds;
            result = resolveMissingMessages(identifier, metaData->inResponseTo(), baseSubject, *metaData, &updatedMessageIds);
            if (result != Success)
                return false;
            if (!missingReferences.isEmpty()) {
                // Add the missing references to the missing messages table
                QVariantList refs;
                QVariantList levels;

                int level = missingReferences.count();
                foreach (const QString &ref, missingReferences) {
                    refs.append(QVariant(ref));
                    levels.append(QVariant(--level));
                }

                QString sql("INSERT INTO missingmessages (id,identifier,level) VALUES (%1,?,?)");
                QSqlQuery query(batchQuery(sql.arg(QString::number(metaData->id().toULongLong())),
                                           QVariantList() << QVariant(refs) << QVariant(levels),
                                           "fullThreadTableUpdate missingmessages insert query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return false;
            }
        }
    }
    return true;
}

bool QMailStorePrivate::createTable(const QString &name)
{
    bool result = true;

    // load schema.
    QFile data(":/QmfSql/" + database()->driverName() + '/' + name);
    if (!data.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load table schema resource:" << name;
        result = false;
    } else {
        result = executeFile(data);
    }

    return result;
}

bool QMailStorePrivate::setupTables(const QList<TableInfo> &tableList)
{
    bool result = true;

    QStringList tables = database()->tables();

    foreach (const TableInfo &table, tableList) {
        const QString &tableName(table.first);
        qint64 version(table.second);

        if (!tables.contains(tableName, Qt::CaseInsensitive)) {
            // Create the table
            result &= (createTable(tableName) && setTableVersion(tableName, version));
        } else {
            // Ensure the table does not have an incompatible version
            qint64 dbVersion = tableVersion(tableName);

            if (dbVersion == 0) {
                qWarning() << "No version for existing table:" << tableName;
                result = false;
            } else if (dbVersion != version) {
                if (version > dbVersion) {

                    //  Migration from localTime time stamp to Utc
                    if (tableName == "mailmessages" && dbVersion <= 113 && version >= 114) {
                        //upgrade time stamp
                        if (!upgradeTimeStampToUtc()) {
                            qWarning() << Q_FUNC_INFO << "Can't upgrade time stamp";
                            result = false;
                        }
                    }

                    // Try upgrading the table
                    result = result && upgradeTableVersion(tableName, dbVersion, version);
                    qWarning() << (result ? "Upgraded" : "Unable to upgrade") << "version for table:" << tableName << " from" << dbVersion << "to" << version;
                } else {
                    qWarning() << "Incompatible version for table:" << tableName << "- existing" << dbVersion << "!=" << version;
                    result = false;
                }
            }
        }
    }

    // quick and dirty check if they're using an old version
    // TODO: remove this
    QSqlQuery query(simpleQuery("SELECT count(*) FROM sqlite_master WHERE `type` = \"table\" AND `name` = \"mailmessages\" AND `sql` LIKE \"%latestinconversation%\"", "old check"));
    if (query.next()) {
        if (query.value(0).toInt() != 0) {
            qFatal("Unsupported database. Please delete the %s directory and try again.", qPrintable(QMail::dataPath()));
        }
    } else {
        qWarning() << "Failure running check";
    }

    return result;
}

bool QMailStorePrivate::setupFolders(const QList<FolderInfo> &folderList)
{
    QSet<quint64> folderIds;

    {
        // TODO: Perhaps we should search only for existing folders?
        QSqlQuery query(simpleQuery("SELECT id FROM mailfolders", 
                                    "folder ids query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        while (query.next())
            folderIds.insert(query.value(0).toULongLong());
    }

    foreach (const FolderInfo &folder, folderList) {
        if (folderIds.contains(folder.id()))
            continue;
        QSqlQuery query(simpleQuery("INSERT INTO mailfolders (id,name,parentid,parentaccountid,displayname,status,servercount,serverunreadcount,serverundiscoveredcount) VALUES (?,?,?,?,?,?,?,?,?)",
                                    QVariantList() << folder.id()
                                                   << folder.name()
                                                   << quint64(0)
                                                   << quint64(0)
                                                   << QString()
                                                   << folder.status()
                                                   << int(0)
                                                   << int(0)
                                                   << int(0),
                                    "setupFolders insert query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    return true;
}

bool QMailStorePrivate::purgeMissingAncestors()
{
    QString sql("DELETE FROM missingancestors WHERE state=1");

    QSqlQuery query(*database());
    query.prepare(sql);
    if (!query.exec()) {
        qWarning() << "Failed to purge missing ancestors - query:" << sql << "- error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool QMailStorePrivate::purgeObsoleteFiles()
{
    QStringList identifiers;

    {
        QString sql("SELECT mailfile FROM obsoletefiles");

        QSqlQuery query(*database());
        if (!query.exec(sql)) {
            qWarning() << "Failed to purge obsolete files - query:" << sql << "- error:" << query.lastError().text();
            return false;
        } else {
            while (query.next()) {
                identifiers.append(query.value(0).toString());
            }
        }
    }

    if (!identifiers.isEmpty()) {
        QMap<QString, QStringList> uriElements;

        foreach (const QString& contentUri, identifiers) {
            uriElements[extractUriElements(contentUri).first].append(extractUriElements(contentUri).second);
        }

        for ( QMap<QString, QStringList>::iterator it(uriElements.begin()) ; it != uriElements.end() ; ++it)
        {
            QStringList schemes(QStringList() << QMailContentManagerFactory::defaultFilterScheme()
                                              << it.key()
                                              << QMailContentManagerFactory::defaultIndexerScheme());

            foreach(QString const& scheme, schemes)
            {
                if (!scheme.isEmpty()) {
                    QMailContentManager *manager(QMailContentManagerFactory::create(scheme));
                    if (!manager)
                        qWarning() << "Unable to create content manager for scheme:" << scheme;
                    else {
                        if (manager->remove(*it) != QMailStore::NoError) {
                            qWarning() << "Unable to remove obsolete message contents:" << *it;
                        }
                    }
                }
            }
        }




         QString sql("DELETE FROM obsoletefiles");

        QSqlQuery query(*database());
        if (!query.exec(sql)) {
            qWarning() << "Failed to purge obsolete file - query:" << sql << "- error:" << query.lastError().text();
            return false;
        }
    }


    return true;
}

bool QMailStorePrivate::performMaintenanceTask(const QString &task, uint secondsFrequency, bool (QMailStorePrivate::*func)(void))
{
    QDateTime lastPerformed(QDateTime::fromTime_t(0));

    {
        QString sql("SELECT performed FROM maintenancerecord WHERE task=?");

        QSqlQuery query(*database());
        query.prepare(sql);
        query.addBindValue(task);
        if (!query.exec()) {
            qWarning() << "Failed to query performed timestamp - query:" << sql << "- error:" << query.lastError().text();
            return false;
        } else {
            if (query.first()) {
                lastPerformed = query.value(0).value<QDateTime>();
            }
        }
    }

    QDateTime nextTime(lastPerformed.addSecs(secondsFrequency));
    QDateTime currentTime(QDateTime::currentDateTime());
    if (currentTime >= nextTime) {
        if (!(this->*func)()) {
            return false;
        }

        // Update the timestamp
        QString sql;
        if (lastPerformed.toTime_t() == 0) {
            sql = "INSERT INTO maintenancerecord (performed,task) VALUES(?,?)";
        } else {
            sql = "UPDATE maintenancerecord SET performed=? WHERE task=?";
        }

        QSqlQuery query(*database());
        query.prepare(sql);
        query.addBindValue(currentTime);
        query.addBindValue(task);
        if (!query.exec()) {
            qWarning() << "Failed to update performed timestamp - query:" << sql << "- error:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

bool QMailStorePrivate::performMaintenance()
{
    // Perform this task no more than once every 24 hours
    if (!performMaintenanceTask("purge missing ancestors", 24*60*60, &QMailStorePrivate::purgeMissingAncestors))
        return false;

    // Perform this task no more than once every hour
    if (!performMaintenanceTask("purge obsolete files", 60*60, &QMailStorePrivate::purgeObsoleteFiles))
        return false;

    return true;
}

QString QMailStorePrivate::parseSql(QTextStream& ts)
{
    QString qry = "";
    while(!ts.atEnd())
    {
        QString line = ts.readLine();
        // comment, remove.
        if (line.contains(QLatin1String("--")))
            line.truncate (line.indexOf (QLatin1String("--")));
        if (line.trimmed ().length () == 0)
            continue;
        qry += line;
        
        if ( line.contains( ';' ) == false) 
            qry += ' ';
        else
            return qry;
    }
    return qry;
}

QString QMailStorePrivate::expandValueList(const QVariantList& valueList)
{
    Q_ASSERT(!valueList.isEmpty());
    return expandValueList(valueList.count());
}

QString QMailStorePrivate::expandValueList(int valueCount)
{
    Q_ASSERT(valueCount > 0);

    if (valueCount == 1) {
        return "(?)";
    } else {
        QString inList = " (?";
        for (int i = 1; i < valueCount; ++i)
            inList += ",?";
        inList += ")";
        return inList;
    }
}

QString QMailStorePrivate::expandProperties(const QMailMessageKey::Properties& prop, bool update) const 
{
    QString out;

    // The ContentScheme and ContentIdentifier properties map to the same field
    QMailMessageKey::Properties properties(prop);
    if ((properties & QMailMessageKey::ContentScheme) && (properties & QMailMessageKey::ContentIdentifier))
        properties &= ~QMailMessageKey::ContentIdentifier;

    const QMailStorePrivate::MessagePropertyMap &map(messagePropertyMap());
    foreach (QMailMessageKey::Property p, messagePropertyList()) {
        if (properties & p) {
            if (!out.isEmpty())
                out += ",";                     
            out += map.value(p);
            if (update)
                out += "=?";
        }
    }

    return out;
}

QString QMailStorePrivate::expandProperties(const QMailThreadKey::Properties& prop, bool update) const
{
    QString out;

    // The ContentScheme and ContentIdentifier properties map to the same field
    QMailThreadKey::Properties properties(prop);
    const QMailStorePrivate::ThreadPropertyMap &map(threadPropertyMap());
    foreach (const QMailThreadKey::Property& p, threadPropertyList()) {
        if (properties & p) {
            if (!out.isEmpty())
                out += ",";
            out += map.value(p);
            if (update)
                out += "=?";
        }
    }

    return out;
}

bool QMailStorePrivate::addAccount(QMailAccount *account, QMailAccountConfiguration *config,
                                   QMailAccountIdList *addedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptAddAccount, this,
                                        account, config, 
                                        addedAccountIds),
                                   "addAccount");
}

bool QMailStorePrivate::addFolder(QMailFolder *folder,
                                  QMailFolderIdList *addedFolderIds, QMailAccountIdList *modifiedAccountIds)
{   
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptAddFolder, this,
                                        folder, 
                                        addedFolderIds, modifiedAccountIds),
                                   "addFolder");
}

bool QMailStorePrivate::addMessages(const QList<QMailMessage *> &messages,
                                    QMailMessageIdList *addedMessageIds, QMailThreadIdList *addedThreadIds, QMailMessageIdList *updatedMessageIds, QMailThreadIdList *updatedThreadIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds)
{
    // Resolve from overloaded member functions:
    AttemptResult (QMailStorePrivate::*func)(QMailMessage*, QString const&, QStringList const&, AttemptAddMessageOut*, Transaction&, bool) = &QMailStorePrivate::attemptAddMessage;

    QSet<QString> contentSchemes;

    AttemptAddMessageOut container(addedMessageIds, addedThreadIds, updatedMessageIds, updatedThreadIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds);

    Transaction t(this);


    foreach (QMailMessage *message, messages) {
        // TODO: remove hack to force eager preview generation
        message->preview();

        // Find the message identifier and references from the header
        QString identifier(identifierValue(message->headerFieldText("Message-ID")));
        QStringList references(identifierValues(message->headerFieldText("References")));
        QString predecessor(identifierValue(message->headerFieldText("In-Reply-To")));
        if (!predecessor.isEmpty()) {
            if (references.isEmpty() || (references.last() != predecessor)) {
                references.append(predecessor);
            }
        }

        if (!repeatedly<WriteAccess>(bind(func, this, message, cref(identifier), cref(references), &container),
                                     "addMessages",
                                     &t)) {
            return false;
        }

        if(!message->contentScheme().isEmpty())
            contentSchemes.insert(message->contentScheme());
    }

    // Ensure that the content manager makes the changes durable before we return
    foreach (const QString &scheme, contentSchemes) {
        if (QMailContentManager *contentManager = QMailContentManagerFactory::create(scheme)) {
            QMailStore::ErrorCode code = contentManager->ensureDurability();
            if (code != QMailStore::NoError) {
                setLastError(code);
                qWarning() << "Unable to ensure message content durability for scheme:" << scheme;
                return false;
            }
        } else {
            setLastError(QMailStore::FrameworkFault);
            qWarning() << "Unable to create content manager for scheme:" << scheme;
            return false;
        }
    }

    if (!t.commit()) {
        qWarning() << "Unable to commit successful addMessages!";
        return false;
    }

    return true;
}

bool QMailStorePrivate::addMessages(const QList<QMailMessageMetaData *> &messages,
                                    QMailMessageIdList *addedMessageIds, QMailThreadIdList *addedThreadIds, QMailMessageIdList *updatedMessageIds, QMailThreadIdList *updatedThreadIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds)
{
    // Resolve from overloaded member functions:
    AttemptResult (QMailStorePrivate::*func)(QMailMessageMetaData*, const QString&, const QStringList&, AttemptAddMessageOut*, Transaction&, bool) = &QMailStorePrivate::attemptAddMessage;

    AttemptAddMessageOut out(addedMessageIds, addedThreadIds, updatedMessageIds, updatedThreadIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds);

    Transaction t(this);

    foreach (QMailMessageMetaData *metaData, messages) {
        QString identifier;
        QStringList references;

        if (!repeatedly<WriteAccess>(bind(func, this, 
                                          metaData, cref(identifier), cref(references),
                                          &out),
                                     "addMessages",
                                     &t)) {
            return false;
        }
    }

    if (!t.commit()) {
        qWarning() << "Unable to commit successful addMessages!";
        return false;
    }

    return true;
}

bool QMailStorePrivate::addThread(QMailThread *thread, QMailThreadIdList *addedThreadIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptAddThread, this,
                                        thread,
                                        addedThreadIds),
                                   "addThread");
}

bool QMailStorePrivate::removeAccounts(const QMailAccountKey &key,
                                       QMailAccountIdList *deletedAccountIds, QMailFolderIdList *deletedFolderIds, QMailThreadIdList *deletedThreadIds, QMailMessageIdList *deletedMessageIds, QMailMessageIdList *updatedMessageIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds)
{
    AttemptRemoveAccountOut out(deletedAccountIds, deletedFolderIds,  deletedThreadIds, deletedMessageIds, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds);

    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRemoveAccounts, this,
                                        cref(key),
                                        &out),
                                   "removeAccounts");
}

bool QMailStorePrivate::removeFolders(const QMailFolderKey &key, QMailStore::MessageRemovalOption option,
                                      QMailFolderIdList *deletedFolderIds, QMailMessageIdList *deletedMessageIds, QMailThreadIdList *deletedThreadIds, QMailMessageIdList *updatedMessageIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds)
{

    AttemptRemoveFoldersOut out(deletedFolderIds, deletedMessageIds, deletedThreadIds, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds);


    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRemoveFolders, this,
                                        cref(key), option, 
                                        &out),
                                   "removeFolders");
}

bool QMailStorePrivate::removeMessages(const QMailMessageKey &key, QMailStore::MessageRemovalOption option,
                                       QMailMessageIdList *deletedMessageIds, QMailThreadIdList* deletedThreadIds, QMailMessageIdList *updatedMessageIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRemoveMessages, this,
                                        cref(key), option,
                                        deletedMessageIds, deletedThreadIds, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds),
                                   "removeMessages");
}

bool QMailStorePrivate::removeThreads(const QMailThreadKey &key, QMailStore::MessageRemovalOption option,
                               QMailThreadIdList *deletedThreads, QMailMessageIdList *deletedMessageIds, QMailMessageIdList *updatedMessageIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds)
{
    AttemptRemoveThreadsOut out(deletedThreads, deletedMessageIds, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds);

    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRemoveThreads, this,
                                        cref(key), option,
                                        &out),
                                   "removeMessages");
}


bool QMailStorePrivate::updateAccount(QMailAccount *account, QMailAccountConfiguration *config,
                                      QMailAccountIdList *updatedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateAccount, this, 
                                        account, config, 
                                        updatedAccountIds), 
                                   "updateAccount");
}

bool QMailStorePrivate::updateAccountConfiguration(QMailAccountConfiguration *config,
                                                   QMailAccountIdList *updatedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateAccount, this, 
                                        reinterpret_cast<QMailAccount*>(0), config, 
                                        updatedAccountIds), 
                                   "updateAccount");
}

bool QMailStorePrivate::updateFolder(QMailFolder *folder,
                                     QMailFolderIdList *updatedFolderIds, QMailAccountIdList *modifiedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateFolder, this, 
                                        folder, 
                                        updatedFolderIds, modifiedAccountIds), 
                                   "updateFolder");
}

bool QMailStorePrivate::updateThread(QMailThread *t,
                              QMailThreadIdList *updatedThreadIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateThread, this,
                                        t,
                                        updatedThreadIds),
                                   "updateThread");
}

bool QMailStorePrivate::updateMessages(const QList<QPair<QMailMessageMetaData*, QMailMessage*> > &messages,
                                       QMailMessageIdList *updatedMessageIds, QMailThreadIdList *modifiedThreads, QMailMessageIdList *modifiedMessageIds, QMailFolderIdList *modifiedFolderIds, QMailAccountIdList *modifiedAccountIds)
{
    QMap<QString, QStringList> contentSyncLater;
    QMap<QString, QStringList> contentRemoveLater;

    Transaction t(this);

    typedef QPair<QMailMessageMetaData*, QMailMessage*> PairType;

    foreach (const PairType &pair, messages) {
        if (!repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateMessage, this,
                                          pair.first, pair.second,
                                          updatedMessageIds, modifiedThreads, modifiedMessageIds, modifiedFolderIds, modifiedAccountIds, &contentRemoveLater),
                                     "updateMessages",
                                     &t)) {
            return false;
        }
    }


    foreach (const PairType &pair, messages) {
        QString scheme(pair.first->contentScheme());

        QMap<QString, QStringList>::iterator it(contentSyncLater.find(scheme));
        if (contentSyncLater.find(scheme) != contentSyncLater.end()) {
            it.value().append(pair.first->contentIdentifier());
        } else {
            contentSyncLater.insert(scheme, QStringList() << pair.first->contentIdentifier());
        }
    }

    for (QMap<QString, QStringList>::const_iterator it(contentSyncLater.begin()); it != contentSyncLater.end() ; ++it) {
        if (QMailContentManager *contentManager = QMailContentManagerFactory::create(it.key())) {
            QMailStore::ErrorCode code = contentManager->ensureDurability(it.value());
            if (code != QMailStore::NoError) {
                setLastError(code);
                qWarning() << "Unable to ensure message content durability for scheme:" << it.key();
                return false;
            }
        } else {
            setLastError(QMailStore::FrameworkFault);
            qWarning() << "Unable to create content manager for scheme:" << it.key();
            return false;
        }
    }

    for (QMap<QString, QStringList>::const_iterator it(contentRemoveLater.begin()); it != contentRemoveLater.end() ; ++it) {
        if (QMailContentManager *contentManager = QMailContentManagerFactory::create(it.key())) {
            QMailStore::ErrorCode code = contentManager->remove(it.value());
            if (code != QMailStore::NoError) {
                setLastError(code);
                qWarning() << "Unable to ensure message content durability for scheme:" << it.key();
                return false;
            }
        } else {
            setLastError(QMailStore::FrameworkFault);
            qWarning() << "Unable to create content manager for scheme:" << it.key();
            return false;
        }
    }



    if (!t.commit()) {
        qWarning() << "Unable to commit successful updateMessages!";
        return false;
    }

    return true;
}

bool QMailStorePrivate::updateMessagesMetaData(const QMailMessageKey &key, const QMailMessageKey::Properties &properties, const QMailMessageMetaData &data,
                                               QMailMessageIdList *updatedMessageIds, QMailThreadIdList *deletedThreads, QMailThreadIdList *modifiedThreads, QMailFolderIdList *modifiedFolderIds, QMailAccountIdList *modifiedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateMessagesMetaData, this,
                                        cref(key), cref(properties), cref(data),
                                        updatedMessageIds, deletedThreads, modifiedThreads, modifiedFolderIds, modifiedAccountIds),
                                   "updateMessagesMetaData");
}

bool QMailStorePrivate::updateMessagesMetaData(const QMailMessageKey &key, quint64 status, bool set,
                                               QMailMessageIdList *updatedMessageIds, QMailThreadIdList *modifiedThreads, QMailFolderIdList *modifiedFolderIds, QMailAccountIdList *modifiedAccountIds)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptUpdateMessagesStatus, this,
                                        cref(key), status, set,
                                        updatedMessageIds, modifiedThreads, modifiedFolderIds, modifiedAccountIds),
                                   "updateMessagesMetaData"); // not 'updateMessagesStatus', due to function name exported by QMailStore
}

bool QMailStorePrivate::ensureDurability()
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptEnsureDurability, this),
                                       "ensureDurability");
}

void QMailStorePrivate::unloadDatabase()
{
    if (databaseptr) {
        shrinkMemory();
        databaseptr->close();
        delete databaseptr;
        databaseptr = 0;
    }
    // Clear all caches
    accountCache.clear();
    folderCache.clear();
    messageCache.clear();
    uidCache.clear();
    threadCache.clear();
    lastQueryMessageResult.clear();
    lastQueryThreadResult.clear();
    requiredTableKeys.clear();
    expiredTableKeys.clear();
    // Close database
    QMail::closeDatabase();
    databaseUnloadTimer.stop();
#if defined(Q_OS_LINUX)
    malloc_trim(0);
#endif
}

bool QMailStorePrivate::shrinkMemory()
{
#if defined(Q_USE_SQLITE)
    QSqlQuery query( *database() );
    if (!query.exec(QLatin1String("PRAGMA shrink_memory"))) {
        qWarning() << "Unable to shrink memory" << query.lastQuery().simplified();
        return false;
    }
#endif
    return true;
}

void QMailStorePrivate::lock()
{
    Q_ASSERT(globalLocks >= 0);
    if (++globalLocks == 1)
        databaseMutex().lock();
}

void QMailStorePrivate::unlock()
{
    if (--globalLocks == 0) {
        databaseMutex().unlock();
    } else if (globalLocks < 0) {
        qWarning() << "Unable to unlock when lock was not called (in this process)";
        globalLocks = 0;
    }
}

bool QMailStorePrivate::purgeMessageRemovalRecords(const QMailAccountId &accountId, const QStringList &serverUids)
{
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptPurgeMessageRemovalRecords, this, 
                                        cref(accountId), cref(serverUids)), 
                                   "purgeMessageRemovalRecords");
}

int QMailStorePrivate::countAccounts(const QMailAccountKey &key) const
{
    int result(0);
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptCountAccounts, const_cast<QMailStorePrivate*>(this), 
                                cref(key), &result), 
                           "countAccounts");
    return result;
}

int QMailStorePrivate::countFolders(const QMailFolderKey &key) const
{
    int result(0);
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptCountFolders, const_cast<QMailStorePrivate*>(this), 
                                cref(key), &result), 
                           "countFolders");
    return result;
}

int QMailStorePrivate::countMessages(const QMailMessageKey &key) const
{
    int result(0);
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptCountMessages, const_cast<QMailStorePrivate*>(this), 
                                cref(key), &result), 
                           "countMessages");
    return result;
}

int QMailStorePrivate::countThreads(const QMailThreadKey &key) const
{
    int result(0);
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptCountThreads, const_cast<QMailStorePrivate*>(this),
                                cref(key), &result),
                           "countThreads");
    return result;
}

int QMailStorePrivate::sizeOfMessages(const QMailMessageKey &key) const
{
    int result(0);
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptSizeOfMessages, const_cast<QMailStorePrivate*>(this), 
                                cref(key), &result), 
                           "sizeOfMessages");
    return result;
}

QMailAccountIdList QMailStorePrivate::queryAccounts(const QMailAccountKey &key, const QMailAccountSortKey &sortKey, uint limit, uint offset) const
{
    QMailAccountIdList ids;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptQueryAccounts, const_cast<QMailStorePrivate*>(this), 
                                cref(key), cref(sortKey), limit, offset, &ids), 
                           "queryAccounts");
    return ids;
}

QMailFolderIdList QMailStorePrivate::queryFolders(const QMailFolderKey &key, const QMailFolderSortKey &sortKey, uint limit, uint offset) const
{
    QMailFolderIdList ids;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptQueryFolders, const_cast<QMailStorePrivate*>(this), 
                                cref(key), cref(sortKey), limit, offset, &ids), 
                           "queryFolders");
    return ids;
}

QMailMessageIdList QMailStorePrivate::queryMessages(const QMailMessageKey &key, const QMailMessageSortKey &sortKey, uint limit, uint offset) const
{
    QMailMessageIdList ids;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptQueryMessages, const_cast<QMailStorePrivate*>(this), 
                                cref(key), cref(sortKey), limit, offset, &ids), 
                           "queryMessages");
    return ids;
}

QMailThreadIdList QMailStorePrivate::queryThreads(const QMailThreadKey &key, const QMailThreadSortKey &sortKey, uint limit, uint offset) const
{
    QMailThreadIdList ids;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptQueryThreads, const_cast<QMailStorePrivate*>(this),
                                cref(key), cref(sortKey), limit, offset, &ids),
                           "queryFolders");
    return ids;
}

QMailAccount QMailStorePrivate::account(const QMailAccountId &id) const
{
    if (accountCache.contains(id))
        return accountCache.lookup(id);

    QMailAccount account;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptAccount, const_cast<QMailStorePrivate*>(this), 
                                cref(id), &account), 
                           "account");
    return account;
}

QMailAccountConfiguration QMailStorePrivate::accountConfiguration(const QMailAccountId &id) const
{
    QMailAccountConfiguration config;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptAccountConfiguration, const_cast<QMailStorePrivate*>(this), 
                                cref(id), &config), 
                           "accountConfiguration");
    return config;
}

QMailFolder QMailStorePrivate::folder(const QMailFolderId &id) const
{
    if (folderCache.contains(id))
        return folderCache.lookup(id);

    QMailFolder folder;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptFolder, const_cast<QMailStorePrivate*>(this), 
                                cref(id), &folder), 
                           "folder");
    return folder;
}

QMailMessage QMailStorePrivate::message(const QMailMessageId &id) const
{
    // Resolve from overloaded member functions:
    AttemptResult (QMailStorePrivate::*func)(const QMailMessageId&, QMailMessage*, ReadLock&) = &QMailStorePrivate::attemptMessage;

    QMailMessage msg;
    repeatedly<ReadAccess>(bind(func, const_cast<QMailStorePrivate*>(this), 
                                cref(id), &msg), 
                           "message(id)");
    return msg;
}

QMailMessage QMailStorePrivate::message(const QString &uid, const QMailAccountId &accountId) const
{
    // Resolve from overloaded member functions:
    AttemptResult (QMailStorePrivate::*func)(const QString&, const QMailAccountId&, QMailMessage*, ReadLock&) = &QMailStorePrivate::attemptMessage;

    QMailMessage msg;
    repeatedly<ReadAccess>(bind(func, const_cast<QMailStorePrivate*>(this), 
                                cref(uid), cref(accountId), &msg), 
                           "message(uid, accountId)");
    return msg;
}

QMailThread QMailStorePrivate::thread(const QMailThreadId &id) const
{
    if (threadCache.contains(id))
        return threadCache.lookup(id);

    // if not in the cache, then preload the cache with the id and its most likely requested siblings
    preloadThreadCache(id);

    return threadCache.lookup(id);
}

QMailMessageMetaData QMailStorePrivate::messageMetaData(const QMailMessageId &id) const
{
    if (messageCache.contains(id)) {
        return messageCache.lookup(id);
    }

    //if not in the cache, then preload the cache with the id and its most likely requested siblings
    preloadHeaderCache(id);

    return messageCache.lookup(id);
}

QMailMessageMetaData QMailStorePrivate::messageMetaData(const QString &uid, const QMailAccountId &accountId) const
{
    QMailMessageMetaData metaData;
    bool success;

    QPair<QMailAccountId, QString> key(accountId, uid);
    if (uidCache.contains(key)) {
        // We can look this message up in the cache
        QMailMessageId id(uidCache.lookup(key));

        if (messageCache.contains(id))
            return messageCache.lookup(id);

        // Resolve from overloaded member functions:
        AttemptResult (QMailStorePrivate::*func)(const QMailMessageId&, QMailMessageMetaData*, ReadLock&) = &QMailStorePrivate::attemptMessageMetaData;

        success = repeatedly<ReadAccess>(bind(func, const_cast<QMailStorePrivate*>(this), 
                                              cref(id), &metaData), 
                                         "messageMetaData(id)");
    } else {
        // Resolve from overloaded member functions:
        AttemptResult (QMailStorePrivate::*func)(const QString&, const QMailAccountId&, QMailMessageMetaData*, ReadLock&) = &QMailStorePrivate::attemptMessageMetaData;

        success = repeatedly<ReadAccess>(bind(func, const_cast<QMailStorePrivate*>(this), 
                                              cref(uid), cref(accountId), &metaData), 
                                         "messageMetaData(uid/accountId)");
    }

    if (success) {
        messageCache.insert(metaData);
        uidCache.insert(qMakePair(metaData.parentAccountId(), metaData.serverUid()), metaData.id());
    }

    return metaData;
}

QMailMessageMetaDataList QMailStorePrivate::messagesMetaData(const QMailMessageKey &key, const QMailMessageKey::Properties &properties, QMailStore::ReturnOption option) const
{
    QMailMessageMetaDataList metaData;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptMessagesMetaData, const_cast<QMailStorePrivate*>(this), 
                                cref(key), cref(properties), option, &metaData), 
                           "messagesMetaData");
    return metaData;
}

QMailThreadList QMailStorePrivate::threads(const QMailThreadKey &key, QMailStore::ReturnOption option) const
{
    QMailThreadList result;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptThreads, const_cast<QMailStorePrivate*>(this),
                                cref(key), option, &result),
                           "threads");
    return result;
}

QMailMessageRemovalRecordList QMailStorePrivate::messageRemovalRecords(const QMailAccountId &accountId, const QMailFolderId &folderId) const
{
    QMailMessageRemovalRecordList removalRecords;
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptMessageRemovalRecords, const_cast<QMailStorePrivate*>(this), 
                                cref(accountId), cref(folderId), &removalRecords), 
                           "messageRemovalRecords(accountId, folderId)");
    return removalRecords;
}

bool QMailStorePrivate::registerAccountStatusFlag(const QString &name)
{
    if (accountStatusMask(name) != 0)
        return true;

    quint64 num;

    static const QString context("accountstatus");
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRegisterStatusBit, this,
                                        cref(name), cref(context), 63, false, &num),
                                   "registerAccountStatusBit");
}

quint64 QMailStorePrivate::accountStatusMask(const QString &name) const
{
    static QMap<QString, quint64> statusMap;
    static const QString context("accountstatus");

    return queryStatusMap(name, context, statusMap);
}

bool QMailStorePrivate::registerFolderStatusFlag(const QString &name)
{
    if (folderStatusMask(name) != 0)
        return true;

    quint64 num;

    static const QString context("folderstatus");
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRegisterStatusBit, this,
                                        cref(name), cref(context), 63, false, &num),
                                   "registerFolderStatusBit");
}

quint64 QMailStorePrivate::folderStatusMask(const QString &name) const
{
    static QMap<QString, quint64> statusMap;
    static const QString context("folderstatus");

    return queryStatusMap(name, context, statusMap);
}

bool QMailStorePrivate::registerMessageStatusFlag(const QString &name)
{
    if (messageStatusMask(name) != 0)
        return true;

    quint64 num;

    static const QString context("messagestatus");
    return repeatedly<WriteAccess>(bind(&QMailStorePrivate::attemptRegisterStatusBit, this,
                                        cref(name), cref(context), 63, false, &num),
                                   "registerMessageStatusBit");
}

quint64 QMailStorePrivate::messageStatusMask(const QString &name) const
{
    static QMap<QString, quint64> statusMap;
    static const QString context("messagestatus");

    return queryStatusMap(name, context, statusMap);
}

quint64 QMailStorePrivate::queryStatusMap(const QString &name, const QString &context, QMap<QString, quint64> &map) const
{
    QMap<QString, quint64>::const_iterator it = map.find(name);
    if (it != map.end())
        return it.value();

    int result(0);
    repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptStatusBit, const_cast<QMailStorePrivate*>(this), 
                                cref(name), cref(context), &result), 
                           "folderStatusMask");
    if (result == 0)
        return 0;

    quint64 maskValue = (1 << (result - 1));
    map[name] = maskValue;
    return maskValue;
}

QMailFolderIdList QMailStorePrivate::folderAncestorIds(const QMailFolderIdList& ids, bool inTransaction, AttemptResult *result) const
{
    QMailFolderIdList ancestorIds;

    QMailStorePrivate *self(const_cast<QMailStorePrivate*>(this));
    if (inTransaction) {
        // We can't retry this query after a busy error if we're in a transaction
        ReadLock l(self);
        *result = self->attemptFolderAncestorIds(ids, &ancestorIds, l);
    } else {
        bool ok = repeatedly<ReadAccess>(bind(&QMailStorePrivate::attemptFolderAncestorIds, self,
                                              cref(ids), &ancestorIds), 
                                         "folderAncestorIds");
        if (result)
            *result = ok ? Success : Failure;
    }

    return ancestorIds;
}

void QMailStorePrivate::removeExpiredData(const QMailMessageIdList& messageIds, const QMailThreadIdList& threadIds, const QStringList& contentUris, const QMailFolderIdList& folderIds, const QMailAccountIdList& accountIds)
{
    foreach (const QMailMessageId& id, messageIds) {
        messageCache.remove(id);
    }

    {
        MutexGuard lock(contentManagerMutex());
        lock.lock();

        QMap<QString, QStringList> uriElements;

        foreach (const QString& contentUri, contentUris) {
            uriElements[extractUriElements(contentUri).first].append(extractUriElements(contentUri).second);

            for ( QMap<QString, QStringList>::iterator it(uriElements.begin()) ; it != uriElements.end() ; ++it)
            {
                QStringList schemes(QStringList() << QMailContentManagerFactory::defaultFilterScheme()
                                                  << it.key()
                                                  << QMailContentManagerFactory::defaultIndexerScheme());

                foreach(QString const& scheme, schemes)
                {
                    if (!scheme.isEmpty()) {
                        QMailContentManager *manager(QMailContentManagerFactory::create(scheme));
                        if (!manager)
                            qWarning() << "Unable to create content manager for scheme:" << scheme;
                        else {
                            if (manager->remove(*it) != QMailStore::NoError) {
                                 qWarning() << "Unable to remove expired message contents:" << *it;
                            }
                        }
                    }
                }
            }
        }
    }

    foreach (const QMailThreadId& id, threadIds) {
        threadCache.remove(id);
    }

    foreach (const QMailFolderId& id, folderIds) {
        folderCache.remove(id);
    }

    foreach (const QMailAccountId& id, accountIds) {
        accountCache.remove(id);
    }
}

template<typename AccessType, typename FunctionType>
bool QMailStorePrivate::repeatedly(FunctionType func, const QString &description, Transaction *t) const
{
    static const unsigned int MinRetryDelay = 64;
    static const unsigned int MaxRetryDelay = 2048;
    static const unsigned int MaxAttempts = 100;

    // This function calls the supplied function repeatedly, retrying whenever it
    // returns the DatabaseFailure result and the database's last error is SQLITE_BUSY.
    // It sleeps between repeated attempts, for increasing amounts of time.
    // The argument should be an object allowing nullary invocation returning an
    // AttemptResult value, created with tr1::bind if necessary.

    unsigned int attemptCount = 0;
    unsigned int delay = MinRetryDelay;

     while (true) {
        AttemptResult result;
        if (t) {
            result = evaluate(AccessType(), func, *t);
        } else {
            result = evaluate(AccessType(), func, description, const_cast<QMailStorePrivate*>(this));
        }

        if (result == Success) {
            if (attemptCount > 0) {
                qWarning() << pid << "Able to" << qPrintable(description) << "after" << attemptCount << "failed attempts";
            }
            return true;
        } else if (result == Failure) {
            qWarning() << pid << "Unable to" << qPrintable(description);
            if (lastError() == QMailStore::NoError) {
                setLastError(errorType(AccessType()));
            }
            return false;
        } else { 
            // result == DatabaseFailure
            if (queryError() == Sqlite3BusyErrorNumber) {
                if (attemptCount < MaxAttempts) {
                    qWarning() << pid << "Failed to" << qPrintable(description) << "- busy, pausing to retry";

                    // Pause before we retry
                    QMail::usleep(delay * 1000);
                    if (delay < MaxRetryDelay)
                        delay *= 2;

                    ++attemptCount;
                } else {
                    qWarning() << pid << "Retry count exceeded - failed to" << qPrintable(description);
                    break;
                }
            } else if (queryError() == Sqlite3ConstraintErrorNumber) {
                qWarning() << pid << "Unable to" << qPrintable(description) << "- constraint failure";
                setLastError(QMailStore::ConstraintFailure);
                break;
            } else {
                qWarning() << pid << "Unable to" << qPrintable(description) << "- code:" << queryError();
                break;
            }
        }
    }

    // We experienced a database-related failure
    if (lastError() == QMailStore::NoError) {
        setLastError(QMailStore::FrameworkFault);
    }
    return false;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::addCustomFields(quint64 id, const QMap<QString, QString> &fields, const QString &tableName)
{
    if (!fields.isEmpty()) {
        QVariantList customFields;
        QVariantList customValues;

        // Insert any custom fields belonging to this account
        QMap<QString, QString>::const_iterator it = fields.begin(), end = fields.end();
        for ( ; it != end; ++it) {
            customFields.append(QVariant(it.key()));
            customValues.append(QVariant(it.value()));
        }

        // Batch insert the custom fields
        QString sql("INSERT INTO %1 (id,name,value) VALUES (%2,?,?)");
        QSqlQuery query(batchQuery(sql.arg(tableName).arg(QString::number(id)),
                                   QVariantList() << QVariant(customFields)
                                                  << QVariant(customValues),
                                   QString("%1 custom field insert query").arg(tableName)));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::updateCustomFields(quint64 id, const QMap<QString, QString> &fields, const QString &tableName)
{
    QMap<QString, QString> existing;

    {
        // Find the existing fields
        QString sql("SELECT name,value FROM %1 WHERE id=?");
        QSqlQuery query(simpleQuery(sql.arg(tableName),
                                    QVariantList() << id,
                                    QString("%1 update custom select query").arg(tableName)));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        while (query.next())
            existing.insert(query.value(0).toString(), query.value(1).toString());
    }

    QVariantList obsoleteFields;
    QVariantList modifiedFields;
    QVariantList modifiedValues;
    QVariantList addedFields;
    QVariantList addedValues;

    // Compare the sets
    QMap<QString, QString>::const_iterator fend = fields.end(), eend = existing.end();
    QMap<QString, QString>::const_iterator it = existing.begin();
    for ( ; it != eend; ++it) {
        QMap<QString, QString>::const_iterator current = fields.find(it.key());
        if (current == fend) {
            obsoleteFields.append(QVariant(it.key()));
        } else if (*current != *it) {
            modifiedFields.append(QVariant(current.key()));
            modifiedValues.append(QVariant(current.value()));
        }
    }

    for (it = fields.begin(); it != fend; ++it) {
        if (existing.find(it.key()) == eend) {
            addedFields.append(QVariant(it.key()));
            addedValues.append(QVariant(it.value()));
        }
    }

    if (!obsoleteFields.isEmpty()) {
        // Remove the obsolete fields
        QString sql("DELETE FROM %1 WHERE id=? AND name IN %2 COLLATE NOCASE");
        QSqlQuery query(simpleQuery(sql.arg(tableName).arg(expandValueList(obsoleteFields)),
                                    QVariantList() << id << obsoleteFields,
                                    QString("%1 update custom delete query").arg(tableName)));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    if (!modifiedFields.isEmpty()) {
        // Batch update the modified fields
        QString sql("UPDATE %1 SET value=? WHERE id=%2 AND name=? COLLATE NOCASE");
        QSqlQuery query(batchQuery(sql.arg(tableName).arg(QString::number(id)),
                                   QVariantList() << QVariant(modifiedValues)
                                                  << QVariant(modifiedFields),
                                   QString("%1 update custom update query").arg(tableName)));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    if (!addedFields.isEmpty()) {
        // Batch insert the added fields
        QString sql("INSERT INTO %1 (id,name,value) VALUES (%2,?,?)");
        QSqlQuery query(batchQuery(sql.arg(tableName).arg(QString::number(id)),
                                   QVariantList() << QVariant(addedFields)
                                                  << QVariant(addedValues),
                                   QString("%1 update custom insert query").arg(tableName)));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    return Success;
}

#ifdef USE_ACCOUNTS_QT
QMailStorePrivate::AttemptResult QMailStorePrivate::addAccountCustomFields(QSharedPointer<Accounts::Account>& ssoAccount, const QMap<QString, QString>& fields)
{
    if (!fields.isEmpty()) {
        ssoAccount->beginGroup("customFields");

        // Insert any custom fields belonging to this account
        QMap<QString, QString>::const_iterator it = fields.begin(), end = fields.end();
        for ( ; it != end; ++it) {
            ssoAccount->setValue(it.key(), QVariant(it.value()));
        }
        ssoAccount->endGroup();

        if (!ssoAccount->syncAndBlock())
            return DatabaseFailure;
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::updateAccountCustomFields(QSharedPointer<Accounts::Account>& ssoAccount, const QMap<QString, QString> &fields)
{
    ssoAccount->beginGroup("customFields");

    QMap<QString, QString> existing;
    {
        foreach (const QString& name, ssoAccount->allKeys()) {
             existing.insert(name, ssoAccount->valueAsString(name));
        }
    }

    QVariantList obsoleteFields;
    QVariantList modifiedFields;
    QVariantList modifiedValues;
    QVariantList addedFields;
    QVariantList addedValues;

    // Compare the sets
    QMap<QString, QString>::const_iterator fend = fields.end(), eend = existing.end();
    QMap<QString, QString>::const_iterator it = existing.begin();
    for ( ; it != eend; ++it) {
        QMap<QString, QString>::const_iterator current = fields.find(it.key());
        if (current == fend) {
            obsoleteFields.append(QVariant(it.key()));
        } else if (*current != *it) {
            modifiedFields.append(QVariant(current.key()));
            modifiedValues.append(QVariant(current.value()));
        }
    }

    for (it = fields.begin(); it != fend; ++it) {
        if (existing.find(it.key()) == eend) {
            addedFields.append(QVariant(it.key()));
            addedValues.append(QVariant(it.value()));
        }
    }

    if (!obsoleteFields.isEmpty()) {
        // Remove the obsolete fields
        foreach (const QVariant& obsolet, obsoleteFields) {
            ssoAccount->remove(obsolet.toString());
        }
    }

    if (!modifiedFields.isEmpty()) {
        // Batch update of the modified fields
        QVariantList::const_iterator field = modifiedFields.begin();
        QVariantList::const_iterator value = modifiedValues.begin();
        while (field != modifiedFields.end() && value != modifiedValues.end())
            ssoAccount->setValue(field++->toString(), *value++);
    }

    if (!addedFields.isEmpty()) {
        // Batch insert of the added fields
        QVariantList::const_iterator field = addedFields.begin();
        QVariantList::const_iterator value = addedValues.begin();
        while (field != addedFields.end() && value != addedValues.end())
            ssoAccount->setValue(field++->toString(), *value++);

    }

    ssoAccount->endGroup();
    if (!ssoAccount->syncAndBlock())
        return DatabaseFailure;

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::accountCustomFields(QSharedPointer<Accounts::Account>& ssoAccount, QMap<QString, QString>* fields)
{
    ssoAccount->beginGroup("customFields");
    foreach (const QString& key, ssoAccount->allKeys()) {
        fields->insert(key, ssoAccount->valueAsString(key));
    }
    ssoAccount->endGroup();

    return Success;
}
#endif

QMailStorePrivate::AttemptResult QMailStorePrivate::customFields(quint64 id, QMap<QString, QString> *fields, const QString &tableName)
{
    QString sql("SELECT name,value FROM %1 WHERE id=?");
    QSqlQuery query(simpleQuery(sql.arg(tableName),
                                QVariantList() << id,
                                QString("%1 custom field query").arg(tableName)));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        fields->insert(query.value(0).toString(), query.value(1).toString());

    return Success;
}

#ifdef USE_ACCOUNTS_QT
QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAddAccount(QMailAccount *account, QMailAccountConfiguration* config,
                                                                      QMailAccountIdList *addedAccountIds,
                                                                      Transaction &t, bool commitOnSuccess)
{
    if (account->id().isValid() && idExists(account->id())) {
        qWarning() << "Account already exists in database, use update instead";
        return Failure;
    }

    // Create new account in Accounts subsystem
    QSharedPointer<Accounts::Account> ssoAccount(manager->createAccount("email"));
    if (!ssoAccount) {
        SSOHandleError(manager->lastError());
        qMailLog(Messaging) << "Failed to create account";
        return DatabaseFailure;
    }

    ssoAccount->setDisplayName(account->name());
    ssoAccount->setEnabled(account->status() & QMailAccount::Enabled);

    Accounts::ServiceList services = ssoAccount->services("e-mail");
    if (!services.count()) {
        qMailLog(Messaging) << "E-mail Services not found, make sure that *.service and *.provider files are properly installed.";
        return DatabaseFailure;
    }
    Q_ASSERT (services.count() == 1);
    Accounts::Service service = services.first();
    Q_ASSERT(service.serviceType() == "e-mail");

    ssoAccount->selectService(service);
    ssoAccount->setEnabled(true); // service is enabled anyway
    ssoAccount->setValue("type", static_cast<int>(account->messageType()));
    ssoAccount->setValue("status", account->status());
    const bool appendSignature = (account->status() & QMailAccount::AppendSignature);
    ssoAccount->setValue("signatureEnabled", appendSignature);
    const bool hasPersistentConnection = (account->status() & QMailAccount::HasPersistentConnection);
    ssoAccount->setValue("hasPersistentConnection", hasPersistentConnection);
    ssoAccount->setValue("signature", account->signature());
    ssoAccount->setValue("emailaddress", account->fromAddress().address());
    ssoAccount->setValue("fullName", account->fromAddress().name());
    //Account was never synced
    ssoAccount->setValue("lastSynchronized", quint64(0));
    ssoAccount->setValue("iconPath", account->iconPath());
    const bool canTransmit = (account->status() & QMailAccount::CanTransmit);
    ssoAccount->setValue("canTransmit", canTransmit);

    if (!ssoAccount->syncAndBlock())
        return DatabaseFailure;

    //Extract the insert id
    QMailAccountId insertId = QMailAccountId(ssoAccount->id());

    {
        // Insert any standard folders configured for this account
        const QMap<QMailFolder::StandardFolder, QMailFolderId> &folders(account->standardFolders());
        if (!folders.isEmpty()) {
            QVariantList types;
            QVariantList folderIds;

            QMap<QMailFolder::StandardFolder, QMailFolderId>::const_iterator it = folders.begin(), end = folders.end();
            for ( ; it != end; ++it) {
                types.append(static_cast<int>(it.key()));
                folderIds.append(it.value().toULongLong());
            }

            // Batch insert the folders
            QString sql("INSERT into mailaccountfolders (id,foldertype,folderid) VALUES (%1,?,?)");
            QSqlQuery query(batchQuery(sql.arg(QString::number(insertId.toULongLong())),
                                       QVariantList() << QVariant(types)
                                       << QVariant(folderIds),
                                       "addAccount mailaccountfolders query"));
            if (query.lastError().type() != QSqlError::NoError) {
                ssoAccount->remove();
                ssoAccount->syncAndBlock();
                return DatabaseFailure;
            }
        }

        // Insert any custom fields belonging to this account
        AttemptResult result = addAccountCustomFields(ssoAccount, account->customFields());
        if (result != Success) {
            ssoAccount->remove();
            ssoAccount->syncAndBlock();
            return result;
        }
    }

    if (config) {
        foreach (const QString &service, config->services()) {
            QMailAccountConfiguration::ServiceConfiguration &serviceConfig(config->serviceConfiguration(service));
            const QMap<QString, QString> &fields = serviceConfig.values();
            QString serviceName = serviceConfig.service();

            // Open configuration group
            ssoAccount->beginGroup(serviceName);

            // Insert any configuration fields belonging to this account
            QMap<QString, QString>::const_iterator it = fields.begin(), end = fields.end();
            for ( ; it != end; ++it) {
                ssoAccount->setValue(it.key(), QVariant(it.value()));
            }
            // Close group of keys
            ssoAccount->endGroup();
        }

        // Save all changes
        if (!ssoAccount->syncAndBlock()) {
            ssoAccount->remove();
            ssoAccount->syncAndBlock();
            return DatabaseFailure;
        }

        config->setId(insertId);
    }

    account->setId(insertId);

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit account changes to database";

        account->setId(QMailAccountId()); //revert the id
        ssoAccount->remove();
        ssoAccount->syncAndBlock();
        return DatabaseFailure;
    }

    addedAccountIds->append(insertId);
    return Success;
}
#else
QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAddAccount(QMailAccount *account, QMailAccountConfiguration* config, 
                                                                      QMailAccountIdList *addedAccountIds, 
                                                                      Transaction &t, bool commitOnSuccess)
{
    if (account->id().isValid() && idExists(account->id())) {
        qWarning() << "Account already exists in database, use update instead";
        return Failure;
    }

    QMailAccountId insertId;

    {
        QString properties("type,name,emailaddress,status,signature,lastsynchronized,iconpath");
        QString values("?,?,?,?,?,?,?");
        QVariantList propertyValues;
        propertyValues << static_cast<int>(account->messageType()) 
                       << account->name() 
                       << account->fromAddress().toString(true)
                       << account->status()
                       << account->signature()
                       << QMailTimeStamp(account->lastSynchronized()).toLocalTime()
                       << account->iconPath();
        {
            QSqlQuery query(simpleQuery(QString("INSERT INTO mailaccounts (%1) VALUES (%2)").arg(properties).arg(values),
                                        propertyValues,
                                        "addAccount mailaccounts query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            //Extract the insert id
            insertId = QMailAccountId(extractValue<quint64>(query.lastInsertId()));
        }

        // Insert any standard folders configured for this account
        const QMap<QMailFolder::StandardFolder, QMailFolderId> &folders(account->standardFolders());
        if (!folders.isEmpty()) {
            QVariantList types;
            QVariantList folderIds;

            QMap<QMailFolder::StandardFolder, QMailFolderId>::const_iterator it = folders.begin(), end = folders.end();
            for ( ; it != end; ++it) {
                types.append(static_cast<int>(it.key()));
                folderIds.append(it.value().toULongLong());
            }

            // Batch insert the folders
            QString sql("INSERT into mailaccountfolders (id,foldertype,folderid) VALUES (%1,?,?)");
            QSqlQuery query(batchQuery(sql.arg(QString::number(insertId.toULongLong())),
                                       QVariantList() << QVariant(types)
                                       << QVariant(folderIds),
                                       "addAccount mailaccountfolders query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        // Insert any custom fields belonging to this account
        AttemptResult result = addCustomFields(insertId.toULongLong(), account->customFields(), "mailaccountcustom");
        if (result != Success)
            return result;
    }

    if (config) {
        foreach (const QString &service, config->services()) {
            QMailAccountConfiguration::ServiceConfiguration &serviceConfig(config->serviceConfiguration(service));
            const QMap<QString, QString> &fields = serviceConfig.values();

            QVariantList configFields;
            QVariantList configValues;

            // Insert any configuration fields belonging to this account
            QMap<QString, QString>::const_iterator it = fields.begin(), end = fields.end();
            for ( ; it != end; ++it) {
                configFields.append(QVariant(it.key()));
                configValues.append(QVariant(it.value()));
            }

            // Batch insert the custom fields
            QString sql("INSERT INTO mailaccountconfig (id,service,name,value) VALUES (%1,'%2',?,?)");
            QSqlQuery query(batchQuery(sql.arg(QString::number(insertId.toULongLong())).arg(service),
                                       QVariantList() << QVariant(configFields)
                                                      << QVariant(configValues),
                                       "addAccount mailaccountconfig query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        config->setId(insertId);
    }

    account->setId(insertId);

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit account changes to database";

        account->setId(QMailAccountId()); //revert the id
        return DatabaseFailure;
    }

    addedAccountIds->append(insertId);
    return Success;
}
#endif

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAddFolder(QMailFolder *folder, 
                                                                     QMailFolderIdList *addedFolderIds, QMailAccountIdList *modifiedAccountIds,
                                                                     Transaction &t, bool commitOnSuccess)
{   
    //check that the parent folder actually exists
    if (!checkPreconditions(*folder))
        return Failure;

    QMailFolderId insertId;

    {
        {
            QSqlQuery query(simpleQuery("INSERT INTO mailfolders (name,parentid,parentaccountid,displayname,status,servercount,serverunreadcount,serverundiscoveredcount) VALUES (?,?,?,?,?,?,?,?)",
                                        QVariantList() << folder->path()
                                                       << folder->parentFolderId().toULongLong()
                                                       << folder->parentAccountId().toULongLong()
                                                       << folder->displayName()
                                                       << folder->status()
                                                       << folder->serverCount()
                                                       << folder->serverUnreadCount()
                                                       << folder->serverUndiscoveredCount(),
                                        "addFolder mailfolders query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            // Extract the inserted id
            insertId = QMailFolderId(extractValue<quint64>(query.lastInsertId()));
        }

        // Insert any custom fields belonging to this folder
        AttemptResult result = addCustomFields(insertId.toULongLong(), folder->customFields(), "mailfoldercustom");
        if (result != Success)
            return result;
    }

    folder->setId(insertId);

    //create links to ancestor folders
    if (folder->parentFolderId().isValid()) {
        {
            //add records for each ancestor folder
            QSqlQuery query(simpleQuery("INSERT INTO mailfolderlinks "
                                        "SELECT DISTINCT id,? FROM mailfolderlinks WHERE descendantid=?",
                                        QVariantList() << folder->id().toULongLong() 
                                                       << folder->parentFolderId().toULongLong(),
                                        "mailfolderlinks insert ancestors"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        {
            // Our direct parent is also an ancestor
            QSqlQuery query(simpleQuery("INSERT INTO mailfolderlinks VALUES (?,?)",
                                        QVariantList() << folder->parentFolderId().toULongLong() 
                                                       << folder->id().toULongLong(),
                                        "mailfolderlinks insert parent"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit folder changes to database";

        folder->setId(QMailFolderId()); //revert the id
        return DatabaseFailure;
    }
   
    addedFolderIds->append(insertId);
    if (folder->parentAccountId().isValid())
        modifiedAccountIds->append(folder->parentAccountId());
    return Success;
}

struct ReferenceStorer
{
    QMailMessage *message;

    ReferenceStorer(QMailMessage *m) : message(m) {}

    bool operator()(const QMailMessagePart &part)
    {
        QString value;

        if (part.referenceType() == QMailMessagePart::MessageReference) {
            value = "message:" + QString::number(part.messageReference().toULongLong());
        } else if (part.referenceType() == QMailMessagePart::PartReference) {
            value = "part:" + part.partReference().toString(true);
        }

        if (!value.isEmpty()) {
            QString loc(part.location().toString(false));

            // Store the reference location into the message
            QString key("qmf-reference-location-" + loc);
            if (message->customField(key) != value) {
                message->setCustomField(key, value);
            }

            // Store the reference resolution into the message
            key = "qmf-reference-resolution-" + loc;
            value = part.referenceResolution();
            if (message->customField(key) != value) {
                message->setCustomField(key, value);
            }
        }

        return true;
    }
};

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAddThread(QMailThread *thread, QMailThreadIdList *addedThreadIds, Transaction &t, bool commitOnSuccess)
{
    // TODO: check preconditions
    QString senders = QMailAddress::toStringList(thread->senders()).join(",");

    QSqlQuery query(simpleQuery("INSERT INTO mailthreads (id,messagecount,unreadcount,serveruid,parentaccountid,subject,senders,lastdate,starteddate,status)"
                                " VALUES (?,?,?,?,?,?,?,?,?,?,?)",
                                QVariantList() << thread->id()
                                            << thread->messageCount()
                                            << thread->unreadCount()
                                            << thread->serverUid()
                                            << thread->parentAccountId()
                                            << thread->subject()
                                            << thread->preview()
                                            << senders
                                            << thread->lastDate().toUTC()
                                            << thread->startedDate().toUTC()
                                            << thread->status(),
                                "addFolder mailfolders query"));

    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    // Extract the inserted id
    QMailThreadId insertId(extractValue<quint64>(query.lastInsertId()));


    thread->setId(insertId);


    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit thread  changes to database";

        thread->setId(QMailThreadId()); // id didn't sync
        return DatabaseFailure;
    }

    addedThreadIds->append(insertId);

    return Success;
}


QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAddMessage(QMailMessage *message, const QString &identifier, const QStringList &references, AttemptAddMessageOut *out,
                                                                      Transaction &t, bool commitOnSuccess)
{
    if (!message->parentAccountId().isValid()) {
        // Require a parent account - possibly relax this later
        qWarning() << "Unable to add message without parent account";
        return Failure;
    }

    if (message->contentScheme().isEmpty()) {
        // Use the default storage scheme
        message->setContentScheme(defaultContentScheme());
    }

    MutexGuard lock(contentManagerMutex());
    lock.lock();

    ReferenceStorer refStorer(message);
    const_cast<const QMailMessage*>(message)->foreachPart<ReferenceStorer&>(refStorer);



    QList<QMailContentManager*> contentManagers;

    foreach(QString scheme, QStringList()
                                << QMailContentManagerFactory::defaultFilterScheme()
                                << message->contentScheme()
                                << QMailContentManagerFactory::defaultIndexerScheme())
     {
        if (!scheme.isEmpty()) {
            QMailContentManager *manager(QMailContentManagerFactory::create(scheme));
            if (!manager) {
                qWarning() << "Unable to create content manager for scheme:" << message->contentScheme();
                return Failure;
             } else {
                 contentManagers.append(manager);
            }
        }

    }

    foreach(QMailContentManager *manager, contentManagers) {
        QMailStore::ErrorCode code = manager->add(message, durability(commitOnSuccess));
        if (code != QMailStore::NoError) {
            setLastError(code);
            qWarning() << "Unable to add message content to URI:" << ::contentUri(*message);
            return Failure;
        }
    }

    AttemptResult result = attemptAddMessage(static_cast<QMailMessageMetaData*>(message), identifier, references, out, t, commitOnSuccess);
    if (result != Success) {
        bool obsoleted(false);
        foreach(QMailContentManager *manager, contentManagers) {
            // Try to remove the content file we added
            QMailStore::ErrorCode code = manager->remove(message->contentIdentifier());
            if (code != QMailStore::NoError && !obsoleted) {
                qWarning() << "Could not remove extraneous message content:" << ::contentUri(*message);
                if (code == QMailStore::ContentNotRemoved) {
                    obsoleted = true;
                    // The existing content could not be removed - try again later
                    if (!obsoleteContent(message->contentIdentifier())) {
                        setLastError(QMailStore::FrameworkFault);
                    }
                } else {
                    setLastError(code);
                }
            }
        }
     }

    return result;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAddMessage(QMailMessageMetaData *metaData, const QString &identifier, const QStringList &references, AttemptAddMessageOut *out,
                                                                      Transaction &t, bool commitOnSuccess)
{
    if (!metaData->parentFolderId().isValid()) {
        qWarning() << "Unable to add message. Invalid parent folder id";
        return Failure;
    }

    if (metaData->id().isValid() && idExists(metaData->id())) {
        qWarning() << "Message ID" << metaData->id() << "already exists in database, use update instead";
        return Failure;
    }

    if(!metaData->serverUid().isEmpty() && metaData->parentAccountId().isValid()
        && messageExists(metaData->serverUid(), metaData->parentAccountId()))
    {
        qWarning() << "Message with serveruid: " << metaData->serverUid() << "and accountid:" << metaData->parentAccountId()
                << "already exist. Use update instead.";
        return Failure;
    }

    bool replyOrForward(false);
    QString baseSubject(QMail::baseSubject(metaData->subject(), &replyOrForward));
    QStringList missingReferences;
    bool missingAncestor(false);

    // Attach this message to a thread
    if (!metaData->parentThreadId().isValid() && metaData->inResponseTo().isValid()) {
        QString sql("SELECT parentthreadid FROM mailmessages WHERE id=%1");
        QSqlQuery query(simpleQuery(sql.arg(metaData->inResponseTo().toULongLong()), "addMessage threadid select query"));

        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.next()) {
            quint64 threadId(extractValue<quint64>(query.value(0)));

            if (threadId == 0)
                qWarning() << "Message had an inResponseTo of " << metaData->inResponseTo() << " which had no thread id";
            metaData->setParentThreadId(QMailThreadId(threadId));
        } else {
            // Predecessor was deleted
            metaData->setInResponseTo(QMailMessageId());
        }
    }

    if (!metaData->inResponseTo().isValid()) {
        // Does this message have any references to resolve?
        AttemptResult result = messagePredecessor(metaData, references, baseSubject, replyOrForward, &missingReferences, &missingAncestor);
        if (result != Success)
            return result;
    }

    if (metaData->parentThreadId().isValid()) {
        //if it is a Trash or Draft message, then we shouldn't update any thread value
        QMailAccount acc(metaData->parentAccountId());
        QMailFolderId trashFolderId = acc.standardFolder(QMailFolder::TrashFolder);
        QMailFolderId draftFolderId = acc.standardFolder(QMailFolder::DraftsFolder);
        const bool& TrashOrDraft = ((metaData->status() & (QMailMessage::Trash | QMailMessage::Draft)) != 0) ||
                (trashFolderId != QMailFolder::LocalStorageFolderId && metaData->parentFolderId() == trashFolderId) ||
                (draftFolderId != QMailFolder::LocalStorageFolderId && metaData->parentFolderId() == draftFolderId);
        if (!TrashOrDraft) {
            APPEND_UNIQUE(out->modifiedThreadIds, metaData->parentThreadId());
            APPEND_UNIQUE(out->updatedThreadIds, metaData->parentThreadId());
            QMailThread thread(metaData->parentThreadId());
            QString senders;
            const QMailAddress &newSender = metaData->from();
            const bool& newStartedMessage = thread.startedDate() > metaData->date();
            const bool& newLastMessage = thread.lastDate() < metaData->date();

            if (!thread.senders().contains(newSender)) {
                senders = QMailAddress::toStringList(QMailAddressList() << newSender
                                                     << thread.senders()).join(",");
            }
            else {
                if (newLastMessage) {
                    QMailAddressList oldSendersList = thread.senders();
                    oldSendersList.removeAll(newSender);
                    oldSendersList.prepend(newSender);
                    senders = QMailAddress::toStringList(oldSendersList).join(",");
                } else {
                    senders = QMailAddress::toStringList(thread.senders()).join(",");
                }
            }

            QString sql("UPDATE mailthreads SET"
                        + QString(" messagecount = messagecount + 1,")
                        + QString(" senders = (?)")
                        + ((newLastMessage && !metaData->preview().isEmpty()) ? QString(", preview = (?)") : QString(""))
                        + (newLastMessage ? QString(", lastdate = (?)") : QString(""))
                        + (newStartedMessage ? QString(", starteddate = (?)") : QString(""))
                        + (metaData->status() & QMailMessage::Read ? QString("") : QString(", unreadcount = unreadcount + 1 "))
                        + QString(", status = (status | %1)").arg(metaData->status()) + " WHERE id= (?)");
            QVariantList bindValues;
            bindValues << QVariant(senders);
            if (newLastMessage && !metaData->preview().isEmpty())
                bindValues << QVariant(metaData->preview());
            if (newLastMessage)
                bindValues << QVariant(metaData->date().toUTC());
            if (newStartedMessage)
                bindValues << QVariant(metaData->date().toUTC());
            bindValues << QVariant(metaData->parentThreadId().toULongLong());
            QSqlQuery query = simpleQuery(sql, bindValues, "addMessage update thread");

            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
    } else {
        quint64 threadId = 0;

        // Add a new thread for this message

        QMap<QString, QVariant> values;
        values.insert("messagecount", (metaData->status() & QMailMessage::Trash ||
                                       metaData->status() & QMailMessage::Draft) ? 0 : 1);
        values.insert("unreadcount", (metaData->status() & QMailMessage::Read ||
                                      metaData->status() & QMailMessage::Trash || metaData->status() & QMailMessage::Draft) ? 0 : 1);
        values.insert("serveruid", "");
        values.insert("parentaccountid", metaData->parentAccountId().toULongLong());
        values.insert("subject", metaData->subject());
        values.insert("preview", metaData->preview());
        values.insert("senders", metaData->from().toString());
        values.insert("lastdate", metaData->date().toUTC());
        values.insert("starteddate", metaData->date().toUTC());
        values.insert("status", metaData->status());
        const QString &columns = QStringList(values.keys()).join(",");
        QSqlQuery query(simpleQuery(QString("INSERT INTO mailthreads (%1) VALUES %2").arg(columns).arg(expandValueList(values.count())),
                                    values.values(),
                                    "addMessage mailthreads insert query"));

        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        threadId = extractValue<quint64>(query.lastInsertId());

        Q_ASSERT(threadId != 0);
        metaData->setParentThreadId(QMailThreadId(threadId));
        APPEND_UNIQUE(out->addedThreadIds, metaData->parentThreadId());
    }

    // Ensure that any phone numbers are added in minimal form
    QMailAddress from(metaData->from());
    QString fromText(from.isPhoneNumber() ? from.minimalPhoneNumber() : from.toString());

    QStringList recipients;
    foreach (const QMailAddress& address, metaData->recipients())
        recipients.append(address.isPhoneNumber() ? address.minimalPhoneNumber() : address.toString());

    quint64 insertId;

    QMap<QString, QVariant> values;

    values.insert("type", static_cast<int>(metaData->messageType()));
    values.insert("parentfolderid", metaData->parentFolderId().toULongLong());
    values.insert("sender", fromText);
    values.insert("recipients", recipients.join(","));
    values.insert("subject", metaData->subject());
    values.insert("stamp", QMailTimeStamp(metaData->date()).toUTC());
    values.insert("status", static_cast<int>(metaData->status()));
    values.insert("parentaccountid", metaData->parentAccountId().toULongLong());
    values.insert("mailfile", ::contentUri(*metaData));
    values.insert("serveruid", metaData->serverUid());
    values.insert("size", metaData->size());
    values.insert("contenttype", static_cast<int>(metaData->content()));
    values.insert("responseid", metaData->inResponseTo().toULongLong());
    values.insert("responsetype", metaData->responseType());
    values.insert("receivedstamp", QMailTimeStamp(metaData->receivedDate()).toUTC());
    values.insert("previousparentfolderid", metaData->previousParentFolderId().toULongLong());
    values.insert("copyserveruid", metaData->copyServerUid());
    values.insert("restorefolderid", metaData->restoreFolderId().toULongLong());
    values.insert("listid", metaData->listId());
    values.insert("rfcID", metaData->rfcId());
    values.insert("preview", metaData->preview());
    values.insert("parentthreadid", metaData->parentThreadId().toULongLong());

    Q_ASSERT(metaData->parentThreadId().toULongLong() != 0);

    const QStringList &list(values.keys());
    QString columns = list.join(",");

    // Add the record to the mailmessages table
    QSqlQuery query(simpleQuery(QString("INSERT INTO mailmessages (%1) VALUES %2").arg(columns).arg(expandValueList(values.count())),
                                values.values(),
                                "addMessage mailmessages query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    //retrieve the insert id
    insertId = extractValue<quint64>(query.lastInsertId());

    metaData->setId(QMailMessageId(insertId));

    if (!baseSubject.isEmpty()) {
        // Ensure that this subject is in the subjects table
        AttemptResult result = registerSubject(baseSubject, insertId, metaData->inResponseTo(), missingAncestor);
        if (result != Success)
            return result;
    }

    // Insert any custom fields belonging to this message
    AttemptResult result(addCustomFields(insertId, metaData->customFields(), "mailmessagecustom"));
    if (result != Success)
        return result;

    // Does this message have any identifier?
    if (!identifier.isEmpty()) {
        QSqlQuery query(simpleQuery("INSERT INTO mailmessageidentifiers (id,identifier) VALUES (?,?)",
                                    QVariantList() << insertId << identifier,
                                    "addMessage mailmessageidentifiers query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    // See if this message resolves any missing message items
    QMailMessageIdList updatedMessageIds;
    result = resolveMissingMessages(identifier, metaData->inResponseTo(), baseSubject, *metaData, &updatedMessageIds);
    APPEND_UNIQUE(out->updatedMessageIds, &updatedMessageIds);
    if (result != Success)
        return result;

    if (!updatedMessageIds.isEmpty()) {
        // Find the set of folders and accounts whose contents are modified by these messages
        result = affectedByMessageIds(updatedMessageIds, out->modifiedFolderIds, out->modifiedAccountIds);
        if (result != Success)
            return result;
    }

    if (!missingReferences.isEmpty()) {
        // Add the missing references to the missing messages table
        QVariantList refs;
        QVariantList levels;

        int level = missingReferences.count();
        foreach (const QString &ref, missingReferences) {
            refs.append(QVariant(ref));
            levels.append(QVariant(--level));
        }

        QString sql("INSERT INTO missingmessages (id,identifier,level) VALUES (%1,?,?)");
        QSqlQuery query(batchQuery(sql.arg(QString::number(insertId)),
                                   QVariantList() << QVariant(refs) << QVariant(levels),
                                   "addMessage missingmessages insert query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    // Find the complete set of modified folders, including ancestor folders
    QMailFolderIdList folderIds;
    folderIds.append(metaData->parentFolderId());
    folderIds += folderAncestorIds(folderIds, true, &result);
    if (result != Success)
        return result;

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit message changes to database";
        return DatabaseFailure;
    }

    metaData->setId(QMailMessageId(insertId));
    metaData->setUnmodified();
    APPEND_UNIQUE(out->addedMessageIds, metaData->id());
    APPEND_UNIQUE(out->modifiedFolderIds, &folderIds);
    if (metaData->parentAccountId().isValid())
        APPEND_UNIQUE(out->modifiedAccountIds, metaData->parentAccountId());
    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptRemoveAccounts(const QMailAccountKey &key, 
                                                                          AttemptRemoveAccountOut *out,
                                                                          Transaction &t, bool commitOnSuccess)
{
    QStringList expiredContent;

    if (deleteAccounts(key, *out->deletedAccountIds, *out->deletedFolderIds, *out->deletedThreadIds, *out->deletedMessageIds, expiredContent, *out->updatedMessageIds, *out->modifiedFolderIds, *out->modifiedThreadIds, *out->modifiedAccountIds)) {
        if (commitOnSuccess && t.commit()) {
            //remove deleted objects from caches
            removeExpiredData(*out->deletedMessageIds, *out->deletedThreadIds, expiredContent, *out->deletedFolderIds, *out->deletedAccountIds);
            return Success;
        }
    }

    return DatabaseFailure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptRemoveFolders(const QMailFolderKey &key, QMailStore::MessageRemovalOption option, 
                                                                         AttemptRemoveFoldersOut *out,
                                                                         Transaction &t, bool commitOnSuccess)
{
    QStringList expiredContent;

    if (deleteFolders(key, option, *out->deletedFolderIds, *out->deletedMessageIds, *out->deletedThreadIds, expiredContent, *out->updatedMessageIds, *out->modifiedFolderIds, *out->modifiedThreadIds, *out->modifiedAccountIds)) {
        if (commitOnSuccess && t.commit()) {
            //remove deleted objects from caches
            removeExpiredData(*out->deletedMessageIds, *out->deletedThreadIds, expiredContent, *out->deletedFolderIds);
            return Success;
        }
    }

    return DatabaseFailure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptRemoveThreads(const QMailThreadKey &key, QMailStore::MessageRemovalOption option,
                                                                         AttemptRemoveThreadsOut *out,
                                                                         Transaction &t, bool commitOnSuccess)
{
    QStringList expiredContent;

    if (deleteThreads(key, option, *out->deletedThreadIds, *out->deletedMessageIds, expiredContent, *out->updatedMessageIds, *out->modifiedFolderIds, *out->modifiedThreadIds, *out->modifiedAccountIds)) {
        if (commitOnSuccess && t.commit()) {
            //remove deleted objects from caches
            removeExpiredData(*out->deletedMessageIds, *out->deletedThreadIds, expiredContent);
            return Success;
        }
    }

    return DatabaseFailure;
}


QMailStorePrivate::AttemptResult QMailStorePrivate::attemptRemoveMessages(const QMailMessageKey &key, QMailStore::MessageRemovalOption option,
                                                                          QMailMessageIdList *deletedMessageIds, QMailThreadIdList* deletedThreadIds, QMailMessageIdList *updatedMessageIds, QMailFolderIdList *modifiedFolderIds, QMailThreadIdList *modifiedThreadIds, QMailAccountIdList *modifiedAccountIds,
                                                                          Transaction &t, bool commitOnSuccess)
{
    QStringList expiredContent;

    if (deleteMessages(key, option, *deletedMessageIds, *deletedThreadIds, expiredContent, *updatedMessageIds, *modifiedFolderIds, *modifiedThreadIds, *modifiedAccountIds)) {
        if (commitOnSuccess && t.commit()) {
            //remove deleted objects from caches
            removeExpiredData(*deletedMessageIds, *deletedThreadIds, expiredContent);
            return Success;
        }
    }

    return DatabaseFailure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptUpdateAccount(QMailAccount *account, QMailAccountConfiguration *config, 
                                                                         QMailAccountIdList *updatedAccountIds,
                                                                         Transaction &t, bool commitOnSuccess)
{
    QMailAccountId id(account ? account->id() : config ? config->id() : QMailAccountId());
    if (!id.isValid())
        return Failure;

#ifdef USE_ACCOUNTS_QT
    QSharedPointer<Accounts::Account> ssoAccount = getEmailAccount(id.toULongLong());
    if (!ssoAccount)
      return Failure;

    Accounts::ServiceList services = ssoAccount->enabledServices();
    Q_ASSERT (services.count() == 1);
    Accounts::Service service = services.first();

    Q_ASSERT(service.isValid());
    Q_ASSERT(service.serviceType() == "e-mail");
#endif

    if (account) {
#ifdef USE_ACCOUNTS_QT
        ssoAccount->selectService(service);
        bool isEmailBoxName = false;
        if(!ssoAccount->valueAsString("email/email_box_name").isEmpty()) {
            isEmailBoxName = true;
            ssoAccount->setValue("email/email_box_name",account->name());
        }
        ssoAccount->selectService();
        if (isEmailBoxName) {
            ssoAccount->setDisplayName(ssoAccount->valueAsString("username"));
        }
        else {
            ssoAccount->setDisplayName(account->name());
        }
        ssoAccount->setEnabled(account->status() & QMailAccount::Enabled);
        ssoAccount->selectService(service);
        ssoAccount->setValue("type", static_cast<int>(account->messageType()));
        ssoAccount->setValue("status", account->status());
        bool signatureEnabled = account->status() & QMailAccount::AppendSignature;
        ssoAccount->setValue("signatureEnabled", signatureEnabled);
        bool hasPersistentConnection = account->status() & QMailAccount::HasPersistentConnection;
        ssoAccount->setValue("hasPersistentConnection", hasPersistentConnection);
        ssoAccount->setValue("signature", account->signature());
        ssoAccount->setValue("emailaddress", account->fromAddress().address());
        ssoAccount->setValue("fullName", account->fromAddress().name());
        if (account->lastSynchronized().isValid()) {
            ssoAccount->setValue("lastSynchronized", static_cast<quint64>(account->lastSynchronized().toLocalTime().toTime_t()));
        }
        else {
            ssoAccount->setValue("lastSynchronized", quint64(0));
        }
        bool isDefault = account->status() & QMailAccount::PreferredSender;
        bool canTransmit = account->status() & QMailAccount::CanTransmit;
        ssoAccount->setValue("email/default", isDefault);
        ssoAccount->setValue("canTransmit", canTransmit);
        ssoAccount->setValue("iconPath", account->iconPath());
#else
        QString properties("type=?, name=?, emailaddress=?, status=?, signature=?, lastsynchronized=?, iconpath=?");
        QVariantList propertyValues;
        propertyValues << static_cast<int>(account->messageType()) 
                       << account->name()
                       << account->fromAddress().toString(true)
                       << account->status()
                       << account->signature()                       
                       << QMailTimeStamp(account->lastSynchronized()).toLocalTime()
                       << account->iconPath();

        {
            QSqlQuery query(simpleQuery(QString("UPDATE mailaccounts SET %1 WHERE id=?").arg(properties),
                                        propertyValues << id.toULongLong(),
                                        "updateAccount mailaccounts query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
#endif
        // Update any standard folders configured
        const QMap<QMailFolder::StandardFolder, QMailFolderId> &folders(account->standardFolders());
        QMap<QMailFolder::StandardFolder, QMailFolderId> existingFolders;

        {
            // Find the existing folders
            QSqlQuery query(simpleQuery("SELECT foldertype,folderid FROM mailaccountfolders WHERE id=?", 
                                        QVariantList() << id.toULongLong(),
                                        "updateAccount mailaccountfolders select query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                existingFolders.insert(QMailFolder::StandardFolder(query.value(0).toInt()), QMailFolderId(query.value(1).toULongLong()));
        }

        QVariantList obsoleteTypes;
        QVariantList modifiedTypes;
        QVariantList modifiedFolderIds;
        QVariantList addedTypes;
        QVariantList addedFolders;

        // Compare the sets
        QMap<QMailFolder::StandardFolder, QMailFolderId>::const_iterator fend = folders.end(), eend = existingFolders.end();
        QMap<QMailFolder::StandardFolder, QMailFolderId>::const_iterator it = existingFolders.begin();
        for ( ; it != eend; ++it) {
            QMap<QMailFolder::StandardFolder, QMailFolderId>::const_iterator current = folders.find(it.key());
            if (current == fend) {
                obsoleteTypes.append(QVariant(static_cast<int>(it.key())));
            } else if (*current != *it) {
                modifiedTypes.append(QVariant(static_cast<int>(current.key())));
                modifiedFolderIds.append(QVariant(current.value().toULongLong()));
            }
        }

        for (it = folders.begin(); it != fend; ++it) {
            if (existingFolders.find(it.key()) == eend) {
                addedTypes.append(QVariant(static_cast<int>(it.key())));
                addedFolders.append(QVariant(it.value().toULongLong()));
            }
        }

        if (!obsoleteTypes.isEmpty()) {
            // Remove the obsolete folders
            QString sql("DELETE FROM mailaccountfolders WHERE id=? AND foldertype IN %2");
            QSqlQuery query(simpleQuery(sql.arg(expandValueList(obsoleteTypes)),
                                        QVariantList() << id.toULongLong() << obsoleteTypes,
                                        "updateAccount mailaccountfolders delete query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        if (!modifiedTypes.isEmpty()) {
            // Batch update the modified folders
            QString sql("UPDATE mailaccountfolders SET folderid=? WHERE id=%2 AND foldertype=?");
            QSqlQuery query(batchQuery(sql.arg(QString::number(id.toULongLong())),
                                       QVariantList() << QVariant(modifiedFolderIds)
                                                      << QVariant(modifiedTypes),
                                       "updateAccount mailaccountfolders update query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        if (!addedTypes.isEmpty()) {
            // Batch insert the added folders
            QString sql("INSERT INTO mailaccountfolders (id,foldertype,folderid) VALUES (%1,?,?)");
            QSqlQuery query(batchQuery(sql.arg(QString::number(id.toULongLong())),
                                          QVariantList() << QVariant(addedTypes) << QVariant(addedFolders),
                                          "updateAccount mailaccountfolders insert query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        if (account->customFieldsModified()) {
#ifdef USE_ACCOUNTS_QT
            AttemptResult result = updateAccountCustomFields(ssoAccount, account->customFields());
#else
            AttemptResult result = updateCustomFields(id.toULongLong(), account->customFields(), "mailaccountcustom");
#endif
            if (result != Success)
                return result;
        }
    }

    if (config) {
#ifdef USE_ACCOUNTS_QT
        ssoAccount->selectService(service);
#endif
        // Find the complete set of configuration fields
        QMap<QPair<QString, QString>, QString> fields;

        foreach (const QString &service, config->services()) {
            QMailAccountConfiguration::ServiceConfiguration &serviceConfig(config->serviceConfiguration(service));
            const QMap<QString, QString> &values = serviceConfig.values();

            // Insert any configuration fields belonging to this account
            QMap<QString, QString>::const_iterator it = values.begin(), end = values.end();
            for ( ; it != end; ++it)
                fields.insert(qMakePair(service, it.key()), it.value());
        }

        // Find the existing fields in the database
        QMap<QPair<QString, QString>, QString> existing;

        {
#ifdef USE_ACCOUNTS_QT
            foreach (const QString& group, ssoAccount->childGroups()) {
                if (group != "customFields") {
                    ssoAccount->beginGroup(group);
                    foreach (const QString& name, ssoAccount->allKeys()) {
                         existing.insert(qMakePair(group, name), ssoAccount->valueAsString(name));
                    }
                    ssoAccount->endGroup();
                }
            }
#else
            QSqlQuery query(simpleQuery("SELECT service,name,value FROM mailaccountconfig WHERE id=?",
                                        QVariantList() << id.toULongLong(),
                                        "updateAccount mailaccountconfig select query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                existing.insert(qMakePair(query.value(0).toString(), query.value(1).toString()), query.value(2).toString());
#endif
        }

        QMap<QString, QVariantList> obsoleteFields;
        QMap<QString, QVariantList> modifiedFields;
        QMap<QString, QVariantList> modifiedValues;
        QMap<QString, QVariantList> addedFields;
        QMap<QString, QVariantList> addedValues;

        // Compare the sets
        QMap<QPair<QString, QString>, QString>::const_iterator fend = fields.end(), eend = existing.end();
        QMap<QPair<QString, QString>, QString>::const_iterator it = existing.begin();
        for ( ; it != eend; ++it) {
            const QPair<QString, QString> &name = it.key();
            QMap<QPair<QString, QString>, QString>::const_iterator current = fields.find(name);
            if (current == fend) {
                obsoleteFields[name.first].append(QVariant(name.second));
            } else if (*current != *it) {
                modifiedFields[name.first].append(QVariant(name.second));
                modifiedValues[name.first].append(QVariant(current.value()));
            }
        }

        for (it = fields.begin(); it != fend; ++it) {
            const QPair<QString, QString> &name = it.key();
            if (existing.find(name) == eend) {
                addedFields[name.first].append(QVariant(name.second));
                addedValues[name.first].append(QVariant(it.value()));
            }
        }

        if (!obsoleteFields.isEmpty()) {
            // Remove the obsolete fields
            QMap<QString, QVariantList>::const_iterator it = obsoleteFields.begin(), end = obsoleteFields.end();
            for ( ; it != end; ++it) {
                const QString &service = it.key();
                const QVariantList &fields = it.value();
#ifdef USE_ACCOUNTS_QT
                ssoAccount->beginGroup(service);
                foreach (const QVariant& field, fields) {
                    ssoAccount->remove(field.toString());
                }
                ssoAccount->endGroup();
#else
                QString sql("DELETE FROM mailaccountconfig WHERE id=? AND service='%1' AND name IN %2");
                QSqlQuery query(simpleQuery(sql.arg(service).arg(expandValueList(fields)),
                                            QVariantList() << id.toULongLong() << fields,
                                            "updateAccount mailaccountconfig delete query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
#endif
            }
        }

        if (!modifiedFields.isEmpty()) {
            // Batch update the modified fields
            QMap<QString, QVariantList>::const_iterator it = modifiedFields.begin(), end = modifiedFields.end();
            for (QMap<QString, QVariantList>::const_iterator vit = modifiedValues.begin(); it != end; ++it, ++vit) {
                const QString &service = it.key();
                const QVariantList &fields = it.value();
                const QVariantList &values = vit.value();
#ifdef USE_ACCOUNTS_QT
                QVariantList::const_iterator field = fields.begin();
                QVariantList::const_iterator value = values.begin();

                ssoAccount->beginGroup(service);
                while (field != fields.end() && value != values.end())
                    ssoAccount->setValue(field++->toString(), *value++);

                ssoAccount->endGroup();
#else
                QString sql("UPDATE mailaccountconfig SET value=? WHERE id=%1 AND service='%2' AND name=?");
                QSqlQuery query(batchQuery(sql.arg(QString::number(id.toULongLong())).arg(service),
                                           QVariantList() << QVariant(values) << QVariant(fields),
                                           "updateAccount mailaccountconfig update query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
#endif
            }
        }

        if (!addedFields.isEmpty()) {
            // Batch insert the added fields
            QMap<QString, QVariantList>::const_iterator it = addedFields.begin(), end = addedFields.end();
            for (QMap<QString, QVariantList>::const_iterator vit = addedValues.begin(); it != end; ++it, ++vit) {
                const QString &service = it.key();
                const QVariantList &fields = it.value();
                const QVariantList &values = vit.value();
#ifdef USE_ACCOUNTS_QT
                QVariantList::const_iterator field = fields.begin();
                QVariantList::const_iterator value = values.begin();

                ssoAccount->beginGroup(service);
                while (field != fields.end() && value != values.end())
                    ssoAccount->setValue(field++->toString(), *value++);

                ssoAccount->endGroup();
#else
                QString sql("INSERT INTO mailaccountconfig (id,service,name,value) VALUES (%1,'%2',?,?)");
                QSqlQuery query(batchQuery(sql.arg(QString::number(id.toULongLong())).arg(service),
                                           QVariantList() << QVariant(fields) << QVariant(values),
                                           "updateAccount mailaccountconfig insert query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
#endif
            }
        }
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit account update to database";
        return DatabaseFailure;
    }

#ifdef USE_ACCOUNTS_QT
    if (!ssoAccount->syncAndBlock())
        return DatabaseFailure;
#endif

    if (account) {
        // Update the account cache
        if (accountCache.contains(id))
            accountCache.insert(*account);
    }

    updatedAccountIds->append(id);
    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptUpdateThread(QMailThread *thread,
                                  QMailThreadIdList *updatedThreadIds,
                                  Transaction &t, bool commitOnSuccess)
{
    if (thread->id().isValid())
        return Failure;

    updatedThreadIds->append(thread->id());

    QString senders = QMailAddress::toStringList(thread->senders()).join(",");

    QSqlQuery query(simpleQuery("UPDATE mailthreads SET messagecount=?, unreadcount=?, serveruid=?, parentaccountid=?, subject=?, preview=?, senders=?, lastDate=?, startedDate=?, status=?"
                                " WHERE id=?",
                                QVariantList() << thread->messageCount()
                                             << thread->unreadCount()
                                             << thread->serverUid()
                                             << thread->parentAccountId().toULongLong()
                                             << thread->subject()
                                             << thread->preview()
                                             << senders
                                             << thread->lastDate().toUTC()
                                             << thread->startedDate().toUTC()
                                             << thread->status()
                                             << thread->id().toULongLong(),
                                "AttemptUpdateThread update"));

    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit folder update to database";
        return DatabaseFailure;
    }


    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptUpdateFolder(QMailFolder *folder, 
                                                                        QMailFolderIdList *updatedFolderIds, QMailAccountIdList *modifiedAccountIds,
                                                                        Transaction &t, bool commitOnSuccess)
{
    //check that the parent folder actually exists
    if(!checkPreconditions(*folder, true))
        return Failure;

    QMailFolderId parentFolderId;
    QMailAccountId parentAccountId;

    {
        //find the current parent folder
        QSqlQuery query(simpleQuery("SELECT parentid, parentaccountid FROM mailfolders WHERE id=?",
                                    QVariantList() << folder->id().toULongLong(),
                                    "mailfolder parent query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.first()) {
            parentFolderId = QMailFolderId(extractValue<quint64>(query.value(0)));
            parentAccountId = QMailAccountId(extractValue<quint64>(query.value(1)));
        }
    }

    {
        QSqlQuery query(simpleQuery("UPDATE mailfolders SET name=?,parentid=?,parentaccountid=?,displayname=?,status=?,servercount=?,serverunreadcount=?,serverundiscoveredcount=? WHERE id=?",
                                    QVariantList() << folder->path()
                                                   << folder->parentFolderId().toULongLong()
                                                   << folder->parentAccountId().toULongLong()
                                                   << folder->displayName()
                                                   << folder->status()
                                                   << folder->serverCount()
                                                   << folder->serverUnreadCount()
                                                   << folder->serverUndiscoveredCount()
                                                   << folder->id().toULongLong(),
                                    "updateFolder mailfolders query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }
    
    if (folder->customFieldsModified()) {
        AttemptResult result = updateCustomFields(folder->id().toULongLong(), folder->customFields(), "mailfoldercustom");
        if (result != Success)
            return result;
    }

    if (parentFolderId != folder->parentFolderId()) {
        // QMailAccount contains a copy of the folder data; we need to tell it to reload
        if (parentFolderId.isValid()) {
            if (parentAccountId.isValid()) {
                modifiedAccountIds->append(parentAccountId);
            } else {
                qWarning() << "Unable to find parent account for folder" << folder->id();
            }
        }
        if (folder->parentFolderId().isValid() && folder->parentAccountId().isValid() && !modifiedAccountIds->contains(folder->parentAccountId()))
            modifiedAccountIds->append(folder->parentAccountId());

        {
            //remove existing links to this folder
            QSqlQuery query(simpleQuery("DELETE FROM mailfolderlinks WHERE descendantid = ?",
                                        QVariantList() << folder->id().toULongLong(),
                                        "mailfolderlinks delete in update"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        {
            //add links to the new parent
            QSqlQuery query(simpleQuery("INSERT INTO mailfolderlinks "
                                        "SELECT DISTINCT id,? FROM mailfolderlinks WHERE descendantid=?",
                                        QVariantList() << folder->id().toULongLong() 
                                                       << folder->parentFolderId().toULongLong(),
                                        "mailfolderlinks insert ancestors"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        {
            QSqlQuery query(simpleQuery("INSERT INTO mailfolderlinks VALUES (?,?)",
                                        QVariantList() << folder->parentFolderId().toULongLong()
                                                       << folder->id().toULongLong(),
                                        "mailfolderlinks insert parent"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
    }
        
    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit folder update to database";
        return DatabaseFailure;
    }

    //update the folder cache
    if (folderCache.contains(folder->id()))
        folderCache.insert(*folder);

    updatedFolderIds->append(folder->id());
    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptUpdateMessage(QMailMessageMetaData *metaData, QMailMessage *message,
                                                                         QMailMessageIdList *all_updatedMessageIds, QMailThreadIdList *all_modifiedThreads, QMailMessageIdList *all_modifiedMessageIds, QMailFolderIdList *all_modifiedFolderIds, QMailAccountIdList *all_modifiedAccountIds, QMap<QString, QStringList> *deleteLaterContent,
                                                                         Transaction &t, bool commitOnSuccess)
{
    if (!metaData->id().isValid())
        return Failure;

    quint64 updateId = metaData->id().toULongLong();

    QMailAccountId parentAccountId;
    QMailFolderId parentFolderId;
    QMailMessageId responseId;
    QString contentUri;
    QMailFolderIdList folderIds;
    quint64 status;

    QMailMessageKey::Properties updateProperties;
    QVariantList extractedValues;

    if (message) {
        // Ensure the part reference info is stored into the message
        ReferenceStorer refStorer(message);
        const_cast<const QMailMessage*>(message)->foreachPart<ReferenceStorer&>(refStorer);
    }

    // Force evaluation of preview, to dirty metadata if it's changed
    metaData->preview();

    if (metaData->dataModified()) {
        // Assume all the meta data fields have been updated
        updateProperties = QMailStorePrivate::updatableMessageProperties();
    }

    // Do we actually have an update to perform?

    bool updateContent(message && message->contentModified());

    if (metaData->dataModified() || updateContent) {
        // Find the existing properties
        {
            QSqlQuery query(simpleQuery("SELECT parentaccountid,parentfolderid,responseid,mailfile,status FROM mailmessages WHERE id=?",
                                        QVariantList() << updateId,
                                        "updateMessage existing properties query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            if (query.first()) {
                parentAccountId = QMailAccountId(extractValue<quint64>(query.value(0)));
                parentFolderId = QMailFolderId(extractValue<quint64>(query.value(1)));
                responseId = QMailMessageId(extractValue<quint64>(query.value(2)));
                contentUri = extractValue<QString>(query.value(3));
                status = extractValue<quint64>(query.value(4));

                // Find any folders affected by this update
                folderIds.append(metaData->parentFolderId());
                if (parentFolderId != metaData->parentFolderId()) {
                    // The previous location folder has also changed
                    folderIds.append(parentFolderId);
                }

                // Ancestor folders are also considered to be affected
                AttemptResult result;
                folderIds += folderAncestorIds(folderIds, true, &result);
                if (result != Success)
                    return result;
            } else {
                qWarning() << "Could not query parent account, folder and content URI";
                return Failure;
            }
        }

        bool replyOrForward(false);
        QString baseSubject(QMail::baseSubject(metaData->subject(), &replyOrForward));
        QStringList missingReferences;
        bool missingAncestor(false);

        if (updateContent || (message && (!metaData->inResponseTo().isValid() || (metaData->inResponseTo() != responseId)))) {
            // Does this message have any references to resolve?
            QStringList references(identifierValues(message->headerFieldText("References")));
            QString predecessor(identifierValue(message->headerFieldText("In-Reply-To")));
            if (!predecessor.isEmpty()) {
                if (references.isEmpty() || (references.last() != predecessor)) {
                    references.append(predecessor);
                }
            }

            AttemptResult result = messagePredecessor(metaData, references, baseSubject, replyOrForward, &missingReferences, &missingAncestor);
            if (result != Success)
                return result;
        }

        //a content scheme may not be supplied
        updateProperties &= ~QMailMessageKey::ContentScheme;

        if (updateContent && !metaData->contentScheme().isEmpty()) {

            updateProperties |= QMailMessageKey::ContentIdentifier;

            bool addContent(updateContent && contentUri.isEmpty());
            if (addContent)
                updateProperties |= QMailMessageKey::ContentScheme;

            // We need to update the content for this message
            if (metaData->contentScheme().isEmpty()) {
                // Use the default storage scheme
                metaData->setContentScheme(defaultContentScheme());
            }

            MutexGuard lock(contentManagerMutex());
            lock.lock();

            QStringList schemes(QStringList() << QMailContentManagerFactory::defaultFilterScheme()
                                              << metaData->contentScheme()
                                              << QMailContentManagerFactory::defaultIndexerScheme());

            foreach(QString const& scheme, schemes)
            {
                if (!scheme.isEmpty()) {
                    if (QMailContentManager *contentManager = QMailContentManagerFactory::create(scheme)) {
                        QString contentUri(::contentUri(*metaData));

                        if (addContent) {
                            // We need to add this content to the message
                            QMailStore::ErrorCode code = contentManager->add(message, durability(commitOnSuccess));
                            if (code != QMailStore::NoError) {
                                setLastError(code);
                                qWarning() << "Unable to add message content to URI:" << contentUri << "for scheme" << scheme;
                                return Failure;
                            }
                        } else {

                            QString oldContentIdentifier(message->contentIdentifier());
                            QMailStore::ErrorCode code = contentManager->update(message, QMailContentManager::NoDurability);
                            if (code == QMailStore::NoError) {
                                QMap<QString, QStringList>::iterator it(deleteLaterContent->find(scheme));
                                if (it == deleteLaterContent->end())
                                    deleteLaterContent->insert(scheme, QStringList() << oldContentIdentifier);
                                else
                                    it.value().append(oldContentIdentifier);
                            } else {
                                qWarning() << "Unable to update message content:" << contentUri;
                                if (code == QMailStore::ContentNotRemoved) {
                                    // The existing content could not be removed - try again later
                                    if (!obsoleteContent(contentUri)) {
                                        setLastError(QMailStore::FrameworkFault);
                                        return Failure;
                                    }
                                } else {
                                    setLastError(code);
                                    return Failure;
                                }
                            }
                        }
                    } else {
                        qWarning() << "Unable to create content manager for scheme:" << metaData->contentScheme();
                        return Failure;
                    }
                }
             }
            metaData->setContentIdentifier(message->contentIdentifier());
        }

        if (metaData->inResponseTo() != responseId) {
            // We need to record this change
            updateProperties |= QMailMessageKey::InResponseTo;
            updateProperties |= QMailMessageKey::ResponseType;

            // Join this message's thread to the predecessor's thread
            quint64 threadId = 0;

            if (metaData->inResponseTo().isValid()) {
                {
                    QSqlQuery query(simpleQuery("SELECT parentthreadid FROM mailmessages WHERE id=?",
                                                QVariantList() << updateId,
                                                "updateMessage mailmessages query"));
                    if (query.lastError().type() != QSqlError::NoError)
                        return DatabaseFailure;

                    if (query.first()) {
                        threadId = extractValue<quint64>(query.value(0));
                    }
                }
                if (threadId && metaData->parentThreadId().toULongLong() != threadId) {
                    {
                        QSqlQuery query(simpleQuery("UPDATE mailmessages SET parentthreadid=(SELECT parentthreadid FROM mailmessages WHERE id=?) WHERE parentthreadid=?",
                                                    QVariantList() << metaData->inResponseTo().toULongLong() << threadId,
                                                    "updateMessage mailmessages update query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;
                    }
                    {
                        // TODO: fix other columns as well if necessary
                        QSqlQuery query(simpleQuery("UPDATE mailthreads "
                                                    "SET messagecount = messagecount + (SELECT messagecount FROM mailthreads WHERE id=?), "
                                                    " unreadcount = unreadcount + (SELECT unreadcount FROM mailthreads WHERE id=?), "
                                                    " status = (status | (SELECT status FROM mailthreads WHERE id=?))"
                                                    "WHERE id=(SELECT parentthreadid FROM mailmessages WHERE id=?)",
                                                    QVariantList() << threadId << threadId << threadId << metaData->inResponseTo().toULongLong(),
                                                    "updateMessage mailthreads update query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;
                    }
                    {
                        QSqlQuery query(simpleQuery("DELETE FROM mailthreads WHERE id=?",
                                                    QVariantList() << threadId,
                                                    "updateMessage mailthreads delete query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;
                    }
                }
            } else {
                // This message is no longer associated with the thread of the former predecessor
                QMailMessageIdList descendantIds;

                {
                    QMailMessageIdList parentIds;

                    parentIds.append(QMailMessageId(updateId));

                    // Find all descendants of this message
                    while (!parentIds.isEmpty()) {
                        QSqlQuery query(simpleQuery("SELECT id FROM mailmessages",
                                                    Key("responseid", QMailMessageKey::id(parentIds)),
                                                    "updateMessage mailmessages responseid query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;

                        while (!parentIds.isEmpty()) {
                            descendantIds.append(parentIds.takeFirst());
                        }

                        while (query.next())
                            parentIds.append(QMailMessageId(extractValue<quint64>(query.value(0))));
                    }
                }

                {
                    // Add a new thread for this message
                    QMap<QString, QVariant> values;
                    values.insert("messagecount", 1);
                    values.insert("unreadcount", metaData->status() & QMailMessage::Read ? 0 : 1);
                    values.insert("serveruid", "");
                    values.insert("parentaccountid", metaData->parentAccountId().toULongLong());
                    values.insert("subject", metaData->subject());
                    values.insert("preview", metaData->preview());
                    values.insert("senders", metaData->from().toString());
                    values.insert("lastdate", metaData->date().toUTC());
                    values.insert("starteddate", metaData->date().toUTC());
                    values.insert("status", metaData->status());
                    const QString &columns = QStringList(values.keys()).join(",");
                    QSqlQuery query(simpleQuery(QString("INSERT INTO mailthreads (%1) VALUES %2").arg(columns).arg(expandValueList(values.count())),
                                                values.values(),
                                                "addMessage mailthreads insert query"));
                    if (query.lastError().type() != QSqlError::NoError)
                        return DatabaseFailure;

                    threadId = extractValue<quint64>(query.lastInsertId());
                }

                {
                    Q_ASSERT(threadId);
                    // Migrate descendants to the new thread
                    QSqlQuery query(simpleQuery("UPDATE mailmessages SET parentthreadid=?",
                                                QVariantList() << threadId,
                                                Key("id", QMailMessageKey::id(descendantIds)),
                                                "updateMessage mailmessages descendants update query"));
                    if (query.lastError().type() != QSqlError::NoError)
                        return DatabaseFailure;
                }
            }
            
            // Remove any missing message/ancestor references associated with this message

            {
                QSqlQuery query(simpleQuery("DELETE FROM missingmessages WHERE id=?",
                                            QVariantList() << updateId,
                                            "updateMessage missingmessages delete query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            {
                QSqlQuery query(simpleQuery("DELETE FROM missingancestors WHERE messageid=?",
                                            QVariantList() << updateId,
                                            "updateMessage missingancestors delete query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }
        }

        if (updateProperties != QMailMessageKey::Properties()) {

            // Check NB#294937. Sometimes metaData contains 0 as a parentThreadId
            // and that is really bad for a thread mode.
            if (!metaData->parentThreadId().isValid())
            {
                //Dirty hack to fix NB#297007, but at least it is better then nothing
                QMailMessageMetaData data(metaData->id());
                if (data.parentThreadId().isValid())
                    metaData->setParentThreadId(data.parentThreadId());
                updateProperties &= ~QMailMessageKey::ParentThreadId;
            }
            extractedValues = messageValues(updateProperties, *metaData);
            {
                QString sql("UPDATE mailmessages SET %1 WHERE id=?");

                QSqlQuery query(simpleQuery(sql.arg(expandProperties(updateProperties, true)),
                                            extractedValues + (QVariantList() << updateId),
                                            "updateMessage mailmessages update"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            // perhaps, we need to update some thread's columns
            // TODO: check other columns.
            if (metaData->parentThreadId().isValid()) {
                QMailAccount account(metaData->parentAccountId());
                QMailFolderId trashFolderId = account.standardFolder(QMailFolder::TrashFolder);
                QMailFolderId draftFolderId = account.standardFolder(QMailFolder::DraftsFolder);
                const bool& movedToTrashOrDraft = (((metaData->status() & (QMailMessage::Trash | QMailMessage::Draft)) != 0)
                        || (trashFolderId != QMailFolder::LocalStorageFolderId && metaData->parentFolderId() == trashFolderId)
                        || (draftFolderId != QMailFolder::LocalStorageFolderId && metaData->parentFolderId() == draftFolderId) )
                        && metaData->parentFolderId() != parentFolderId;
                const bool& movedFromTrashOrDraft = ((parentFolderId == trashFolderId || parentFolderId == draftFolderId
                      || ((status & (QMailMessage::Trash | QMailMessage::Draft)) != (metaData->status() & (QMailMessage::Trash | QMailMessage::Draft)))) &&
                                                    (metaData->parentFolderId() != parentFolderId));
                // if message was moved to/from Trash or Draft folder we should update all threads values in an appropriate way
                if (movedToTrashOrDraft || movedFromTrashOrDraft) {
                        //It is easier to recalculate all thread values, because we must check all threads messages to understand should we
                        //change thread status or not.
                        const QMailThreadIdList idList = QMailThreadIdList() << metaData->parentThreadId();
                        QMailThreadIdList deletedThreadIds; // FIXME: add deletedThreadIds as argument for updateMessage().
                        if (!recalculateThreadsColumns(idList, deletedThreadIds))
                            return DatabaseFailure;
                        APPEND_UNIQUE(all_modifiedThreads, metaData->parentThreadId());
                } else {
                    QMailThread thread(metaData->parentThreadId());
                    const bool& updatePreview = (metaData->date() >= thread.lastDate()) && (thread.preview() != metaData->preview()) && !metaData->preview().isEmpty();
                    const bool& updateSubject = (metaData->inResponseTo() == QMailMessageId()) && (metaData->date().toUTC() == thread.startedDate().toUTC());
                    const bool& messageUnreadStatusChanged = (status & QMailMessage::Read) != (metaData->status() & QMailMessage::Read);
                    const bool& threadStatusChanged = (thread.status() & metaData->status()) != 0;
                    const bool& threadSendersChanged = !thread.senders().contains(metaData->from()) || metaData->date() > thread.lastDate();

                    if (updatePreview || updateSubject || messageUnreadStatusChanged || threadStatusChanged || threadSendersChanged) {
                        QString senders;
                        if (threadSendersChanged) {
                            if (metaData->date() > thread.lastDate()) {
                                QMailAddressList oldSendersList = thread.senders();
                                oldSendersList.removeAll(metaData->from());
                                oldSendersList.prepend(metaData->from());
                                senders = QMailAddress::toStringList(oldSendersList).join(",");
                            } else {
                                senders = QMailAddress::toStringList(QMailAddressList() << metaData->from() << thread.senders()).join(",");
                            }
                        }
                        AttemptResult res = updateThreadsValues(QMailThreadIdList(), QMailThreadIdList() << metaData->parentThreadId(),
                                                                ThreadUpdateData(0, messageUnreadStatusChanged ? ((metaData->status() & QMailMessage::Read) ? -1 : 1) : 0,
                                                                                 updateSubject ? metaData->subject() : QString(),
                                                                                 updatePreview ? metaData->preview() : QString(),
                                                                                 senders, QMailTimeStamp(), QMailTimeStamp(),
                                                                                 metaData->status() ));
                        if (res != Success)
                            return res;
                        APPEND_UNIQUE(all_modifiedThreads, metaData->parentThreadId());
                    }
                }
            }
        }

        if (metaData->customFieldsModified()) {
            AttemptResult result = updateCustomFields(updateId, metaData->customFields(), "mailmessagecustom");
            if (result != Success)
                return result;

            updateProperties |= QMailMessageKey::Custom;
        }

        if (updateProperties & QMailMessageKey::Subject) {
            if (!baseSubject.isEmpty()) {
                // Ensure that this subject is in the subjects table
                AttemptResult result = registerSubject(baseSubject, updateId, metaData->inResponseTo(), missingAncestor);
                if (result != Success)
                    return result;
            }
        }

        bool updatedIdentifier(false);
        QString messageIdentifier;

        if (updateContent) {
            // We may have a change in the message identifier
            QString existingIdentifier;

            {
                QSqlQuery query(simpleQuery("SELECT identifier FROM mailmessageidentifiers WHERE id=?",
                                            QVariantList() << updateId,
                                            "updateMessage existing identifier query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;

                if (query.first()) {
                    existingIdentifier = extractValue<QString>(query.value(0));
                }
            }

            messageIdentifier = identifierValue(message->headerFieldText("Message-ID"));

            if (messageIdentifier != existingIdentifier) {
                if (!messageIdentifier.isEmpty()) {
                    updatedIdentifier = true;

                    if (!existingIdentifier.isEmpty()) {
                        QSqlQuery query(simpleQuery("UPDATE mailmessageidentifiers SET identifier=? WHERE id=?",
                                                    QVariantList() << messageIdentifier << updateId,
                                                    "updateMessage mailmessageidentifiers update query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;
                    } else {
                        // Add the new value
                        QSqlQuery query(simpleQuery("INSERT INTO mailmessageidentifiers (id,identifier) VALUES (?,?)",
                                                    QVariantList() << updateId << messageIdentifier,
                                                    "updateMessage mailmessageidentifiers insert query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;
                    }
                } else {
                    if (!existingIdentifier.isEmpty()) {
                        // Remove any existing value
                        QSqlQuery query(simpleQuery("DELETE FROM mailmessageidentifiers WHERE id=?",
                                                    QVariantList() << updateId,
                                                    "updateMessage mailmessageidentifiers delete query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;
                    }
                }
            }

            if (!missingReferences.isEmpty()) {
                // Add the missing references to the missing messages table
                QVariantList refs;
                QVariantList levels;

                int level = missingReferences.count();
                foreach (const QString &ref, missingReferences) {
                    refs.append(QVariant(ref));
                    levels.append(QVariant(--level));
                }

                {
                    QSqlQuery query(simpleQuery("DELETE FROM missingmessages WHERE id=?",
                                                QVariantList() << updateId,
                                                "addMessage missingmessages delete query"));
                    if (query.lastError().type() != QSqlError::NoError)
                        return DatabaseFailure;
                }

                {
                    QString sql("INSERT INTO missingmessages (id,identifier,level) VALUES (%1,?,?)");
                    QSqlQuery query(batchQuery(sql.arg(QString::number(updateId)),
                                               QVariantList() << QVariant(refs) << QVariant(levels),
                                               "addMessage missingmessages insert query"));
                    if (query.lastError().type() != QSqlError::NoError)
                        return DatabaseFailure;
                }
            }
        }

        if (updatedIdentifier || (updateProperties & QMailMessageKey::InResponseTo)) {
            // See if this message resolves any missing message items
            QMailMessageIdList updatedMessageIds;
            AttemptResult result = resolveMissingMessages(messageIdentifier, metaData->inResponseTo(), baseSubject, *metaData, &updatedMessageIds);
            APPEND_UNIQUE(all_updatedMessageIds, &updatedMessageIds);
            if (result != Success)
                return result;

            if (!updatedMessageIds.isEmpty()) {
                // Find the set of folders and accounts whose contents are modified by these messages
                result = affectedByMessageIds(updatedMessageIds, all_modifiedFolderIds, all_modifiedAccountIds);
                if (result != Success)
                    return result;
            }
        }
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit message update to database";
        return DatabaseFailure;
    }

    if (parentAccountId.isValid()) {
        // The message is now up-to-date with data store
        metaData->setUnmodified();

        if (messageCache.contains(metaData->id())) {
            QMailMessageMetaData cachedMetaData = messageCache.lookup(metaData->id());
            if (!extractedValues.isEmpty()) {
                // Update the cache with the modifications we recorded
                updateMessageValues(updateProperties, extractedValues, metaData->customFields(), cachedMetaData);
                cachedMetaData.setUnmodified();
                messageCache.insert(cachedMetaData);
            }
            uidCache.insert(qMakePair(cachedMetaData.parentAccountId(), cachedMetaData.serverUid()), cachedMetaData.id());
        }

        APPEND_UNIQUE(all_updatedMessageIds, metaData->id());
        APPEND_UNIQUE(all_modifiedFolderIds, &folderIds);

        if (metaData->parentAccountId().isValid())
            APPEND_UNIQUE(all_modifiedAccountIds, metaData->parentAccountId());
        if (parentAccountId.isValid()) {
            if (parentAccountId != metaData->parentAccountId())
                APPEND_UNIQUE(all_modifiedAccountIds, parentAccountId);
        }
    }

    if (updateContent) {
        APPEND_UNIQUE(all_modifiedMessageIds, metaData->id());
    }

    foreach (const QMailThreadId& id, *all_modifiedThreads) {
        threadCache.remove(id);
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptUpdateMessagesMetaData(const QMailMessageKey &key, const QMailMessageKey::Properties &props, const QMailMessageMetaData &data,
                                                                                  QMailMessageIdList *updatedMessageIds, QMailThreadIdList* deletedThreadIds, QMailThreadIdList* modifiedThreadIds, QMailFolderIdList *modifiedFolderIds, QMailAccountIdList *modifiedAccountIds,
                                                                                  Transaction &t, bool commitOnSuccess)
{
    //do some checks first
    if (props & QMailMessageKey::Id) {
        qWarning() << "Updating of messages IDs is not supported";
        return Failure;
    }
    
    QMailMessageKey::Properties properties(props);

    if (properties & QMailMessageKey::ParentFolderId) {
        if (!idExists(data.parentFolderId())) {
            qWarning() << "Update of messages failed. Parent folder does not exist";
            return Failure;
        }
    }

    QVariantList extractedValues;

    //get the valid ids
    *updatedMessageIds = queryMessages(key, QMailMessageSortKey(), 0, 0);
    if (!updatedMessageIds->isEmpty()) {
        // Find the set of folders and accounts whose contents are modified by this update
        QMailMessageKey modifiedMessageKey(QMailMessageKey::id(*updatedMessageIds));
        AttemptResult result = affectedByMessageIds(*updatedMessageIds, modifiedFolderIds, modifiedAccountIds);
        if (result != Success)
            return result;

        // If we're setting parentFolderId, that folder is modified also
        if (properties & QMailMessageKey::ParentFolderId) {
            if (!modifiedFolderIds->contains(data.parentFolderId()))
                modifiedFolderIds->append(data.parentFolderId());
        }

        if (properties & QMailMessageKey::Custom) {
            // Here, we can't compare the input to each target individually.  Instead, remove
            // all custom fields from the affected messages, and add (or re-add) the new ones
            QVariantList addedFields;
            QVariantList addedValues;

            const QMap<QString, QString> &fields = data.customFields();
            QMap<QString, QString>::const_iterator it = fields.begin(), end = fields.end();
            for ( ; it != end; ++it) {
                addedFields.append(QVariant(it.key()));
                addedValues.append(QVariant(it.value()));
            }

            {
                // Remove the obsolete fields
                QSqlQuery query(simpleQuery("DELETE FROM mailmessagecustom",
                                            Key(modifiedMessageKey),
                                            "updateMessagesMetaData mailmessagecustom delete query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            if (!addedFields.isEmpty()) {
                foreach (const QMailMessageId &id, *updatedMessageIds) {
                    // Batch insert the added fields
                    QString sql("INSERT INTO mailmessagecustom (id,name,value) VALUES (%1,?,?)");
                    QSqlQuery query(batchQuery(sql.arg(QString::number(id.toULongLong())),
                                               QVariantList() << QVariant(addedFields)
                                                              << QVariant(addedValues),
                                               "updateMessagesMetaData mailmessagecustom insert query"));
                    if (query.lastError().type() != QSqlError::NoError)
                        return DatabaseFailure;
                }
            }

            properties &= ~QMailMessageKey::Custom;
        }

        if (properties != 0) {
            {
                QString sql("SELECT parentthreadid FROM mailmessages WHERE id IN %1");
                QVariantList bindValues;
                foreach (const QMailMessageId &messageId, *updatedMessageIds)
                {
                    bindValues << messageId.toULongLong();
                }

                QSqlQuery query(simpleQuery(sql.arg(expandValueList(bindValues)),
                                            bindValues,
                                            "updateMessagesMetaData mailmessages query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;

                while (query.next())
                    modifiedThreadIds->append(QMailThreadId(extractValue<quint64>(query.value(0))));
            }
            {
            extractedValues = messageValues(properties, data);
            QString sql("UPDATE mailmessages SET %1");
            QSqlQuery query(simpleQuery(sql.arg(expandProperties(properties, true)),
                                        extractedValues,
                                        Key(modifiedMessageKey),
                                        "updateMessagesMetaData mailmessages query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
            // now let's check any changes in threads.
            // it's easier to update all thread's data, because otherwise we should check
            // are there any changes in status, unreadcount etc. or not.
            bool res = recalculateThreadsColumns(*modifiedThreadIds, *deletedThreadIds);
            if (!res)
                return Failure;
        }
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit metadata update to database";
        return DatabaseFailure;
    }

    // Update the header cache
    foreach (const QMailMessageId& id, *updatedMessageIds) {
        if (messageCache.contains(id)) {
            QMailMessageMetaData cachedMetaData = messageCache.lookup(id);
            updateMessageValues(props, extractedValues, data.customFields(), cachedMetaData);
            cachedMetaData.setUnmodified();
            messageCache.insert(cachedMetaData);
            uidCache.insert(qMakePair(cachedMetaData.parentAccountId(), cachedMetaData.serverUid()), cachedMetaData.id());
        }
    }

    foreach (const QMailThreadId& id, *modifiedThreadIds) {
        threadCache.remove(id);
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptUpdateMessagesStatus(const QMailMessageKey &key, quint64 status, bool set,
                                                                                QMailMessageIdList *updatedMessageIds, QMailThreadIdList* modifiedThreadIds, QMailFolderIdList *modifiedFolderIds, QMailAccountIdList *modifiedAccountIds,
                                                                                Transaction &t, bool commitOnSuccess)
{

    //get the valid ids
    *updatedMessageIds = queryMessages(key, QMailMessageSortKey(), 0, 0);

    if (!updatedMessageIds->isEmpty()) {
        // Find the set of folders and accounts whose contents are modified by this update
        AttemptResult result = affectedByMessageIds(*updatedMessageIds, modifiedFolderIds, modifiedAccountIds);
        if (result != Success)
            return result;

        // perhaps, we need to update unreadcount column or status column in mailthreads table
        QVariantList bindMessagesIds;
        QVariantList bindMessagesIdsBatch;
            foreach (const QMailMessageId& id, *updatedMessageIds)
            {
                const QMailThreadId &threadId = QMailMessageMetaData(id).parentThreadId();
            if (!modifiedThreadIds->contains(threadId) && threadId.isValid())
                modifiedThreadIds->append(threadId);
            bindMessagesIds << id.toULongLong();
            }
        if ( (status & QMailMessage::Read)) {
            foreach (const QMailThreadId& threadId, *modifiedThreadIds)
            {
                if (threadId.isValid()) {

                    QList<quint64> oldStatusList;

                    while (!bindMessagesIds.isEmpty()) {
                        bindMessagesIdsBatch.clear();
                        bindMessagesIdsBatch = bindMessagesIds.mid(0,500);
                        if (bindMessagesIds.count() > 500) {
                            bindMessagesIds = bindMessagesIds.mid(500);
                        } else {
                            bindMessagesIds.clear();
                        }
                        QString sql("SELECT status FROM mailmessages WHERE id IN %1 and parentthreadid = %2");
                        QSqlQuery query(simpleQuery(sql.arg(expandValueList(bindMessagesIdsBatch)).arg(threadId.toULongLong()),
                                                    bindMessagesIdsBatch,
                                                    "status mailmessages query"));
                        if (query.lastError().type() != QSqlError::NoError)
                            return DatabaseFailure;

                        while (query.next())
                            oldStatusList.append(query.value(0).toULongLong());
                    }
                    qlonglong unreadCount = 0;
                    foreach (const quint64& oldStatus, oldStatusList)
                    {
                        if (set != bool(oldStatus & QMailMessage::Read)) {
                            set ? --unreadCount : ++unreadCount;
                        }
                    }
                    QMailThread thread(threadId);
                    const bool threadStatusChanged = thread.status() != (thread.status() | status);
                    if (unreadCount != 0 || threadStatusChanged) {
                        AttemptResult res = updateThreadsValues(QMailThreadIdList(), QMailThreadIdList() << threadId, ThreadUpdateData(0, unreadCount, set ? status : 0 - status));
                        if (res != Success)
                            return res;
                        APPEND_UNIQUE(modifiedThreadIds, threadId);
                    }
                }
            }

        }

        QString sql;
        if (set) {
            sql = QString("UPDATE mailmessages SET status=(status | %1)").arg(status);
        } else {
            // essentially SET status &= ~unsetmask
            // but sqlite can't handle a large unsetmask, so use or and xor,
            // but sqllite doesn't support xor so use & and |.
            sql = QString("UPDATE mailmessages SET status=(~((status|%1)& %1))&(status|%1)").arg(status);
        }
        QSqlQuery query(simpleQuery(sql, Key(QMailMessageKey::id(*updatedMessageIds)),
                                        "updateMessagesMetaData status query"));
        if (query.lastError().type() != QSqlError::NoError) {
            return DatabaseFailure;
        }
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit metadata status update to database";
        return DatabaseFailure;
    }

    // Update the header cache
    foreach (const QMailMessageId& id, *updatedMessageIds) {
        if (messageCache.contains(id)) {
            QMailMessageMetaData cachedMetaData = messageCache.lookup(id);
            quint64 newStatus = cachedMetaData.status();
            newStatus = set ? (newStatus | status) : (newStatus & ~status);
            cachedMetaData.setStatus(newStatus);
            cachedMetaData.setUnmodified();
            messageCache.insert(cachedMetaData);
            uidCache.insert(qMakePair(cachedMetaData.parentAccountId(), cachedMetaData.serverUid()), cachedMetaData.id());
        }
    }

    foreach (const QMailThreadId& id, *modifiedThreadIds) {
        if (threadCache.contains(id)) {
            threadCache.remove(id);
        }
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptPurgeMessageRemovalRecords(const QMailAccountId &accountId, const QStringList &serverUids,
                                                                                      Transaction &t, bool commitOnSuccess)
{
    QMailMessageIdList removalIds;

    {
        QString sql("SELECT id FROM deletedmessages WHERE parentaccountid=?");

        QVariantList bindValues;
        bindValues << accountId.toULongLong();

        if (!serverUids.isEmpty()) {
            QVariantList uidValues;
            foreach (const QString& uid, serverUids)
                uidValues.append(uid);

            sql.append(" AND serveruid IN %1");
            sql = sql.arg(expandValueList(uidValues));

            bindValues << uidValues;
        }

        QSqlQuery query(simpleQuery(sql, 
                                    bindValues,
                                    "purgeMessageRemovalRecord info query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        while (query.next())
            removalIds.append(QMailMessageId(extractValue<quint64>(query.value(0))));
    }

    // anything to remove?
    if (!removalIds.isEmpty()) {
        QSqlQuery query(simpleQuery("DELETE FROM deletedmessages",
                                    Key(QMailMessageKey::id(removalIds)),
                                    "purgeMessageRemovalRecord delete query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit message removal record deletion to database";
        return DatabaseFailure;
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptEnsureDurability(Transaction &t, bool commitOnSuccess)
{
    QSqlQuery query(simpleQuery("PRAGMA wal_checkpoint(FULL)", "ensure durability query"));
    if (query.lastError().type() != QSqlError::NoError) {
        qWarning() << "Could not ensure durability of mail store";
        return DatabaseFailure;
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit message removal record deletion to database";
        return DatabaseFailure;
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptCountAccounts(const QMailAccountKey &key, int *result, 
                                                                         ReadLock &)
{
#ifdef USE_ACCOUNTS_QT
    QMailAccountIdList accountIDList = searchSSOAccounts(key);
    *result =  accountIDList.count();
#else
    QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM mailaccounts",
                                Key(key),
                                "countAccounts mailaccounts query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first())
        *result = extractValue<int>(query.value(0));
#endif
    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptCountFolders(const QMailFolderKey &key, int *result, 
                                                                        ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM mailfolders",
                                Key(key),
                                "countFolders mailfolders query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first())
        *result = extractValue<int>(query.value(0));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptCountThreads(const QMailThreadKey &key,
                                                                         int *result,
                                                                         ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM mailthreads",
                                Key(key),
                                "countThreads count query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first())
        *result = extractValue<int>(query.value(0));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptCountMessages(const QMailMessageKey &key, 
                                                                         int *result, 
                                                                         ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM mailmessages",
                                Key(key),
                                "countMessages mailmessages query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first())
        *result = extractValue<int>(query.value(0));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptSizeOfMessages(const QMailMessageKey &key, 
                                                                          int *result, 
                                                                          ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT SUM(size) FROM mailmessages",
                                Key(key),
                                "sizeOfMessages mailmessages query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first())
        *result = extractValue<int>(query.value(0));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptQueryAccounts(const QMailAccountKey &key, const QMailAccountSortKey &sortKey, uint limit, uint offset,
                                                                         QMailAccountIdList *ids, 
                                                                         ReadLock &)
{
#ifdef USE_ACCOUNTS_QT
    QMailAccountIdList accountIDList = searchSSOAccounts(key, sortKey);

    if (limit)
        *ids << accountIDList.mid(offset, limit);
    else
        *ids << accountIDList.mid(offset, -1);
#else
    QSqlQuery query(simpleQuery("SELECT id FROM mailaccounts",
                                QVariantList(),
                                QList<Key>() << Key(key) << Key(sortKey),
                                qMakePair(limit, offset),
                                "queryAccounts mailaccounts query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        ids->append(QMailAccountId(extractValue<quint64>(query.value(0))));
#endif
    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptQueryThreads(const QMailThreadKey &key, const QMailThreadSortKey &sortKey, uint limit, uint offset,
                                                                        QMailThreadIdList *ids,
                                                                        ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT id FROM mailthreads",
                                QVariantList(),
                                QList<Key>() << Key(key) << Key(sortKey),
                                qMakePair(limit, offset),
                                "querythreads mailthreads query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        ids->append(QMailThreadId(extractValue<quint64>(query.value(0))));

    //store the results of this call for cache preloading
    lastQueryThreadResult = *ids;

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptQueryFolders(const QMailFolderKey &key, const QMailFolderSortKey &sortKey, uint limit, uint offset,
                                                                        QMailFolderIdList *ids, 
                                                                        ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT id FROM mailfolders",
                                QVariantList(),
                                QList<Key>() << Key(key) << Key(sortKey),
                                qMakePair(limit, offset),
                                "queryFolders mailfolders query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        ids->append(QMailFolderId(extractValue<quint64>(query.value(0))));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptQueryMessages(const QMailMessageKey &key, const QMailMessageSortKey &sortKey, uint limit, uint offset,
                                                                         QMailMessageIdList *ids, 
                                                                         ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT id FROM mailmessages",
                                QVariantList(),
                                QList<Key>() << Key(key) << Key(sortKey),
                                qMakePair(limit, offset),
                                "queryMessages mailmessages query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        ids->append(QMailMessageId(extractValue<quint64>(query.value(0))));

    //store the results of this call for cache preloading
    lastQueryMessageResult = *ids;

    return Success;
}

#ifdef USE_ACCOUNTS_QT
QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAccount(const QMailAccountId &id,
                                                                   QMailAccount *result,
                                                                   ReadLock &)
{
    if (!id.isValid())
        return Failure;

    QSharedPointer<Accounts::Account> ssoAccount = getEmailAccount(id.toULongLong());
    if (!ssoAccount) {
        return Failure;
    }
    Accounts::ServiceList services = ssoAccount->enabledServices();
    Q_ASSERT (services.count() == 1);
    Accounts::Service service = services.first();

    Q_ASSERT(service.serviceType() == "e-mail");

    ssoAccount->selectService(service);

    *result = extractAccount(ssoAccount);
    Q_ASSERT(result->id() == id);

    {
        // Find any standard folders configured for this account
        QSqlQuery query(simpleQuery("SELECT foldertype,folderid FROM mailaccountfolders WHERE id=?",
                                    QVariantList() << id.toULongLong(),
                                    "account mailaccountfolders query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        while (query.next())
            result->setStandardFolder(QMailFolder::StandardFolder(query.value(0).toInt()), QMailFolderId(query.value(1).toULongLong()));
    }

    // Find any custom fields for this SSO account
    QMap<QString, QString> fields;
    AttemptResult attemptResult = accountCustomFields(ssoAccount, &fields);
    if (attemptResult != Success)
        return attemptResult;

    result->setCustomFields(fields);
    result->setCustomFieldsModified(false);

    // Find the type of the account
    foreach (const QString& group, ssoAccount->childGroups()) {
        if (group != "customFields") {
            ssoAccount->beginGroup(group);

            QString serviceType = ssoAccount->valueAsString("servicetype");
            if (serviceType.contains("source"))
                result->addMessageSource(group);

            if (serviceType.contains("sink"))
                result->addMessageSink(group);

            ssoAccount->endGroup();
        }
    }

    //update cache
    accountCache.insert(*result);
    return Success;
}
#else
QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAccount(const QMailAccountId &id, 
                                                                   QMailAccount *result, 
                                                                   ReadLock &)
{
    {
        QSqlQuery query(simpleQuery("SELECT * FROM mailaccounts WHERE id=?",
                                    QVariantList() << id.toULongLong(),
                                    "account mailaccounts query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.first()) {
            *result = extractAccount(query.record());
        }
    }

    if (result->id().isValid()) {
        {
            // Find any standard folders configured for this account
            QSqlQuery query(simpleQuery("SELECT foldertype,folderid FROM mailaccountfolders WHERE id=?",
                                        QVariantList() << id.toULongLong(),
                                        "account mailaccountfolders query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                result->setStandardFolder(QMailFolder::StandardFolder(query.value(0).toInt()), QMailFolderId(query.value(1).toULongLong()));
        }

        // Find any custom fields for this account
        QMap<QString, QString> fields;
        AttemptResult attemptResult = customFields(id.toULongLong(), &fields, "mailaccountcustom");
        if (attemptResult != Success)
            return attemptResult;

        result->setCustomFields(fields);
        result->setCustomFieldsModified(false);

        {
            // Find the type of the account
            QSqlQuery query(simpleQuery("SELECT service,value FROM mailaccountconfig WHERE id=? AND name='servicetype'",
                                        QVariantList() << id.toULongLong(),
                                        "account mailaccountconfig query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next()) {
                QString service(query.value(0).toString());
                QString type(query.value(1).toString());

                if (type.contains("source")) {
                    result->addMessageSource(service);
                }
                if (type.contains("sink")) {
                    result->addMessageSink(service);
                }
            }
        }

        //update cache 
        accountCache.insert(*result);
        return Success;
    }

    return Failure;
}
#endif

#ifdef USE_ACCOUNTS_QT
QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAccountConfiguration(const QMailAccountId &id,
                                                                                QMailAccountConfiguration *result,
                                                                                ReadLock &)
{
    QSharedPointer<Accounts::Account> ssoAccount = getEmailAccount(id.toULongLong());

    if (!ssoAccount)
        return Failure;

    Accounts::ServiceList services = ssoAccount->enabledServices();
    Q_ASSERT (services.count() == 1);

    Accounts::Service service = services.first();

    Q_ASSERT(service.serviceType() == "e-mail");
    ssoAccount->selectService(service);

    foreach (const QString& group, ssoAccount->childGroups()) {
        if (group != "customFields") {
            if (!result->services().contains(group)) {
                // Add this service to the configuration
                result->addServiceConfiguration(group);
            }

            QMailAccountConfiguration::ServiceConfiguration* serviceConfig = &result->serviceConfiguration(group);
            Q_ASSERT(serviceConfig);

            ssoAccount->beginGroup(group);
            foreach (const QString& key, ssoAccount->allKeys()) {
                serviceConfig->setValue(key,ssoAccount->valueAsString(key));
            }
            ssoAccount->endGroup();
        }
    }

    result->setId(id);
    result->setModified(false);

    return Success;
}
#else
QMailStorePrivate::AttemptResult QMailStorePrivate::attemptAccountConfiguration(const QMailAccountId &id, 
                                                                                QMailAccountConfiguration *result, 
                                                                                ReadLock &)
{
    // Find any configuration fields for this account
    QSqlQuery query(simpleQuery("SELECT service,name,value FROM mailaccountconfig WHERE id=? ORDER BY service",
                                QVariantList() << id.toULongLong(),
                                "accountConfiguration mailaccountconfig query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    QString service;
    QMailAccountConfiguration::ServiceConfiguration *serviceConfig = 0;

    while (query.next()) {
        QString svc(query.value(0).toString());
        if (svc != service) {
            service = svc;

            if (!result->services().contains(service)) {
                // Add this service to the configuration
                result->addServiceConfiguration(service);
            }

            serviceConfig = &result->serviceConfiguration(service);
        }

        serviceConfig->setValue(query.value(1).toString(), query.value(2).toString());
    }

    if (service.isEmpty()) {
        // No services - is this an error?
        QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM mailaccounts WHERE id=?",
                                    QVariantList() << id.toULongLong(),
                                    "accountConfiguration mailaccounts query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.first()) {
            if (extractValue<int>(query.value(0)) == 0)
                return Failure;
        }
    } 

    result->setId(id);
    result->setModified(false);

    return Success;
}
#endif

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptThread(const QMailThreadId &id, QMailThread *result, ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT * FROM mailthreads WHERE id=?",
                                QVariantList() << id.toULongLong(),
                                "folder mailfolders query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first()) {
        *result = extractThread(query.record());
    }

    return (result->id().isValid()) ? Success : Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptThreads(const QMailThreadKey& key,
                                                                   QMailStore::ReturnOption option,
                                                                   QMailThreadList *result,
                                                                   ReadLock &)
{
    Q_UNUSED (option);

    QSqlQuery query(simpleQuery("SELECT * FROM mailthreads t0",
                                Key(key, "t0"),
                                "attemptThreads query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        result->append(extractThread(query.record()));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptFolder(const QMailFolderId &id,
                                                                  QMailFolder *result,
                                                                  ReadLock &)
{
    {
        QSqlQuery query(simpleQuery("SELECT * FROM mailfolders WHERE id=?",
                                    QVariantList() << id.toULongLong(),
                                    "folder mailfolders query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.first()) {
            *result = extractFolder(query.record());
        }
    }

    if (result->id().isValid()) {
        // Find any custom fields for this folder
        QMap<QString, QString> fields;
        AttemptResult attemptResult = customFields(id.toULongLong(), &fields, "mailfoldercustom");
        if (attemptResult != Success)
            return attemptResult;

        result->setCustomFields(fields);
        result->setCustomFieldsModified(false);

        //update cache 
        folderCache.insert(*result);
        return Success;
    }

    return Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessage(const QMailMessageId &id, 
                                                                   QMailMessage *result, 
                                                                   ReadLock &)
{
    QMap<QString, QString> fields;

    // Find any custom fields for this message
    AttemptResult attemptResult = customFields(id.toULongLong(), &fields, "mailmessagecustom");
    if (attemptResult != Success)
        return attemptResult;

    QSqlQuery query(simpleQuery("SELECT * FROM mailmessages WHERE id=?",
                                QVariantList() << id.toULongLong(),
                                "message mailmessages id query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first()) {
        *result = extractMessage(query.record(), fields);
        if (result->id().isValid()) {
            result->setId(id);
            return Success;
        }
    }

    return Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessage(const QString &uid, const QMailAccountId &accountId, 
                                                                   QMailMessage *result, 
                                                                   ReadLock &lock)
{
    quint64 id(0);

    AttemptResult attemptResult = attemptMessageId(uid, accountId, &id, lock);
    if (attemptResult != Success)
        return attemptResult;
            
    if (id != 0) {
        return attemptMessage(QMailMessageId(id), result, lock);
    }

    return Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessageMetaData(const QMailMessageId &id,
                                                                           QMailMessageMetaData *result, 
                                                                           ReadLock &)
{
    QMap<QString, QString> fields;

    // Find any custom fields for this message
    AttemptResult attemptResult = customFields(id.toULongLong(), &fields, "mailmessagecustom");
    if (attemptResult != Success)
        return attemptResult;

    QSqlQuery query(simpleQuery("SELECT * FROM mailmessages WHERE id=?",
                                QVariantList() << id.toULongLong(),
                                "message mailmessages id query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first()) {
        *result = extractMessageMetaData(query.record(), fields);
        if (result->id().isValid())
            return Success;
    }

    return Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessageMetaData(const QString &uid, const QMailAccountId &accountId, 
                                                                           QMailMessageMetaData *result, 
                                                                           ReadLock &lock)
{
    quint64 id(0);

    AttemptResult attemptResult = attemptMessageId(uid, accountId, &id, lock);
    if (attemptResult != Success)
        return attemptResult;
            
    if (id != 0) {
        return attemptMessageMetaData(QMailMessageId(id), result, lock);
    }

    return Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessagesMetaData(const QMailMessageKey& key, const QMailMessageKey::Properties &properties, QMailStore::ReturnOption option, 
                                                                            QMailMessageMetaDataList *result, 
                                                                            ReadLock &)
{
    if (properties == QMailMessageKey::Custom) {
        // We're only selecting custom fields
        QString sql("SELECT %1 name, value FROM mailmessagecustom WHERE id IN ( SELECT t0.id FROM mailmessages t0");
        sql += buildWhereClause(Key(key, "t0")) + " )";

        QVariantList whereValues(::whereClauseValues(key));
        QSqlQuery query(simpleQuery(sql.arg(option == QMailStore::ReturnDistinct ? "DISTINCT " : ""),
                                    whereValues,
                                    "messagesMetaData combined query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        // Find all the values for each parameter name in the set
        QMap<QString, QStringList> fields;
        while (query.next())
            fields[query.value(0).toString()].append(query.value(1).toString());

        // Create records for each of these parameters
        int maxLen = 0;
        foreach (const QStringList &list, fields.values())
            maxLen = qMax<uint>(maxLen, list.count());

        for (int i = 0; i < maxLen; ++i)
            result->append(QMailMessageMetaData());

        // Add all pairs to the results
        foreach (const QString &name, fields.keys()) {
            QMailMessageMetaDataList::iterator it = result->begin();
            foreach (const QString &value, fields[name]) {
                (*it).setCustomField(name, value);
                ++it;
            }
        }

        QMailMessageMetaDataList::iterator it = result->begin(), end = result->end();
        for ( ; it != end; ++it)
            (*it).setCustomFieldsModified(false);
    } else {
        bool includeCustom(properties & QMailMessageKey::Custom);
        if (includeCustom && (option == QMailStore::ReturnDistinct)) {
            qWarning() << "Warning: Distinct-ness is not supported with custom fields!";
        }

        QString sql("SELECT %1 %2 FROM mailmessages t0");
        sql = sql.arg(option == QMailStore::ReturnDistinct ? "DISTINCT " : "");

        QMailMessageKey::Properties props(properties);

        bool removeId(false);
        if (includeCustom && !(props & QMailMessageKey::Id)) {
            // We need the ID to match against the custom table
            props |= QMailMessageKey::Id;
            removeId = true;
        }

        {
            QSqlQuery query(simpleQuery(sql.arg(expandProperties(props, false)),
                                        Key(key, "t0"),
                                        "messagesMetaData mailmessages query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                result->append(extractMessageMetaData(query.record(), props, props));
        }

        if (includeCustom) {
            QMailMessageMetaDataList::iterator it = result->begin(), end = result->end();
            for ( ; it != end; ++it) {
                // Add the custom fields to the record
                QMap<QString, QString> fields;
                AttemptResult attemptResult = customFields((*it).id().toULongLong(), &fields, "mailmessagecustom");
                if (attemptResult != Success)
                    return attemptResult;

                QMailMessageMetaData &metaData(*it);
                metaData.setCustomFields(fields);
                metaData.setCustomFieldsModified(false);

                if (removeId)
                    metaData.setId(QMailMessageId());
            }
        }
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessageRemovalRecords(const QMailAccountId &accountId, const QMailFolderId &folderId, 
                                                                                 QMailMessageRemovalRecordList *result, 
                                                                                 ReadLock &)
{
    QVariantList values;
    values << accountId.toULongLong();

    QString sql("SELECT * FROM deletedmessages WHERE parentaccountid=?");
    if (folderId.isValid()) {
        sql += " AND parentfolderid=?";
        values << folderId.toULongLong();
    }

    QSqlQuery query(simpleQuery(sql,
                                values,
                                "messageRemovalRecords deletedmessages query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        result->append(extractMessageRemovalRecord(query.record()));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessageFolderIds(const QMailMessageKey &key, 
                                                                            QMailFolderIdList *result, 
                                                                            ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT DISTINCT t0.parentfolderid FROM mailmessages t0",
                                Key(key, "t0"),
                                "messageFolderIds folder select query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next())
        result->append(QMailFolderId(extractValue<quint64>(query.value(0))));

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptFolderAccountIds(const QMailFolderKey &key, 
                                                                            QMailAccountIdList *result, 
                                                                            ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT DISTINCT parentaccountid FROM mailfolders t0",
                                Key(key, "t0"),
                                "folderAccountIds account select query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next()) {
        QMailAccountId accountId(extractValue<quint64>(query.value(0)));
        if (accountId.isValid()) { // local folders don't have a parent account
            result->append(accountId);
        }
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptFolderAncestorIds(const QMailFolderIdList &ids, 
                                                                             QMailFolderIdList *result, 
                                                                             ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT DISTINCT id FROM mailfolderlinks",
                                Key("descendantid", QMailFolderKey::id(ids)),
                                "folderAncestorIds id select query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next()) {
        QMailFolderId folderId(extractValue<quint64>(query.value(0)));
        if (folderId.isValid()) {
            result->append(folderId);
        } else {
            qWarning() << "Unable to find parent account for folder" << folderId;
        }
    }

    return Success;
}

void QMailStorePrivate::preloadHeaderCache(const QMailMessageId& id) const
{
    QMailMessageIdList idBatch;
    idBatch.append(id);

    int index = lastQueryMessageResult.indexOf(id);
    if (index != -1) {
        // Preload based on result of last call to queryMessages
        int count = 1;

        QMailMessageIdList::const_iterator begin = lastQueryMessageResult.begin();
        QMailMessageIdList::const_iterator end = lastQueryMessageResult.end();
        QMailMessageIdList::const_iterator lowIt = begin + index;
        QMailMessageIdList::const_iterator highIt = lowIt;

        bool ascend(true);
        bool descend(lowIt != begin);

        while ((count < (QMailStorePrivate::lookAhead * 2)) && (ascend || descend)) {
            if (ascend) {
                ++highIt;
                if (highIt == end) {
                    ascend = false;
                } else  {
                    if (!messageCache.contains(*highIt)) {
                        idBatch.append(*highIt);
                        ++count;
                    } else {
                        // Most likely, a sequence in the other direction will be more useful
                        ascend = false;
                    }
                }
            }

            if (descend) {
                --lowIt;
                if (!messageCache.contains(*lowIt)) {
                    idBatch.prepend(*lowIt);
                    ++count;

                    if (lowIt == begin) {
                        descend = false;
                    }
                } else {
                    // Most likely, a sequence in the other direction will be more useful
                    descend = false;
                }
            }
        }
    } else {
        // Don't bother preloading - if there is a query result, we have now searched outside it;
        // we should consider it to have outlived its usefulness
        if (!lastQueryMessageResult.isEmpty())
            lastQueryMessageResult = QMailMessageIdList();
    }

    QMailMessageMetaData result;
    QMailMessageKey key(QMailMessageKey::id(idBatch));
    foreach (const QMailMessageMetaData& metaData, messagesMetaData(key, allMessageProperties(), QMailStore::ReturnAll)) {
        if (metaData.id().isValid()) {
            messageCache.insert(metaData);
            uidCache.insert(qMakePair(metaData.parentAccountId(), metaData.serverUid()), metaData.id());
            if (metaData.id() == id) {
                result = metaData;
        }
    }
    }
}

void QMailStorePrivate::preloadThreadCache(const QMailThreadId& id) const
{
    QMailThreadIdList idBatch;
    idBatch.append(id);

    int index = lastQueryThreadResult.indexOf(id);
    if (index == -1) {
        // Don't bother preloading - if there is a query result, we have now searched outside it;
        // we should consider it to have outlived its usefulness
        if (!lastQueryThreadResult.isEmpty())
            lastQueryThreadResult = QMailThreadIdList();
    } else {

        // Preload based on result of last call to queryMessages
        int count = 1;

        QMailThreadIdList::const_iterator begin = lastQueryThreadResult.begin();
        QMailThreadIdList::const_iterator end = lastQueryThreadResult.end();
        QMailThreadIdList::const_iterator lowIt = begin + index;
        QMailThreadIdList::const_iterator highIt = lowIt;

        bool ascend(true);
        bool descend(lowIt != begin);

        while ((count < (QMailStorePrivate::lookAhead * 2)) && (ascend || descend)) {
            if (ascend) {
                ++highIt;
                if (highIt == end) {
                    ascend = false;
                } else  {
                    if (!threadCache.contains(*highIt)) {
                        idBatch.append(*highIt);
                        ++count;
                    } else {
                        // Most likely, a sequence in the other direction will be more useful
                        ascend = false;
                    }
                }
            }

            if (descend) {
                --lowIt;
                if (!threadCache.contains(*lowIt)) {
                    idBatch.prepend(*lowIt);
                    ++count;

                    if (lowIt == begin) {
                        descend = false;
                    }
                } else {
                    // Most likely, a sequence in the other direction will be more useful
                    descend = false;
                }
            }
        }
    }

    QMailThread result;
    QMailThreadKey key(QMailThreadKey::id(idBatch));
    foreach (const QMailThread &thread, threads(key, QMailStore::ReturnAll)) {
        if (thread.id().isValid()) {
            threadCache.insert(thread);
            if (thread.id() == id)
                result = thread;
            // TODO: populate uid cache
        }
    }
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptStatusBit(const QString &name, const QString &context,
                                                                     int *result,
                                                                     ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT COALESCE(statusbit,0) FROM mailstatusflags WHERE name=? AND context=?",
                                QVariantList() << name << context,
                                "mailstatusflags select"));
    if (query.lastError().type() != QSqlError::NoError) {
        *result = 0;
        return DatabaseFailure;
    }


    if (query.next())
        *result = extractValue<int>(query.value(0));
    else
        *result = 0;

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptRegisterStatusBit(const QString &name, const QString &context, int maximum, bool check, quint64 *result,
                                                                             Transaction &t, bool commitOnSuccess)
{
    if (check) {
        QSqlQuery query(simpleQuery("SELECT COALESCE(statusbit,0) FROM mailstatusflags WHERE name=? AND context=?",
                                    QVariantList() << name << context,
                                    "attemptRegisterStatusBit select"));
        if (query.lastError().type() != QSqlError::NoError) {
            *result = 0;
            return DatabaseFailure;
        }


        if (query.next())
            *result = (static_cast<quint64>(1) << (extractValue<int>(query.value(0))-1));
        else
            *result = 0;

        if (*result) {
            if (commitOnSuccess && !t.commit()) {
                qWarning() << "Could not commit aftering reading status flag";
                return DatabaseFailure;
            }
            return Success;
        }
    } else {
        *result = 0;
    }

    int highest = 0;

    {
        // Find the highest 
        QSqlQuery query(simpleQuery("SELECT MAX(statusbit) FROM mailstatusflags WHERE context=?",
                                    QVariantList() << context,
                                    "mailstatusflags register select"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.next())
            highest = extractValue<int>(query.value(0));
    }

    if (highest == maximum) {
        return Failure;
    } else {
        QSqlQuery query(simpleQuery("INSERT INTO mailstatusflags (name,context,statusbit) VALUES (?,?,?)",
                                    QVariantList() << name << context << (highest + 1),
                                    "mailstatusflags register insert"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
        *result = static_cast<quint64>(1) << highest;
    }

    if (commitOnSuccess && !t.commit()) {
        qWarning() << "Could not commit statusflag changes to database";
        return DatabaseFailure;
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::attemptMessageId(const QString &uid, const QMailAccountId &accountId, 
                                                                     quint64 *result, 
                                                                     ReadLock &)
{
    QSqlQuery query(simpleQuery("SELECT id FROM mailmessages WHERE serveruid=? AND parentaccountid=?",
                                QVariantList() << uid << accountId.toULongLong(),
                                "message mailmessages uid/parentaccountid query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    if (query.first()) {
        *result = extractValue<quint64>(query.value(0));
        return Success;
    }

    return Failure;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::affectedByMessageIds(const QMailMessageIdList &messages, QMailFolderIdList *folderIds, QMailAccountIdList *accountIds) const
{
    AttemptResult result;

    // Find the set of folders whose contents are modified by this update
    QMailFolderIdList messageFolderIds;

    QMailStorePrivate *self(const_cast<QMailStorePrivate*>(this));
    {
        ReadLock l(self);
        result = self->attemptMessageFolderIds(QMailMessageKey::id(messages), &messageFolderIds, l);
    }

    if (result != Success)
        return result;

    return affectedByFolderIds(messageFolderIds, folderIds, accountIds);
}

QMailStorePrivate::AttemptResult QMailStorePrivate::affectedByFolderIds(const QMailFolderIdList &folders, QMailFolderIdList *all_folderIds, QMailAccountIdList *all_accountIds) const
{
    AttemptResult result;

    // Any ancestor folders are also modified
    QMailFolderIdList ancestorIds;

    QMailStorePrivate *self(const_cast<QMailStorePrivate*>(this));
    {
        ReadLock l(self);
        result = self->attemptFolderAncestorIds(folders, &ancestorIds, l);
    }

    if (result != Success)
        return result;

    QMailFolderIdList folderIds;
    folderIds = folders + ancestorIds;
    APPEND_UNIQUE(all_folderIds, &folderIds);

    // Find the set of accounts whose contents are modified by this update
    ReadLock l(self);
    QMailAccountIdList accountIds;
    result = self->attemptFolderAccountIds(QMailFolderKey::id(folderIds), &accountIds, l);
    APPEND_UNIQUE(all_accountIds, &accountIds);
    return result;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::messagePredecessor(QMailMessageMetaData *metaData, const QStringList &references, const QString &baseSubject, bool replyOrForward,
                                                                       QStringList *missingReferences, bool *missingAncestor)
{
    QList<quint64> potentialPredecessors;

    if (!references.isEmpty()) {
        // Find any messages that correspond to these references
        QMap<QString, QList<quint64> > referencedMessages;

        QVariantList refs;
        foreach (const QString &ref, references) {
            refs.append(QVariant(ref));
        }

        {
            QString sql("SELECT id,identifier FROM mailmessageidentifiers WHERE identifier IN %1 AND id IN (SELECT id FROM mailmessages WHERE parentaccountid = %2)");
            QSqlQuery query(simpleQuery(sql.arg(expandValueList(refs)).arg(metaData->parentAccountId().toULongLong()),
                                        refs,
                                        "messagePredecessor mailmessageidentifiers select query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next()) {
                referencedMessages[extractValue<QString>(query.value(1))].append(extractValue<quint64>(query.value(0).toInt()));
            }
        }

        if (referencedMessages.isEmpty()) {
            // All the references are missing
            *missingReferences = references;
            if(findPotentialPredecessorsBySubject(metaData, baseSubject, missingAncestor, potentialPredecessors) == DatabaseFailure)
                return DatabaseFailure;
        } else {
            for (int i = references.count() - 1; i >= 0; --i) {
                const QString &refId(references.at(i));

                QMap<QString, QList<quint64> >::const_iterator it = referencedMessages.find(refId);
                if (it != referencedMessages.end()) {
                    const QList<quint64> &messageIds(it.value());

                    if (messageIds.count() == 1) {
                        // This is the best parent message choice
                        potentialPredecessors.append(messageIds.first());
                        break;
                    } else {
                        // TODO: We need to choose a best selection from amongst these messages
                        // For now, just process the order the DB gave us
                        potentialPredecessors = messageIds;
                        break;
                    }
                } else {
                    missingReferences->append(refId);
                }
            }
        }
    } else if (!baseSubject.isEmpty() && replyOrForward) {
        // This message has a thread ancestor,  but we can only estimate which is the best choice
        *missingAncestor = true;

        // Find the preceding messages of all thread matching this base subject
        QSqlQuery query(simpleQuery("SELECT id FROM mailmessages "
                                    "WHERE id!=? "
                                    "AND parentaccountid=? "
                                    "AND stamp<? "
                                    "AND parentthreadid IN ("
                                            "SELECT threadid FROM mailthreadsubjects WHERE subjectid = ("
                                                "SELECT id FROM mailsubjects WHERE basesubject=?"
                                        ")"
                                     ")"
                                    "ORDER BY stamp DESC",
                                    QVariantList() << metaData->id().toULongLong() 
                                                    << metaData->parentAccountId().toULongLong() 
                                                    << metaData->date().toLocalTime() 
                                                    << baseSubject,
                                    "messagePredecessor mailmessages select query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        while (query.next())
            potentialPredecessors.append(extractValue<quint64>(query.value(0)));
    }

    if (!potentialPredecessors.isEmpty()) {
         // Don't potentially overflow sqlite max arg limit of 1000, 100 potential predecessors is more than enough
        potentialPredecessors = potentialPredecessors.mid(0, 100);
        quint64 predecessorId(0);
        quint64 messageId(metaData->id().toULongLong());

        if (messageId != 0 && metaData->parentThreadId().isValid()) {
            // We already exist - therefore we must ensure that we do not create a response ID cycle
            QMap<quint64, quint64> predecessor;

            {

                // Find the predecessor message for every message in the same thread as us or the thread of any potential predecessor of us
                QVariantList vl;
                vl << messageId;
                foreach(quint64 p, potentialPredecessors) {
                    vl << p;
                }
                QSqlQuery query(simpleQuery(QString("SELECT id,responseid FROM mailmessages WHERE parentaccountid = %1 AND parentthreadid IN (SELECT parentthreadid FROM mailmessages WHERE id IN %2)").arg(metaData->parentAccountId().toULongLong()).arg(expandValueList(vl)),
                                            vl,
                                            "identifyAncestors mailmessages query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;

                while (query.next())
                    predecessor.insert(extractValue<quint64>(query.value(0)), extractValue<quint64>(query.value(1)));
            }

            // Choose the best predecessor, ensuring that we don't pick a message whose own ancestors include us
            while (!potentialPredecessors.isEmpty()) {
                quint64 ancestorId = potentialPredecessors.first();

                bool descendant(false);
                while (ancestorId) {
                    if (ancestorId == messageId) {
                        // This message is a descendant of ourself
                        descendant = true;
                        break;
                    } else {
                        ancestorId = predecessor[ancestorId];
                    }
                }

                if (!descendant) {
                    // This message can become our predecessor
                    predecessorId = potentialPredecessors.first();
                    break;
                } else {
                    // Try the next option, if any
                    potentialPredecessors.takeFirst();
                }
            }
        } else {
            // Just take the first selection
            predecessorId = potentialPredecessors.first();
        }

        if (predecessorId) {
            const QMailMessageId predecessorMsgId(predecessorId);
            metaData->setInResponseTo(predecessorMsgId);

            metaData->setParentThreadId(QMailMessageMetaData(predecessorMsgId).parentThreadId());

            if (metaData->responseType() == QMailMessageMetaData::NoResponse)
                metaData->setResponseType(QMailMessageMetaData::UnspecifiedResponse);
        }
    }
    missingReferences->removeDuplicates();

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::identifyAncestors(const QMailMessageId &predecessorId, const QMailMessageIdList &childIds, QMailMessageIdList *ancestorIds)
{
    if (!childIds.isEmpty() && predecessorId.isValid()) {
        QMap<quint64, quint64> predecessor;

        {
            QMailMessageMetaData predecessorMsg(predecessorId);
            // Find the predecessor message for every message in the same thread as the predecessor
            QSqlQuery query(simpleQuery("SELECT id,responseid FROM mailmessages WHERE parentthreadid = ? AND parentaccountid = ?",
                                        QVariantList() << predecessorMsg.parentThreadId().toULongLong() << predecessorMsg.parentAccountId().toULongLong(),
                                        "identifyAncestors mailmessages query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                predecessor.insert(extractValue<quint64>(query.value(0)), extractValue<quint64>(query.value(1)));
        }

        // Ensure that none of the prospective children are predecessors of this message
        quint64 messageId = predecessorId.toULongLong();
        while (messageId) {
            if (childIds.contains(QMailMessageId(messageId))) {
                ancestorIds->append(QMailMessageId(messageId));
            }

            messageId = predecessor[messageId];
            if (ancestorIds->contains(QMailMessageId(messageId))) {
                break;
           }
        }
    }
    
    if (predecessorId.isValid()) {
        ancestorIds->append(predecessorId);
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::resolveMissingMessages(const QString &identifier, const QMailMessageId &predecessorId, const QString &baseSubject, const QMailMessageMetaData &message, QMailMessageIdList *updatedMessageIds)
{
    QMap<QMailMessageId, quint64> descendants;

    if (!identifier.isEmpty()) {
        QSqlQuery query(simpleQuery(QString("SELECT DISTINCT id,level FROM missingmessages WHERE identifier=? AND id IN (SELECT id FROM mailmessages WHERE parentaccountid = %1)").arg(message.parentAccountId().toULongLong()),
                                    QVariantList() << identifier,
                                    "resolveMissingMessages missingmessages query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        while (query.next())
            descendants.insert(QMailMessageId(extractValue<quint64>(query.value(0))), extractValue<quint64>(query.value(1)));
    }

    if (!descendants.isEmpty() && predecessorId.isValid()) {
        QMailMessageIdList ancestorIds;

        // Do not create a cycle - ensure that none of these messages is an ancestor of the new message
        AttemptResult result = identifyAncestors(predecessorId, descendants.keys(), &ancestorIds);
        if (result != Success)
            return result;

        // Ensure that none of the ancestors become descendants of this message
        foreach (const QMailMessageId &id, ancestorIds) {
            descendants.remove(id);
        }
    }
    descendants.remove(QMailMessageId(message.id()));

    if (!descendants.isEmpty()) {
        QVariantList descendantIds;
        QVariantList descendantLevels;

        QMap<QMailMessageId, quint64>::const_iterator it = descendants.begin(), end = descendants.end();
        for ( ; it != end; ++it) {
            Q_ASSERT(it.key() != QMailMessageId());
            updatedMessageIds->append(it.key());

            descendantIds.append(QVariant(it.key().toULongLong()));
            descendantLevels.append(QVariant(it.value()));
        }

        {
            // Update these descendant messages to have the new message as their predecessor
            QSqlQuery query(simpleQuery("UPDATE mailmessages SET responseid=?",
                                        QVariantList() << message.id().toULongLong(),
                                        Key(QMailMessageKey::id(*updatedMessageIds)),
                                        "resolveMissingMessages mailmessages update query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        {
            // Truncate the missingmessages entries for each updated messages
            QSqlQuery query(batchQuery("DELETE FROM missingmessages WHERE id=? AND level>=?",
                                       QVariantList() << QVariant(descendantIds) << QVariant(descendantLevels),
                                       "resolveMissingMessages missingmessages delete query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }

        QVariantList obsoleteThreadIds;

        {
            // Find the threads that the descendants currently belong to
            QString sql("SELECT DISTINCT parentthreadid FROM mailmessages WHERE id IN %1 AND parentthreadid != ?");
            QVariantList bindValues;
            foreach (const QMailMessageId& id, *updatedMessageIds) {
                bindValues << id.toULongLong();
            }
            QSqlQuery query(simpleQuery(sql.arg(expandValueList(bindValues)),
                                        QVariantList() << bindValues << message.parentThreadId().toULongLong(),
                                        "resolveMissingMessages mailmessages query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                obsoleteThreadIds.append(QVariant(extractValue<quint64>(query.value(0))));
        }

        if (!obsoleteThreadIds.isEmpty()) {
            {
                // Attach the descendants to the thread of their new predecessor
                QString sql("UPDATE mailmessages SET parentthreadid=(SELECT parentthreadid FROM mailmessages WHERE id=%1) "
                            "WHERE parentthreadid IN %2");
                QSqlQuery query(simpleQuery(sql.arg(message.id().toULongLong()).arg(expandValueList(obsoleteThreadIds)),
                                 QVariantList() << obsoleteThreadIds,
                                             "resolveMissingMessages mailmessages update query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            {
                // TODO: fix other columns as well if necessary
                QString sql("UPDATE mailthreads "
                            "SET messagecount = messagecount + (SELECT SUM(messagecount) FROM mailthreads WHERE id IN %1), "
                            "unreadcount = unreadcount + (SELECT SUM(unreadcount) FROM mailthreads WHERE id IN %2) "
                            "WHERE id = (?)");
                QSqlQuery query(simpleQuery(sql.arg(expandValueList(obsoleteThreadIds)).arg(expandValueList(obsoleteThreadIds)),
                                            QVariantList() << obsoleteThreadIds << obsoleteThreadIds << message.parentThreadId().toULongLong(),
                                            "resolveMissingMessages mailthreads update query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            {
            // Delete the obsolete threads
            QString sql("DELETE FROM mailthreads WHERE id IN %1");
            QSqlQuery query(simpleQuery(sql.arg(expandValueList(obsoleteThreadIds)),
                                        obsoleteThreadIds,
                                        "resolveMissingMessages mailthreads delete query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
            }
        }
    }

    if (!baseSubject.isEmpty()) {
        QMailMessageIdList ids;

        {
            // See if there are any messages waiting for a thread ancestor message with this subject
            // (or who have one that is older than this message)
            QSqlQuery query(simpleQuery("SELECT id FROM mailmessages mm "
                                        "WHERE id IN ("
                                            "SELECT messageid FROM missingancestors WHERE subjectid=(SELECT id FROM mailsubjects WHERE basesubject=?) "
                                        ") AND "
                                            "stamp > (SELECT stamp FROM mailmessages WHERE id=?) "
                                        "AND ("
                                            "mm.responseid=0 "
                                        "OR "
                                            "(SELECT stamp FROM mailmessages WHERE id=?) > (SELECT stamp FROM mailmessages WHERE id=mm.responseid)"
                                        ")",
                                        QVariantList() << baseSubject << message.id().toULongLong() << message.id().toULongLong(),
                                        "resolveMissingMessages missingancestors query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            while (query.next())
                ids.append(QMailMessageId(extractValue<quint64>(query.value(0))));
        }

        if (!ids.isEmpty() && predecessorId.isValid()) {
            QMailMessageIdList ancestorIds;

            // Do not create a cycle - ensure that none of these messages is an ancestor of the new message
            AttemptResult result = identifyAncestors(predecessorId, ids, &ancestorIds);
            if (result != Success)
                return result;

            // Ensure that none of the ancestors become descendants of this message
            foreach (const QMailMessageId &id, ancestorIds) {
                ids.removeAll(id);
            }
        }
        ids.removeAll(QMailMessageId(message.id()));

        if (!ids.isEmpty()) {
            {
                // Update these descendant messages to have the new message as their predecessor
                QSqlQuery query(simpleQuery("UPDATE mailmessages SET responseid=?",
                                            QVariantList() << message.id().toULongLong(),
                                            Key(QMailMessageKey::id(ids)),
                                            "resolveMissingMessages mailmessages update root query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            {
                // Remove the missing ancestor records
                QSqlQuery query(simpleQuery("UPDATE missingancestors SET state=1",
                                            Key("messageid", QMailMessageKey::id(ids)),
                                            "resolveMissingMessages missingancestors delete query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return DatabaseFailure;
            }

            *updatedMessageIds += ids;
        }
    }

    return Success;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::registerSubject(const QString &baseSubject, quint64 messageId, const QMailMessageId &predecessorId, bool missingAncestor)
{
    int subjectId = 0;

    {
        QSqlQuery query(simpleQuery("SELECT id FROM mailsubjects WHERE basesubject=?",
                                    QVariantList() << baseSubject,
                                    "registerSubject mailsubjects query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.next())
            subjectId = extractValue<quint64>(query.value(0));
    }
    
    if (subjectId == 0) {
        QSqlQuery query(simpleQuery("INSERT INTO mailsubjects (basesubject) VALUES (?)",
                                    QVariantList() << baseSubject,
                                    "registerSubject mailsubjects insert query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        // Retrieve the insert id
        subjectId = extractValue<quint64>(query.lastInsertId());
    }

    // Ensure that this thread is linked to the base subject of this message
    int count = 0;
    {
        QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM mailthreadsubjects "
                                    "WHERE subjectid=? AND threadid = (SELECT parentthreadid FROM mailmessages WHERE id=?)",
                                    QVariantList() << subjectId << messageId,
                                    "registerSubject mailthreadsubjects query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;

        if (query.next())
            count = extractValue<int>(query.value(0));
    }
    
    if (count == 0) {
        QSqlQuery query(simpleQuery("INSERT INTO mailthreadsubjects (threadid,subjectid) SELECT parentthreadid,? FROM mailmessages WHERE id=?",
                                    QVariantList() << subjectId << messageId,
                                    "registerSubject mailthreadsubjects insert query"));
        if (query.lastError().type() != QSqlError::NoError)
            return DatabaseFailure;
    }

    if (missingAncestor) {
        count = 0;

        {
            // We need to record that this message's ancestor is currently missing
            QSqlQuery query(simpleQuery("SELECT COUNT(*) FROM missingancestors WHERE messageid=?",
                                        QVariantList() << messageId,
                                        "registerSubject missingancestors query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;

            if (query.next())
                count = extractValue<int>(query.value(0));
        }

        if (count == 0) {
            quint64 state(predecessorId.isValid() ? 1 : 0);
            QSqlQuery query(simpleQuery("INSERT INTO missingancestors (messageid,subjectid,state) VALUES(?,?,?)",
                                        QVariantList() << messageId << subjectId << state,
                                        "registerSubject missingancestors insert query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        } else {
            QSqlQuery query(simpleQuery("UPDATE missingancestors SET subjectid=? WHERE messageid=?",
                                        QVariantList() << subjectId << messageId,
                                        "registerSubject missingancestors update query"));
            if (query.lastError().type() != QSqlError::NoError)
                return DatabaseFailure;
        }
    }

    return Success;
}

bool QMailStorePrivate::checkPreconditions(const QMailFolder& folder, bool update)
{
    //if the parent is valid, check that it exists 
    //if the account is valid, check that is exists 

    if(!update)
    {
        if(folder.id().isValid())
        {
            qWarning() << "Folder exists, use update instead of add.";
            return false;
        }
    }
    else 
    {
        if(!folder.id().isValid())
        {
            qWarning() << "Folder does not exist, use add instead of update.";
            return false;
        }

        if(folder.parentFolderId().isValid() && folder.parentFolderId() == folder.id())
        {
            qWarning() << "A folder cannot be a child to itself";
            return false;
        }
    }

    if(folder.parentFolderId().isValid())
    {
        if(!idExists(folder.parentFolderId(),"mailfolders"))
        {
            qWarning() << "Parent folder does not exist!";
            return false;
        }
    }

    if(folder.parentAccountId().isValid())
    {
#ifdef USE_ACCOUNTS_QT

        Accounts::AccountId accountId = folder.parentAccountId().toULongLong();
        QSharedPointer<Accounts::Account> ssoAccount = getEmailAccount(accountId);

        if (!ssoAccount) {
          qWarning() << "Parent account does not exist!";
          return false;
        }

        if (!ssoAccount->supportsService("e-mail")) {
          qMailLog(Messaging) << "Parent account does not support e-mail service!";
          return false;
        }
#else
        if(!idExists(folder.parentAccountId(),"mailaccounts")) {
            qWarning() << "Parent account does not exist!";
            return false;
        }
#endif
    }

    return true;
}

bool QMailStorePrivate::recalculateThreadsColumns(const QMailThreadIdList& modifiedThreads, QMailThreadIdList& deletedThreads)
{
    QMap<QMailThreadId, QMailMessageMetaDataList> existedMessagesMap;
    {
        QVariantList bindValues;
        QVariantList bindValuesBatch;
        foreach (const QMailThreadId& threadId, modifiedThreads)
        {
            bindValues << threadId.toULongLong();
        }

        while (!bindValues.isEmpty()) {
            bindValuesBatch = bindValues.mid(0, 500);
            if (bindValues.count() > 500) {
                bindValues = bindValues.mid(500);
            } else {
                bindValues.clear();
            }
            QString sql;
                sql = QString("SELECT id, parentfolderid, sender, subject, status, preview, parentthreadid  FROM mailmessages WHERE parentthreadid IN %1 ORDER BY stamp").arg(expandValueList(bindValuesBatch));
            QSqlQuery query(simpleQuery(sql, bindValuesBatch,
                                "recalculateThreadsColumns select messages info query"));
            if (query.lastError().type() != QSqlError::NoError)
                return false;

            while (query.next()) {
                QMailMessageMetaData data(QMailMessageId(extractValue<quint64>(query.value(0))));
                data.setParentFolderId(QMailFolderId(extractValue<quint64>(query.value(1))));
                data.setFrom(QMailAddress(extractValue<QString>(query.value(2))));
                data.setSubject(extractValue<QString>(query.value(3)));
                data.setStatus(extractValue<quint64>(query.value(4)));
                data.setPreview(extractValue<QString>(query.value(5)));
                QMailThreadId tId(extractValue<quint64>(query.value(6)));
                existedMessagesMap[tId].append(data);
            }
        }
    }

    foreach (const QMailThreadId& threadId, modifiedThreads)
    {
        // if all thread's messages were deleted we should delete thread as well.
        if (!existedMessagesMap.keys().contains(threadId)) {
            deletedThreads.append(threadId);
            continue;
        }

        // it's easier to recalculate and reset all additable thread's columns then to find out what column was changed by
        // this message(s) deletion
        QMailThread thread(threadId);
        uint messagesCount = 0;
        uint unreadCount = 0;
        uint firstUnread = 0;
        uint firstMessageIndex = 0;
        uint lastMessageIndex = 0;
        uint index = 0;
        quint64 status = 0;
        QStringList senders;
        const QMailMessageMetaDataList &threadsMessagesList = existedMessagesMap.value(threadId);
        foreach (const QMailMessageMetaData& data, threadsMessagesList)
        {
            // Messages moved to Draft or Trash folder should not being counted.
            QMailAccount account(data.parentAccountId());
            QMailFolderId trashFolderId = account.standardFolder(QMailFolder::TrashFolder);
            QMailFolderId draftFolderId = account.standardFolder(QMailFolder::DraftsFolder);
            const bool& trashOrDraftMessage = (((data.status() & (QMailMessage::Trash | QMailMessage::Draft)) != 0) ||
                                               (data.parentFolderId() == trashFolderId) || (data.parentFolderId() == draftFolderId));
            if (!trashOrDraftMessage) {
                status |= data.status();
                if (!senders.contains(data.from().toString()))
                    senders.append(data.from().toString());
                lastMessageIndex = index;
                if (messagesCount == 0) firstMessageIndex = index;
                messagesCount++;
                if ((data.status() & QMailMessage::Read) == 0) {
                    if (unreadCount == 0) firstUnread = messagesCount - 1;
                    ++unreadCount;
                }
            }
            index++;
        }

        // messages are sorted by time stamp, so we can set preview, lastDate, subject and startedDate easily by taking them from last and first message in the list
        QMailMessageMetaData firstMessage(threadsMessagesList.at(firstMessageIndex));
        QMailMessageMetaData lastMessage(threadsMessagesList.at(lastMessageIndex));
        thread.setLastDate(QMailTimeStamp(lastMessage.date().toUTC()));
        thread.setPreview(lastMessage.preview());
        thread.setStartedDate(QMailTimeStamp(firstMessage.date().toUTC()));
        thread.setSubject(firstMessage.subject());
        thread.setMessageCount(messagesCount);
        thread.setUnreadCount(unreadCount);
        thread.setStatus(status);
        thread.setSenders(QMailAddress::fromStringList(senders));
        QMailThreadKey::Properties props(QMailThreadKey::MessageCount | QMailThreadKey::UnreadCount |
                                         QMailThreadKey::LastDate | QMailThreadKey::StartedDate |
                                         QMailThreadKey::Preview | QMailThreadKey::Subject |
                                         QMailThreadKey::Status | QMailThreadKey::Senders);

        QSqlQuery query(simpleQuery(QString("UPDATE mailthreads SET %1 WHERE id=?").arg(expandProperties(props, true)),
                                    QVariantList() << threadValues(props, thread) << threadId.toULongLong(),
                                    "deleteMessages mailthreads update query"));

        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    // remove all empty threads
    if (!deletedThreads.isEmpty()) {
        AttemptResult res = updateThreadsValues(deletedThreads);
        if (res != Success)
            return false;
    }
    return true;
}

bool QMailStorePrivate::deleteMessages(const QMailMessageKey& key,
                                       QMailStore::MessageRemovalOption option,
                                       QMailMessageIdList& outDeletedMessageIds,
                                       QMailThreadIdList& deletedThreadIds,
                                       QStringList& expiredContent,
                                       QMailMessageIdList& updatedMessageIds,
                                       QMailFolderIdList& modifiedFolderIds,
                                       QMailThreadIdList& modifiedThreadIds,
                                       QMailAccountIdList& modifiedAccountIds)
{
    QMailMessageIdList deletedMessageIds;
    
    QString elements("id,mailfile,parentaccountid,parentfolderid,parentthreadid");
    if (option == QMailStore::CreateRemovalRecord)
        elements += ",serveruid";

    QVariantList removalAccountIds;
    QVariantList removalServerUids;
    QVariantList removalFolderIds;

    {
        // Get the information we need to delete these messages
        QSqlQuery query(simpleQuery(QString("SELECT %1 FROM mailmessages").arg(elements),
                                    Key(key),
                                    "deleteMessages info query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        bool noMessages = true;
        while (query.next()) {
            QMailMessageId messageId(extractValue<quint64>(query.value(0)));
 
            // Deletion handling logic for this message has already been executed in this transaction
            if (outDeletedMessageIds.contains(messageId))
                continue;
            
            noMessages = false;
            
            deletedMessageIds.append(messageId);
            outDeletedMessageIds.append(messageId);
            
            QString contentUri(extractValue<QString>(query.value(1)));
            if (!contentUri.isEmpty())
                expiredContent.append(contentUri);

            QMailAccountId parentAccountId(extractValue<quint64>(query.value(2)));
            if (parentAccountId.isValid() && !modifiedAccountIds.contains(parentAccountId))
                modifiedAccountIds.append(parentAccountId);

            QMailFolderId folderId(extractValue<quint64>(query.value(3)));
            if (folderId.isValid() && !modifiedFolderIds.contains(folderId))
                modifiedFolderIds.append(folderId);

            QMailThreadId threadId(extractValue<quint64>(query.value(4)));
            if (threadId.isValid() && !modifiedThreadIds.contains(threadId))
                modifiedThreadIds.append(threadId);

            if (option == QMailStore::CreateRemovalRecord) {
                // Extract the info needed to create removal records
                removalAccountIds.append(parentAccountId.toULongLong());
                removalServerUids.append(extractValue<QString>(query.value(5)));
                removalFolderIds.append(folderId.toULongLong());
            }
        }
            
        // No messages? Then we're already done
        if (noMessages)
            return true;
    }

    if (!modifiedFolderIds.isEmpty()) {
        // Any ancestor folders of the directly modified folders are indirectly modified
        QSqlQuery query(simpleQuery("SELECT DISTINCT id FROM mailfolderlinks",
                                    Key("descendantid", QMailFolderKey::id(modifiedFolderIds)),
                                    "deleteMessages mailfolderlinks ancestor query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        while (query.next()) {
            QMailFolderId folderId(extractValue<quint64>(query.value(0)));
            if (folderId.isValid()  && !modifiedFolderIds.contains(folderId))
                modifiedFolderIds.append(folderId);
        }
    }

    // Insert the removal records
    if (!removalAccountIds.isEmpty()) {
        // WARNING - QList::operator<<(QList) actually appends the list items to the object,
        // rather than insert the actual list!
        QSqlQuery query(batchQuery("INSERT INTO deletedmessages (parentaccountid,serveruid,parentfolderid) VALUES (?,?,?)",
                                   QVariantList() << QVariant(removalAccountIds)
                                                  << QVariant(removalServerUids)
                                                  << QVariant(removalFolderIds),
                                   "deleteMessages insert removal records query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Delete any custom fields associated with these messages
        QSqlQuery query(simpleQuery("DELETE FROM mailmessagecustom",
                                    Key(QMailMessageKey::id(deletedMessageIds)),
                                    "deleteMessages delete mailmessagecustom query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Delete any identifiers associated with these messages
        QSqlQuery query(simpleQuery("DELETE FROM mailmessageidentifiers",
                                    Key(QMailMessageKey::id(deletedMessageIds)),
                                    "deleteMessages delete mailmessageidentifiers query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Delete any missing message identifiers associated with these messages
        QSqlQuery query(simpleQuery("DELETE FROM missingmessages",
                                    Key(QMailMessageKey::id(deletedMessageIds)),
                                    "deleteMessages delete missingmessages query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Delete any missing ancestor records for these messages
        QSqlQuery query(simpleQuery("DELETE FROM missingancestors",
                                    Key("messageid", QMailMessageKey::id(deletedMessageIds)),
                                    "deleteMessages missing ancestors delete query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        QMap<QMailMessageId, QMailMessageId> update_map;

        {
            // Find any messages that need to updated
            QSqlQuery query(simpleQuery("SELECT id, responseid FROM mailmessages",
                                        Key("responseid", QMailMessageKey::id(deletedMessageIds)),
                                        "deleteMessages mailmessages updated query"));
            if (query.lastError().type() != QSqlError::NoError)
                return false;

            while (query.next()) {
                QMailMessageId from(QMailMessageId(extractValue<quint64>(query.value(0))));
                QMailMessageId to(QMailMessageId(extractValue<quint64>(query.value(1))));

                update_map.insert(from, to);
            }
        }

        QMap<QMailMessageId, QMailMessageId> predecessors;

        // Find the predecessors for any messages we're removing
        QSqlQuery query(simpleQuery("SELECT id,responseid FROM mailmessages",
                                    Key(QMailMessageKey::id(deletedMessageIds)),
                                    "deleteMessages mailmessages predecessor query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        while (query.next())
            predecessors.insert(QMailMessageId(extractValue<quint64>(query.value(0))), QMailMessageId(extractValue<quint64>(query.value(1))));

        {
            QVariantList messageIdList;
            QVariantList newResponseIdList;
            for (QMap<QMailMessageId, QMailMessageId>::iterator it(update_map.begin()) ; it != update_map.end() ; ++it) {
                QMailMessageId to_update(it.key());

                if (!deletedMessageIds.contains(to_update)) {
                    updatedMessageIds.append(to_update);
                    messageIdList.push_back(QVariant(to_update.toULongLong()));

                    QMailMessageId to;

                    QMap<QMailMessageId, QMailMessageId>::iterator toIterator(predecessors.find(it.value()));
                    Q_ASSERT(toIterator != predecessors.end());
                    // This code makes the assumption of noncyclic dependencies
                    do {
                        to = *toIterator;
                        toIterator = predecessors.find(to);
                    } while (toIterator != predecessors.end());

                    newResponseIdList.push_back(to.toULongLong());
                }
            }

            Q_ASSERT(messageIdList.size() == newResponseIdList.size());
            if (messageIdList.size())
            {
                // Link any descendants of the messages to the deleted messages' predecessor
                QSqlQuery query(batchQuery("UPDATE mailmessages SET responseid=? WHERE id=?",
                                           QVariantList() << QVariant(newResponseIdList) << QVariant(messageIdList),
                                           "deleteMessages mailmessages update query"));
                if (query.lastError().type() != QSqlError::NoError)
                    return false;
            }
        }
    }

    {
        // Perform the message deletion
        QSqlQuery query(simpleQuery("DELETE FROM mailmessages",
                                    Key(QMailMessageKey::id(deletedMessageIds)),
                                    "deleteMessages mailmessages delete query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Remove any subjects that are unreferenced after this deletion
        {
            QSqlQuery query(simpleQuery("DELETE FROM mailthreadsubjects WHERE threadid NOT IN (SELECT parentthreadid FROM mailmessages)",
                                        "deleteMessages mailthreadsubjects delete query"));
            if (query.lastError().type() != QSqlError::NoError)
                return false;
        }

        {
            QSqlQuery query(simpleQuery("DELETE FROM mailsubjects WHERE id NOT IN (SELECT subjectid FROM mailthreadsubjects)",
                                        "deleteMessages mailthreadsubjects delete query"));
            if (query.lastError().type() != QSqlError::NoError)
                return false;
        }
    }

        // Update modified threads. Remove empty threads.
        return recalculateThreadsColumns(modifiedThreadIds, deletedThreadIds);
}

bool QMailStorePrivate::deleteThreads(const QMailThreadKey& key,
                   QMailStore::MessageRemovalOption option,
                   QMailThreadIdList& deletedThreadIds,
                   QMailMessageIdList& deletedMessageIds,
                   QStringList& expiredMailfiles,
                   QMailMessageIdList& updatedMessageIds,
                   QMailFolderIdList& modifiedFolderIds,
                   QMailThreadIdList& modifiedThreadIds,
                   QMailAccountIdList& modifiedAccountIds)
{
    QMailThreadIdList threadsToDelete;

    {
        // Get the identifiers for all the threads we're deleting
        QSqlQuery query(simpleQuery("SELECT t0.id FROM mailthreads t0",
                                    Key(key, "t0"),
                                    "deleteThreads info query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        while (query.next()) {
            QMailThreadId thread(extractValue<quint64>(query.value(0)));
            if (thread.isValid())
                threadsToDelete.append(thread);
        }
    }

    if (threadsToDelete.isEmpty())
        return true;

    // Create a key to select messages in the thread to be deleted
    QMailMessageKey messagesKey(QMailMessageKey::parentThreadId(key));

    // Delete all the messages contained by the folders we're deleting
    if (!deleteMessages(messagesKey, option, deletedMessageIds, deletedThreadIds, expiredMailfiles, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds))
        return false;


    {
        // Perform the thread deletion
        QString sql("DELETE FROM mailthreads");
        QSqlQuery query(simpleQuery(sql, Key(QMailThreadKey::id(threadsToDelete)),
                                    "deleteThreads delete mailthreads query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    deletedThreadIds.append(threadsToDelete);

    // Do not report any deleted entities as updated TODO: factor this into deleteMessages
    for (QMailMessageIdList::iterator mit = updatedMessageIds.begin(); mit != updatedMessageIds.end(); ) {
        if (deletedMessageIds.contains(*mit)) {
            mit = updatedMessageIds.erase(mit);
        } else {
            ++mit;
        }
    }

    return true;
}


bool QMailStorePrivate::deleteFolders(const QMailFolderKey& key, 
                                      QMailStore::MessageRemovalOption option,
                                      QMailFolderIdList& deletedFolderIds,
                                      QMailMessageIdList& deletedMessageIds,
                                      QMailThreadIdList& deletedThreadIds,
                                      QStringList& expiredContent,
                                      QMailMessageIdList& updatedMessageIds,
                                      QMailFolderIdList& modifiedFolderIds,
                                      QMailThreadIdList& modifiedThreadIds,
                                      QMailAccountIdList& modifiedAccountIds)
{
    {
        // Get the identifiers for all the folders we're deleting
        QSqlQuery query(simpleQuery("SELECT t0.id FROM mailfolders t0",
                                    Key(key, "t0"),
                                    "deleteFolders info query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        bool noFolders = true;
        while (query.next()) {
            noFolders = false;

            deletedFolderIds.append(QMailFolderId(extractValue<quint64>(query.value(0))));
        }

        // No folders? Then we're already done
        if (noFolders) 
            return true;
    }

    // Create a key to select messages in the folders to be deleted
    QMailMessageKey messagesKey(QMailMessageKey::parentFolderId(key));

    // Delete all the messages contained by the folders we're deleting
    if (!deleteMessages(messagesKey, option, deletedMessageIds, deletedThreadIds, expiredContent, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds))
        return false;

    // Delete any references to these folders in the mailfolderlinks table
    QString statement("DELETE FROM mailfolderlinks WHERE %1 IN ( SELECT t0.id FROM mailfolders t0");
    statement += buildWhereClause(Key(key, "t0")) + " )";

    QVariantList whereValues(::whereClauseValues(key));

    {
        // Delete where target folders are ancestors
        QSqlQuery query(simpleQuery(statement.arg("id"),
                                    whereValues,
                                    "deleteFolders mailfolderlinks ancestor query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Delete where target folders are descendants
        QSqlQuery query(simpleQuery(statement.arg("descendantid"),
                                    whereValues,
                                    "deleteFolders mailfolderlinks descendant query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Delete any custom fields associated with these folders
        QString sql("DELETE FROM mailfoldercustom");
        QSqlQuery query(simpleQuery(sql, Key(QMailFolderKey::id(deletedFolderIds)),
                                    "deleteFolders delete mailfoldercustom query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Perform the folder deletion
        QString sql("DELETE FROM mailfolders");
        QSqlQuery query(simpleQuery(sql, Key(QMailFolderKey::id(deletedFolderIds)),
                                    "deleteFolders delete mailfolders query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    // Do not report any deleted entities as updated
    for (QMailMessageIdList::iterator mit = updatedMessageIds.begin(); mit != updatedMessageIds.end(); ) {
        if (deletedMessageIds.contains(*mit)) {
            mit = updatedMessageIds.erase(mit);
        } else {
            ++mit;
        }
    }

    for (QMailFolderIdList::iterator fit = modifiedFolderIds.begin(); fit != modifiedFolderIds.end(); ) {
        if (deletedFolderIds.contains(*fit)) {
            fit = modifiedFolderIds.erase(fit);
        } else {
            ++fit;
        }
    }

    return true;
}

bool QMailStorePrivate::deleteAccounts(const QMailAccountKey& key,
                                       QMailAccountIdList& deletedAccountIds,
                                       QMailFolderIdList& deletedFolderIds,
                                       QMailThreadIdList& deletedThreadIds,
                                       QMailMessageIdList& deletedMessageIds,
                                       QStringList& expiredContent,
                                       QMailMessageIdList& updatedMessageIds,
                                       QMailFolderIdList& modifiedFolderIds,
                                       QMailThreadIdList& modifiedThreadIds,
                                       QMailAccountIdList& modifiedAccountIds)
{
#ifdef USE_ACCOUNTS_QT
    // Searching across all accounts inside SSO
    deletedAccountIds << searchSSOAccounts(key);

    // No accounts? Then we're already done
    if (deletedAccountIds.isEmpty())
        return true;
#else
    {
        // Get the identifiers for all the accounts we're deleting
        QSqlQuery query(simpleQuery("SELECT t0.id FROM mailaccounts t0",
                                    Key(key, "t0"),
                                    "deleteAccounts info query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;

        bool noAccounts = true;
        while (query.next()) {
            noAccounts = false;
            
            deletedAccountIds.append(QMailAccountId(extractValue<quint64>(query.value(0))));
        }

        // No accounts? Then we're already done
        if (noAccounts)
            return true;
    }
#endif

    // We won't create new message removal records, since there will be no account to link them to
    QMailStore::MessageRemovalOption option(QMailStore::NoRemovalRecord);

    // Delete any messages belonging to these accounts, more efficient to do this first
    // before folders and threads are deleted

    // Create a key to select messages for the accounts to be deleted
    QMailMessageKey messagesKey(QMailMessageKey::parentAccountId(key));

    // Delete all the messages contained by the folders we're deleting
    if (!deleteMessages(messagesKey, option, deletedMessageIds, deletedThreadIds, expiredContent, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds))
        return false;

    // Create a key to select folders from the accounts to be deleted
    QMailFolderKey foldersKey(QMailFolderKey::parentAccountId(key));
    
    // Delete all the folders contained by the accounts we're deleting
    if (!deleteFolders(foldersKey, option, deletedFolderIds, deletedMessageIds, deletedThreadIds, expiredContent, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds))
        return false;

    {
        // Delete the removal records related to these accounts
        QSqlQuery query(simpleQuery("DELETE FROM deletedmessages",
                                    Key("parentaccountid", QMailAccountKey::id(deletedAccountIds)),
                                    "deleteAccounts removal record delete query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Remove any standard folders associated with these accounts
        QSqlQuery query(simpleQuery("DELETE FROM mailaccountfolders",
                                    Key("id", QMailAccountKey::id(deletedAccountIds)),
                                    "deleteAccounts delete mailaccountfolders query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
        // Create a key to select threads for the accounts to be deleted
        QMailThreadKey threadKey(QMailThreadKey::parentAccountId(deletedAccountIds));

        // Delete all threads contained by the account we're deleting
        if (!deleteThreads(threadKey, option, deletedThreadIds, deletedMessageIds, expiredContent, updatedMessageIds, modifiedFolderIds, modifiedThreadIds, modifiedAccountIds))
            return false;
#ifdef USE_ACCOUNTS_QT
    {
        // Remove accounts from SSO
        foreach (const QMailAccountId& accountID, deletedAccountIds) {

            QSharedPointer<Accounts::Account> ssoAccount(Accounts::Account::fromId(manager, accountID.toULongLong(), this));

            if (ssoAccount) {
                ssoAccount->remove();
                if (!ssoAccount->syncAndBlock()) {
                    return false;
                }
            } else {
                SSOHandleError(manager->lastError());
            }
        }
    }
#else
    {
        // Remove any custom fields associated with these accounts
        QSqlQuery query(simpleQuery("DELETE FROM mailaccountcustom",
                                    Key("id", QMailAccountKey::id(deletedAccountIds)),
                                    "deleteAccounts delete mailaccountcustom query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Remove any configuration fields associated with these accounts
        QSqlQuery query(simpleQuery("DELETE FROM mailaccountconfig",
                                    Key("id", QMailAccountKey::id(deletedAccountIds)),
                                    "deleteAccounts delete mailaccountconfig query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }

    {
        // Perform the account deletion
        QSqlQuery query(simpleQuery("DELETE FROM mailaccounts",
                                    Key("id", QMailAccountKey::id(deletedAccountIds)),
                                    "deleteAccounts delete mailaccounts query"));
        if (query.lastError().type() != QSqlError::NoError)
            return false;
    }
#endif
    // Do not report any deleted entities as updated
    for (QMailMessageIdList::iterator mit = updatedMessageIds.begin(); mit != updatedMessageIds.end(); ) {
        if (deletedMessageIds.contains(*mit)) {
            mit = updatedMessageIds.erase(mit);
        } else {
            ++mit;
        }
    }

    for (QMailFolderIdList::iterator fit = modifiedFolderIds.begin(); fit != modifiedFolderIds.end(); ) {
        if (deletedFolderIds.contains(*fit)) {
            fit = modifiedFolderIds.erase(fit);
        } else {
            ++fit;
        }
    }

    for (QMailAccountIdList::iterator ait = modifiedAccountIds.begin(); ait != modifiedAccountIds.end(); ) {
        if (deletedAccountIds.contains(*ait)) {
            ait = modifiedAccountIds.erase(ait);
        } else {
            ++ait;
        }
    }

    return true;
}

QMailStorePrivate::AttemptResult QMailStorePrivate::findPotentialPredecessorsBySubject(QMailMessageMetaData *metaData, const QString& baseSubject, bool *missingAncestor, QList<quint64> &potentialPredecessors)
{
    // This message has a thread ancestor,  but we can only estimate which is the best choice
    *missingAncestor = true;

    // Find the preceding messages of all thread matching this base subject
    QSqlQuery query(simpleQuery("SELECT id FROM mailmessages "
                                "WHERE id!=? "
                                "AND parentaccountid=? "
                                "AND stamp<? "
                                "AND parentthreadid IN ("
                                        "SELECT threadid FROM mailthreadsubjects WHERE subjectid = ("
                                            "SELECT id FROM mailsubjects WHERE basesubject=?"
                                    ")"
                                 ")"
                                "ORDER BY stamp DESC",
                                QVariantList() << metaData->id().toULongLong()
                                                << metaData->parentAccountId().toULongLong()
                                                << metaData->date().toLocalTime()
                                                << baseSubject,
                                "messagePredecessor mailmessages select query"));
    if (query.lastError().type() != QSqlError::NoError)
        return DatabaseFailure;

    while (query.next()) {
        potentialPredecessors.append(extractValue<quint64>(query.value(0)));
    }
    return Success;
}

bool QMailStorePrivate::obsoleteContent(const QString& identifier)
{
    QSqlQuery query(simpleQuery("INSERT INTO obsoletefiles (mailfile) VALUES (?)",
                                QVariantList() << QVariant(identifier),
                                "obsoleteContent files insert query"));
    if (query.lastError().type() != QSqlError::NoError) {
        qWarning() << "Unable to record obsolete content:" << identifier;
        return false;
    }

    return true;
}

QSqlQuery QMailStorePrivate::simpleQuery(const QString& statement, const QString& descriptor)
{
    return performQuery(statement, false, QVariantList(), QList<Key>(), qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::simpleQuery(const QString& statement, const QVariantList& bindValues, const QString& descriptor)
{
    return performQuery(statement, false, bindValues, QList<Key>(), qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::simpleQuery(const QString& statement, const Key& key, const QString& descriptor)
{
    return performQuery(statement, false, QVariantList(), QList<Key>() << key, qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::simpleQuery(const QString& statement, const QVariantList& bindValues, const Key& key, const QString& descriptor)
{
    return performQuery(statement, false, bindValues, QList<Key>() << key, qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::simpleQuery(const QString& statement, const QVariantList& bindValues, const QList<Key>& keys, const QString& descriptor)
{
    return performQuery(statement, false, bindValues, keys, qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::simpleQuery(const QString& statement, const QVariantList& bindValues, const QList<Key>& keys, const QPair<uint, uint> &constraint, const QString& descriptor)
{
    return performQuery(statement, false, bindValues, keys, constraint, descriptor);
}

QSqlQuery QMailStorePrivate::batchQuery(const QString& statement, const QVariantList& bindValues, const QString& descriptor)
{
    return performQuery(statement, true, bindValues, QList<Key>(), qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::batchQuery(const QString& statement, const QVariantList& bindValues, const Key& key, const QString& descriptor)
{
    return performQuery(statement, true, bindValues, QList<Key>() << key, qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::batchQuery(const QString& statement, const QVariantList& bindValues, const QList<Key>& keys, const QString& descriptor)
{
    return performQuery(statement, true, bindValues, keys, qMakePair(0u, 0u), descriptor);
}

QSqlQuery QMailStorePrivate::performQuery(const QString& statement, bool batch, const QVariantList& bindValues, const QList<Key>& keys, const QPair<uint, uint> &constraint, const QString& descriptor)
{
    QString keyStatements;
    QVariantList keyValues;

    bool firstClause(true);
    foreach (const Key &key, keys) {
        if (key.isType<QMailMessageKey>() || key.isType<QMailFolderKey>() || key.isType<QMailAccountKey>() || key.isType<QMailThreadKey>()) {
            keyStatements.append(buildWhereClause(key, false, firstClause));
            keyValues << whereClauseValues(key);
        } else if (key.isType<QMailMessageSortKey>() || key.isType<QMailFolderSortKey>() || key.isType<QMailAccountSortKey>() || key.isType<QMailThreadSortKey>()) {
            keyStatements.append(buildOrderClause(key));
        } else if (key.isType<QString>()) {
            keyStatements.append(key.key<QString>());
        } else {
            Q_ASSERT(false);
        }

        firstClause = false;
    }

    QString constraintStatements;
    if ((constraint.first > 0) || (constraint.second > 0)) {
        if (constraint.first > 0) {
            constraintStatements.append(QString(" LIMIT %1").arg(constraint.first));
        }
        if (constraint.second > 0) {
            constraintStatements.append(QString(" OFFSET %1").arg(constraint.second));
        }
    }

    QSqlQuery query(prepare(statement + keyStatements + constraintStatements));
    if (queryError() != QSqlError::NoError) {
        qWarning() << "Could not prepare query" << descriptor;
    } else {
        foreach (const QVariant& value, bindValues)
            query.addBindValue(value);
        foreach (const QVariant& value, keyValues)
            query.addBindValue(value);

        if (!execute(query, batch)){
            qWarning() << "Could not execute query" << descriptor;
        }
    }

    return query;
}

void QMailStorePrivate::emitIpcNotification(QMailStoreImplementation::AccountUpdateSignal signal, const QMailAccountIdList &ids)
{
    if ((signal == &QMailStore::accountsUpdated) || (signal == &QMailStore::accountsRemoved)) {
        foreach (const QMailAccountId &id, ids)
            accountCache.remove(id);
    }

    QMailStoreImplementation::emitIpcNotification(signal, ids);
}

void QMailStorePrivate::emitIpcNotification(QMailStoreImplementation::FolderUpdateSignal signal, const QMailFolderIdList &ids)
{
    if ((signal == &QMailStore::foldersUpdated) || (signal == &QMailStore::foldersRemoved)) {
        foreach (const QMailFolderId &id, ids)
            folderCache.remove(id);
    }

    QMailStoreImplementation::emitIpcNotification(signal, ids);
}

void QMailStorePrivate::emitIpcNotification(QMailStoreImplementation::ThreadUpdateSignal signal, const QMailThreadIdList &ids)
{
    if ((signal == &QMailStore::threadsUpdated) || (signal == &QMailStore::threadsRemoved)) {
        foreach (const QMailThreadId &id, ids)
            threadCache.remove(id);
    }

    QMailStoreImplementation::emitIpcNotification(signal, ids);
}

void QMailStorePrivate::emitIpcNotification(QMailStoreImplementation::MessageUpdateSignal signal, const QMailMessageIdList &ids)
{
    Q_ASSERT(!ids.contains(QMailMessageId()));

    if ((signal == &QMailStore::messagesUpdated) || (signal == &QMailStore::messagesRemoved)) {
        foreach (const QMailMessageId &id, ids)
            messageCache.remove(id);
    }

    QMailStoreImplementation::emitIpcNotification(signal, ids);
}

void QMailStorePrivate::emitIpcNotification(QMailStoreImplementation::MessageDataPreCacheSignal signal, const QMailMessageMetaDataList &data)
{
    if(!data.isEmpty()) {

        QMailMessageIdList ids;

        foreach(const QMailMessageMetaData& metaData, data)
        {
            messageCache.insert(metaData);
            uidCache.insert(qMakePair(metaData.parentAccountId(), metaData.serverUid()), metaData.id());

            ids.append(metaData.id());
        }

        QMailStoreImplementation::emitIpcNotification(signal, data);
    }

}

void QMailStorePrivate::emitIpcNotification(const QMailMessageIdList& ids,  const QMailMessageKey::Properties& properties,
                                     const QMailMessageMetaData& data)
{
    Q_ASSERT(!ids.contains(QMailMessageId()));

    foreach(const QMailMessageId& id, ids) {

        if(messageCache.contains(id)) {
            QMailMessageMetaData metaData = messageCache.lookup(id);
            if ((properties & QMailMessageKey::Custom)) {
                metaData.setCustomFields(data.customFields());
            }
            foreach (QMailMessageKey::Property p, messagePropertyList()) {
                switch (properties & p)
                {
                case QMailMessageKey::Id:
                    metaData.setId(data.id());
                    break;

                case QMailMessageKey::Type:
                    metaData.setMessageType(data.messageType());
                    break;

                case QMailMessageKey::ParentFolderId:
                    metaData.setParentFolderId(data.parentFolderId());
                    break;

                case QMailMessageKey::Sender:
                    metaData.setFrom(data.from());
                    break;

                case QMailMessageKey::Recipients:
                    metaData.setRecipients(data.recipients());
                    break;

                case QMailMessageKey::Subject:
                    metaData.setSubject(data.subject());
                    break;

                case QMailMessageKey::TimeStamp:
                    metaData.setDate(data.date());
                    break;

                case QMailMessageKey::ReceptionTimeStamp:
                    metaData.setReceivedDate(data.receivedDate());
                    break;

                case QMailMessageKey::Status:
                    metaData.setStatus(data.status());
                    break;

                case QMailMessageKey::ParentAccountId:
                    metaData.setParentAccountId(data.parentAccountId());
                    break;

                case QMailMessageKey::ServerUid:
                    metaData.setServerUid(data.serverUid());
                    break;

                case QMailMessageKey::Size:
                    metaData.setSize(data.size());
                    break;

                case QMailMessageKey::ContentType:
                    metaData.setContent(data.content());
                    break;

                case QMailMessageKey::PreviousParentFolderId:
                    metaData.setPreviousParentFolderId(data.previousParentFolderId());
                    break;

                case QMailMessageKey::ContentScheme:
                    metaData.setContentScheme(data.contentScheme());
                    break;

                case QMailMessageKey::ContentIdentifier:
                    metaData.setContentIdentifier(data.contentIdentifier());
                    break;

                case QMailMessageKey::InResponseTo:
                    metaData.setInResponseTo(data.inResponseTo());
                    break;

                case QMailMessageKey::ResponseType:
                    metaData.setResponseType(data.responseType());
                    break;

                case QMailMessageKey::ParentThreadId:
                    metaData.setParentThreadId(data.parentThreadId());
                    break;
                }
            }

            if (properties != allMessageProperties()) {
                // This message is not completely loaded
                metaData.setStatus(QMailMessage::UnloadedData, true);
            }

            metaData.setUnmodified();
            messageCache.insert(metaData);

        }
    }

    QMailStoreImplementation::emitIpcNotification(ids, properties, data);
}

void QMailStorePrivate::emitIpcNotification(const QMailMessageIdList& ids, quint64 status, bool set)
{
    Q_ASSERT(!ids.contains(QMailMessageId()));

    foreach(const QMailMessageId& id, ids) {

        if(messageCache.contains(id)) {
            QMailMessageMetaData metaData = messageCache.lookup(id);
            metaData.setStatus(status, set);
            metaData.setUnmodified();
            messageCache.insert(metaData);
        }
    }

    QMailStoreImplementation::emitIpcNotification(ids, status, set);
}

#ifdef USE_ACCOUNTS_QT
QMailAccountIdList QMailStorePrivate::searchSSOAccounts(const QMailAccountKey& key, const QMailAccountSortKey& sortKey) const
{
    Q_UNUSED (sortKey);

    Accounts::AccountIdList accountIDList = manager->accountList("e-mail");

    // Populate all E-Mail accounts
    typedef QList<Accounts::Account*> AccountList;
    QMailAccountIdList accountList;

    foreach (const Accounts::AccountId& accountID, accountIDList) {
        Accounts::Account* ssoAccount = Accounts::Account::fromId(manager, accountID, const_cast<QMailStorePrivate*>(this));
        if (!ssoAccount) {
            SSOHandleError(manager->lastError());
            continue;
        }

        Accounts::ServiceList services = ssoAccount->enabledServices();
        const int &count = services.count();
        switch (count) {
        case 0: // ignore such accounts
            break;
        case 1: {
            Accounts::Service service = services.first();
            ssoAccount->selectService(service);
            if (SSOAccountSatisfyTheKey(ssoAccount, key))
                accountList.append(QMailAccountId(ssoAccount->id()));
            } break;
        default:
            qCritical() << Q_FUNC_INFO << "Account must contain one enabled email service. Got" << count
                        << "for account" << accountID;
            Q_ASSERT (false);
            return QMailAccountIdList();
        }

        delete ssoAccount;
    }

    /*
     * TBD: Use sortKey to sort found accounts properly
     */

    return accountList;
}

void QMailStorePrivate::accountCreated(Accounts::AccountId id)
{
    // ignore non-email accounts
    if (!accountValid(id))
        return;

    QMailAccountIdList ids;
    ids << QMailAccountId(id);
    Q_Q(QMailStore);

    ENFORCE(QMetaObject::invokeMethod(q, "accountsAdded", Qt::QueuedConnection, Q_ARG(QMailAccountIdList, ids)));
}

void QMailStorePrivate::accountRemoved(Accounts::AccountId id)
{
    // ignore non-email accounts
    if (!accountValid(id))
        return;

    const QMailAccountId& qId = QMailAccountId(id);
    ENFORCE(QMetaObject::invokeMethod(this, "onAccountRemovedFinished", Qt::QueuedConnection, Q_ARG(QMailAccountId, qId)));
}

void QMailStorePrivate::onAccountRemovedFinished(const QMailAccountId &id)
{
    Q_Q (QMailStore);
    emit q->accountsRemoved(QMailAccountIdList() << id);
    // remove from cache after the notification, so that it is possible to
    // know details of the removed account
    accountCache.remove(id);
}

void QMailStorePrivate::accountUpdated(Accounts::AccountId id)
{
    if (!accountValid(id))
        return;

    const QMailAccountId& qId = QMailAccountId(id);
    accountCache.remove(qId);
    Q_Q(QMailStore);
    ENFORCE(QMetaObject::invokeMethod(q, "accountsUpdated", Qt::QueuedConnection, Q_ARG(QMailAccountIdList, QMailAccountIdList() << qId)));
}

bool QMailStorePrivate::accountValid(Accounts::AccountId id) const
{
    QSharedPointer<Accounts::Account> account(Accounts::Account::fromId(manager, id, const_cast<QMailStorePrivate*>(this)));

    if (!account) {
        SSOHandleError(manager->lastError());
        return false;
    }

    // Account should already have the type "e-mail",
    // ignore extra checks


    return true;
}

void QMailStorePrivate::disconnectIpc()
{
    QMailStoreImplementation::disconnectIpc();

    ipcLastDbUpdated = QMail::lastDbUpdated();
}

void QMailStorePrivate::reconnectIpc()
{
    QMailStoreImplementation::reconnectIpc();

    // clear cache if needed
    const QDateTime& lastDbUpdated = QMail::lastDbUpdated();
    if (ipcLastDbUpdated != lastDbUpdated) {
        // Clear all caches
        accountCache.clear();
        folderCache.clear();
        messageCache.clear();
        uidCache.clear();
        threadCache.clear();

        ipcLastDbUpdated = lastDbUpdated;
    }
}
#endif
