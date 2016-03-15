#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QUrl>
#include <qmailmessage.h>
#include <QmlObjectListModel.h>
#include "MailAddress.h"

/** @short MinimalMessage provides just the required info for display in the message list

    There isn't need to create the full Message here but instead return it as a property
    of this minimal property set.
*/
class MinimalMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int messageId READ messageId WRITE setMessageId NOTIFY minMessageChanged)
    Q_PROPERTY(MailAddress *from READ from NOTIFY minMessageChanged)
    Q_PROPERTY(QString subject READ subject NOTIFY minMessageChanged)
    Q_PROPERTY(QString preview READ preview NOTIFY minMessageChanged)
    Q_PROPERTY(bool hasAttachments READ hasAttachments NOTIFY minMessageChanged)
    Q_PROPERTY(bool isRead READ isRead NOTIFY minMessageChanged)
    Q_PROPERTY(bool isImportant READ isFlagged NOTIFY minMessageChanged)
    Q_PROPERTY(QDateTime date READ date NOTIFY minMessageChanged)
    Q_PROPERTY(QString prettyDate READ prettyDate NOTIFY minMessageChanged)

public:
    explicit MinimalMessage(QObject *parent = 0);
    int messageId() const { return m_id.toULongLong(); }
    MailAddress *from() const;
    QString subject() const;
    QString preview() const;
    bool hasAttachments() const;
    bool isRead() const;
    bool isFlagged() const;
    QDateTime date() const;
    QString prettyDate();
signals:
    void minMessageChanged();

public slots:
    void setMessageId(const quint64 &id);
    void setMessageId(const QMailMessageId &id);
    void emitMinMessageChanged() { emit minMessageChanged(); }

protected:
    QMailMessageId m_id;

private:
    MailAddress *m_from;
};

class Message : public MinimalMessage // Extend on what we already have above
{
    Q_OBJECT
    Q_PROPERTY(QObject *to READ to NOTIFY messageChanged)
    Q_PROPERTY(QObject *cc READ cc NOTIFY messageChanged)
    Q_PROPERTY(QObject *bcc READ bcc NOTIFY messageChanged)
public:
    explicit Message(QObject *parent = 0);
    QObject *to() const { return m_to; }
    QObject *cc() const { return m_cc; }
    QObject *bcc() const { return m_bcc; }

signals:
    void messageChanged();

private:
    QQmlObjectListModel<MailAddress> *m_to;
    QQmlObjectListModel<MailAddress> *m_cc;
    QQmlObjectListModel<MailAddress> *m_bcc;
};

#endif // MESSAGE_H
