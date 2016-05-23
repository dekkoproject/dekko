#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <QQuickTextDocument>
#include "MailAddress.h"

class MessageBuilder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *to READ to NOTIFY modelsChanged)
    Q_PROPERTY(QObject *cc READ cc NOTIFY modelsChanged)
    Q_PROPERTY(QObject *bcc READ bcc NOTIFY modelsChanged)
    Q_PROPERTY(QQuickTextDocument *subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_ENUMS(RecipientModels)
public:
    explicit MessageBuilder(QObject *parent = 0);

    enum RecipientModels { To, Cc, Bcc };

    QObject *to() const { return m_to; }
    QObject *cc() const { return m_cc; }
    QObject *bcc() const {return m_bcc; }

    QQuickTextDocument *subject() const;

signals:
    void modelsChanged();

    void subjectChanged(QQuickTextDocument *subject);

public slots:
    void addRecipient(const RecipientModels which, const QString &emailAddress);
    void addRecipient(const RecipientModels which, const QString &name, const QString &address);
    void removeRecipient(const RecipientModels which, const int &index);
    void reset();

    void setSubject(QQuickTextDocument * subject);

private:
    QQmlObjectListModel<MailAddress> *m_to;
    QQmlObjectListModel<MailAddress> *m_cc;
    QQmlObjectListModel<MailAddress> *m_bcc;
    QQuickTextDocument *m_subject;
};

#endif // MESSAGEBUILDER_H
