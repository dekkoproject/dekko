#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QmlObjectListModel.h>
#include <qmailmessage.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Attachment;

class Attachments : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
public:
    explicit Attachments(QObject *parent = 0);
    QObject *model() { return m_model; }

    void setMessageId(const QMailMessageId &id);
signals:
    void modelChanged();
private:
    void initModel();
    QMailMessageId m_id;
    QQmlObjectListModel<Attachment> *m_model;
};

class Attachment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayName READ displayName NOTIFY attachmentChanged)
    Q_PROPERTY(QString mimeType READ mimeType NOTIFY attachmentChanged)
    Q_PROPERTY(QString url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString size READ size NOTIFY attachmentChanged)
    Q_PROPERTY(int sizeInBytes READ sizeInBytes CONSTANT)
    Q_PROPERTY(bool contentAvailable READ contentAvailable NOTIFY attachmentChanged)
    Q_PROPERTY(QString location READ location NOTIFY attachmentChanged)
    Q_PROPERTY(Type type READ type NOTIFY attachmentChanged)
    Q_PROPERTY(bool fetchInProgress READ fetchInProgress NOTIFY progressChanged)
    Q_PROPERTY(QString mimeTypeIcon READ mimeTypeIcon NOTIFY attachmentChanged)

    Q_ENUMS(Type)

public:
    explicit Attachment(QObject *parent = 0);

    enum Type {
        Text,
        Image,
        Media,
        Multipart,
        Other
    };

    QString displayName();
    QString mimeType() const;
    QString size();
    int sizeInBytes() const;
    Type type() const;
    bool contentAvailable() const;
    QString location() const;
    QString url() const;
    bool fetchInProgress() const;
    QString mimeTypeIcon() const;

    void init(const QMailMessageId &id, const QMailMessagePartContainer::Location &location);

signals:
    void attachmentChanged();
    void readyToOpen(QString url);
    void progressChanged();
    void urlChanged();

public slots:
    // This is a quick hack passing in a qml object to avoid a circular dependency.
    // ideally we should split the Client* classes into another lib
    // and avoid a circular dependency properly in the long term.
    void open(QObject *qmlObject);

private slots:
    void fetch();
    void handlePartFetched();

private:
    QString writePartToFile();
    QUrl partFetchUrl();
    QUrl saveToAccessibleFile();
    QString sizeToReadableString(const int &size);
    bool isRfc822();

    QMailMessageId m_id;
    QMailMessagePartContainer::Location m_location;
    Type m_type;
    QMailMessagePart m_part;
    QString m_url;
    bool m_fetching;
    QNetworkAccessManager *m_qnam;
    QNetworkReply *m_reply;
};

#endif // ATTACHMENTS_H
