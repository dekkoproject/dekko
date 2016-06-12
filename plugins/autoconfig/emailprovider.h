#ifndef EMAILPROVIDER_H
#define EMAILPROVIDER_H

#define QTQMLTRICKS_NO_PREFIX_ON_GETTERS

#include <QObject>
#include <QList>
#include <QDomNode>
#include <QDomElement>
#include <QQmlAutoPropertyHelpers.h>
#include <QmlObjectListModel.h>


class ServerConfig;

class EmailProvider : public QObject
{
    Q_OBJECT
    QML_READONLY_AUTO_PROPERTY(QString, version)
    QML_READONLY_AUTO_PROPERTY(QStringList, domains)
    QML_READONLY_AUTO_PROPERTY(QString, displayName)
    QML_READONLY_AUTO_PROPERTY(QString, shortName)
    QML_READONLY_AUTO_PROPERTY(QQmlObjectListModel<ServerConfig>*, incoming)
    QML_READONLY_AUTO_PROPERTY(QQmlObjectListModel<ServerConfig>*, outgoing)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
//    Q_PROPERTY(bool hasImapConfiguration READ hasImapConfiguration CONSTANT)
//    Q_PROPERTY(bool hasPopConfiguration READ hasPopConfiguration CONSTANT)

public:
    explicit EmailProvider(QObject *parent = 0);
    enum Format {Xml, Json};
    static EmailProvider* fromXml(const QByteArray &xmlData);
    static EmailProvider* fromJson(const QByteArray &jsonData);

    void setData(const Format &format, const QByteArray &data);
    bool isValid();
    Q_INVOKABLE bool hasImapConfiguration();
    Q_INVOKABLE QObject* getFirstImapConfig();
    Q_INVOKABLE bool hasPopConfiguration();
    Q_INVOKABLE QObject* getFirstPopConfig();
    Q_INVOKABLE bool hasSmtpConfiguration();
    Q_INVOKABLE QObject* getFirstSmtpConfig();
private:
    // XML specifics
    void parseXmlData();
    void setXmlVersion(const QDomNode &node);
    void setXmlDomains(const QDomElement &providerNode);
    void setXmlName(const QDomElement &providerNode);
    void setXmlServers(const QDomElement &providerNode);
    // JSON specifics
    void parseJsonData();
    Format m_format;
    QByteArray m_data;
};

class ServerConfig : public QObject
{
    Q_OBJECT
public:
    explicit ServerConfig(QObject *parent = 0);

    enum ServerType { UNKNOWN, POP3, IMAP, SMTP };
    enum SocketType { PLAIN, STARTTLS, SSL };
    enum PlaceHolder { NONE, EMAIL_ADDRESS, EMAIL_LOCAL_PART, EMAIL_DOMAIN, REAL_NAME };
    enum AuthMechanism {
        LOGIN,
        CRAM_MD5,
        NTLM,
        GSSAPI,
        CLIENT_IP,
        INVALID
    };

    void setConfig(const QDomNode &server);

protected:
    ServerType getServerType(const QString &type);
    SocketType getSocketType(const QString &type);
    PlaceHolder getPlaceHolderType(const QString &placeHolder);
    AuthMechanism getAuthMechanism(const QString &authMech);

private:
    Q_ENUMS(ServerType)
    Q_ENUMS(SocketType)
    Q_ENUMS(PlaceHolder)
    Q_ENUMS(AuthMechanism)
    QML_READONLY_AUTO_PROPERTY(ServerType, type)
    QML_READONLY_AUTO_PROPERTY(QString, hostname)
    QML_READONLY_AUTO_PROPERTY(int, port)
    QML_READONLY_AUTO_PROPERTY(SocketType, socket)
    QML_READONLY_AUTO_PROPERTY(PlaceHolder, username)
    QML_READONLY_AUTO_PROPERTY(AuthMechanism, mechanism)
    QML_READONLY_AUTO_PROPERTY(QString, password)
    enum { INCOMING, OUTGOING };
};

#endif // EMAILPROVIDER_H
