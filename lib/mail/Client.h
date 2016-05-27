#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "ClientService.h"
#include <qmailstore.h>
#include "Folder.h"

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *service READ service NOTIFY serviceChanged)
    Q_PROPERTY(bool hasConfiguredAccounts READ hasConfiguredAccounts CONSTANT)

public:
    static Client *instance();
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);
    explicit Client(QObject *parent = 0);

    QObject *service() const { return m_service; }
    bool hasConfiguredAccounts();

    // QML API
    Q_INVOKABLE void deleteMessage(const int &msgId);
    Q_INVOKABLE void restoreMessage(const int &msgId);
    Q_INVOKABLE void markMessageImportant(const int &msgId, const bool important);
    Q_INVOKABLE void markMessageRead(const int &msgId, const bool read);
    Q_INVOKABLE void markMessageTodo(const int &msgId, const bool todo);
    Q_INVOKABLE void markMessageDone(const int &msgId, const bool done);
    Q_INVOKABLE void moveToStandardFolder(const int &msgId, const int &standardFolder); // standardFolder is a Folder::FolderType

    // C++ Extras API
    void deleteMessages(const QMailMessageIdList &idList);
    void markMessagesImportant(const QMailMessageIdList &idList, const bool important);
    void markMessagesRead(const QMailMessageIdList &idList, const bool read);
    void markMessagesTodo(const QMailMessageIdList &idList, const bool todo);
    void markMessagesDone(const QMailMessageIdList &idList, const bool done);

    // C++ fetch api
    void downloadMessagePart(const QMailMessagePart *msgPart);
    void downloadMessage(const QMailMessageId &msgId);
    void downloadMessages(const QMailMessageIdList &idList);

    // C++ Store api
    bool addMessage(QMailMessage *msg);
    void removeMessage(const QMailMessageId &id, const QMailStore::MessageRemovalOption &option);
    void moveToStandardFolder(const QMailMessageIdList &msgIds, const Folder::FolderType &folder);
//    void addMessages(const QMailMessageList &msgList);
//    void updateMessages(const QMailMessageList &msgList);

    // C++ Submission api
    void sendMessage(const QMailMessage &msg);

signals:
    void serviceChanged();
    void messagePartNowAvailable(const quint64 &msgId, const QString &partLocation);
    void messagePartFetchFailed(const quint64 &msgId, const QString &partLocation);
    void messagesNowAvailable(const QMailMessageIdList &idList);
    void messageFetchFailed(const QMailMessageIdList &ids);
    void messagesSent(const QMailMessageIdList &ids);
    void messageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error);

public slots:

private:
    ClientService *m_service;
};

#endif // CLIENT_H
