#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QmlObjectListModel.h>
#include "ClientServiceAction.h"

class ClientService : public QObject
{
    Q_OBJECT
    /** @short list model of recent actions that are undoable */
    Q_PROPERTY(QObject *undoActionQueue READ undoQueue NOTIFY queueChanged)

//    Q_PROPERTY(bool hasUndoableAction READ hasUndoableAction NOTIFY undoCountChanged)
//    Q_PROPERTY(QString undoableActionDescription READ undoDescription NOTIFY undoCountChanged)
//    Q_PROPERTY(ClientServiceAction *currentAction READ currentAction NOTIFY currentActionChanged)
public:
    explicit ClientService(QObject *parent = 0);
    QObject *undoQueue() { return m_undoQueue; }

    void deleteMessages(const QMailMessageIdList &ids);

signals:
    void undoCountChanged();
    void updatesRolledBack();
    void queueChanged();

public slots:
    void undoActions();


private slots:
    /** @short Process the next action in the action queue */
    void processNextAction();
    /** @short Get's called on ClientServiceAction::complete() */
    void exportMailStoreUpdate();

    void undoableCountChanged();
    void rollBackMailStoreUpdates(const QMailAccountIdList &accounts);

private:
    // We use a qmlobjectlistmodel here instead of a simple qlist
    // then we can have a nice little queued actions widget using the model
    QQmlObjectListModel<ClientServiceAction> *m_undoQueue;
    QTimer *m_undoTimer;
};

#endif // CLIENTSERVICE_H
