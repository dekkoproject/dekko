#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QmlObjectListModel.h>
#include <qmailserviceaction.h>
#include "ClientServiceAction.h"

class ClientService : public QObject
{
    Q_OBJECT
    /** @short list model of recent actions that are undoable */
    Q_PROPERTY(QObject *undoActionQueue READ undoQueue NOTIFY queueChanged)
    Q_PROPERTY(bool hasUndoableAction READ hasUndoableAction NOTIFY undoCountChanged)
    Q_PROPERTY(QString undoableActionDescription READ undoDescription NOTIFY undoCountChanged)

    /** @short model of queued service actions */
    Q_PROPERTY(QObject *serviceActionQueue READ serviceQueue NOTIFY queueChanged)

public:
    explicit ClientService(QObject *parent = 0);
    QObject *undoQueue() { return m_undoQueue; }
    QObject *serviceQueue() { return m_serviceQueue; }
    bool hasUndoableAction();
    QString undoDescription();
    void deleteMessages(const QMailMessageIdList &ids);
    void restoreMessage(const QMailMessageId &id);

signals:
    void undoCountChanged();
    void updatesRolledBack();
    void queueChanged();

public slots:
    void undoActions();


private slots:
    /** @short Process the next action in the action queue */
    void processNextServiceAction();
    /** @short Get's called on ClientServiceAction::complete() */
    void exportMailStoreUpdate();
    void exportMailStoreUpdate(const QMailAccountId &id);

    void undoableCountChanged();
    void rollBackMailStoreUpdates(const QMailAccountIdList &accounts);

    void activityChanged(QMailServiceAction::Activity activity);

private:
    void connectServiceAction(QMailServiceAction* action);

private:
    // We use a qmlobjectlistmodel here instead of a simple qlist
    // then we can have a nice little queued actions widget using the model
    QQmlObjectListModel<ClientServiceAction> *m_undoQueue;
    QQmlObjectListModel<ClientServiceAction> *m_serviceQueue;
    QTimer *m_undoTimer;

    QMailRetrievalAction *m_exportAction;
};

#endif // CLIENTSERVICE_H
