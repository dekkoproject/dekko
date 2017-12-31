#include "emailprovider.h"
#include <QDomDocument>
#include <QXmlStreamReader>

#define NO_VERSION "0.0"
#define MOZILLA_VERSION "1.1"
#define DEKKO_VERSION "1.2"

EmailProvider::EmailProvider(QObject *parent) : QObject(parent),
    m_incoming(0), m_outgoing(0), m_format(Xml)
{
    m_incoming = new QQmlObjectListModel<ServerConfig>(this);
    m_outgoing = new QQmlObjectListModel<ServerConfig>(this);
}

EmailProvider *EmailProvider::fromXml(const QByteArray &xmlData)
{
    EmailProvider *provider = new EmailProvider();
    if (xmlData.isEmpty()) {
        return Q_NULLPTR;
    }
    //    qDebug() << "===========PROVIDER INFO=========";
    //    qDebug() << xmlData;
    //    qDebug() << "==============================";
    provider->setData(Xml, xmlData);
    return provider;
}

EmailProvider *EmailProvider::fromJson(const QByteArray &jsonData)
{
    EmailProvider *provider = new EmailProvider();
    if (jsonData.isEmpty()) {
        provider->deleteLater();
        return Q_NULLPTR;
    }
    provider->setData(Json, jsonData);
    return provider;
}

void EmailProvider::setData(const Format &format, const QByteArray &data)
{
    m_data = data;
    m_format = format;
    if (m_format == Xml)
        parseXmlData();
    else
        parseJsonData();
}

bool EmailProvider::isValid()
{
    if (m_version == QStringLiteral(NO_VERSION)) {
        return false;
    }
    return !m_incoming->isEmpty();
}

bool EmailProvider::hasImapConfiguration()
{
    qDebug() << __func__ << m_incoming->size();
    foreach(ServerConfig *config, m_incoming->toList()) {
        qDebug() << config->hostname();
        qDebug() << config->type();
        qDebug() << ServerConfig::ServerType::UNKNOWN;
        if (config->type() == ServerConfig::ServerType::IMAP) {
            qDebug() << "Config is IMAP account";
            return true;
        }
    }
    return false;
}

QObject *EmailProvider::getFirstImapConfig()
{
    foreach(ServerConfig *config, m_incoming->toList()) {
        if (config->type() == ServerConfig::ServerType::IMAP) {
            return config;
        }
    }
    return new QObject();
}

bool EmailProvider::hasPopConfiguration()
{
    qDebug() << __func__;
    foreach(ServerConfig *config, m_incoming->toList()) {
        qDebug() << config->hostname();
        qDebug() << config->type();
        qDebug() << ServerConfig::ServerType::UNKNOWN;
        if (config->type() == ServerConfig::ServerType::POP3) {
            qDebug() << "Config is POP3 account";
            return true;
        }
    }
    return false;
}

QObject *EmailProvider::getFirstPopConfig()
{
    foreach(ServerConfig *config, m_incoming->toList()) {
        if (config->type() == ServerConfig::ServerType::POP3) {
            return config;
        }
    }
    return new QObject();
}

bool EmailProvider::hasSmtpConfiguration()
{
    foreach(ServerConfig *config, m_outgoing->toList()) {
        if (config->type() == ServerConfig::ServerType::SMTP) {
            return true;
        }
    }
    return false;
}

QObject *EmailProvider::getFirstSmtpConfig()
{
    foreach(ServerConfig *config, m_outgoing->toList()) {
        if (config->type() == ServerConfig::ServerType::SMTP) {
            return config;
        }
    }
    return new QObject();
}

void EmailProvider::parseXmlData()
{
    if (m_format == Xml) {
        QDomDocument doc;
        doc.setContent(m_data);
        QDomNodeList nodeList = doc.elementsByTagName(QStringLiteral("clientConfig"));
        if (nodeList.isEmpty()) {
            return;
        }
        QDomNode node = nodeList.at(0);
        setXmlVersion(node);
        if (m_version == QStringLiteral(NO_VERSION)) {
            // We can't do anything with this
            return;
        }
        QDomElement provider = node.firstChildElement(QStringLiteral("emailProvider"));
        setXmlDomains(provider);
        setXmlName(provider);
        setXmlServers(provider);
        if (m_version == QStringLiteral(DEKKO_VERSION)) {
            // TODO: parse <imap> and <identities>
        }
    } else {
        qWarning() << "Cannot parse non Xml data";
    }
}

void EmailProvider::setXmlVersion(const QDomNode &node)
{
    QString nodeVersion = node.attributes().namedItem(QStringLiteral("version")).nodeValue();
    if (nodeVersion.isEmpty()) {
        m_version = QStringLiteral(NO_VERSION);
        return;
    }
    m_version = nodeVersion;
}

void EmailProvider::setXmlDomains(const QDomElement &providerNode)
{
    QDomNodeList providers = providerNode.elementsByTagName(QStringLiteral("domain"));
    for (int i = 0; i < providers.count(); ++i) {
        m_domains << providers.at(i).toElement().text();
    }
}

void EmailProvider::setXmlName(const QDomElement &providerNode)
{
    QDomNodeList dnList = providerNode.elementsByTagName(QStringLiteral("displayName"));
    if (!dnList.isEmpty())
        m_displayName = dnList.at(0).toElement().text();;
    QDomNodeList snList = providerNode.elementsByTagName(QStringLiteral("displayShortName"));
    if (!snList.isEmpty())
        m_shortName = snList.at(0).toElement().text();;
}

void EmailProvider::setXmlServers(const QDomElement &providerNode)
{
    auto appendServersToModel = [](QQmlObjectListModel<ServerConfig> *model, const QDomNodeList &servers) {
        for (int i = 0; i < servers.count(); ++i) {
            ServerConfig *c = new ServerConfig();
            c->setConfig(servers.at(i));
            model->append(c);
        }
    };
    appendServersToModel(m_incoming, providerNode.elementsByTagName(QStringLiteral("incomingServer")));
    appendServersToModel(m_outgoing, providerNode.elementsByTagName(QStringLiteral("outgoingServer")));
}

void EmailProvider::parseJsonData(){}

ServerConfig::ServerConfig(QObject *parent) : QObject(parent),
    m_type(ServerType::UNKNOWN),
    m_port(0),
    m_socket(SocketType::SSL),
    m_username(PlaceHolder::NONE),
    m_mechanism(AuthMechanism::LOGIN)
{

}

void ServerConfig::setConfig(const QDomNode &server) {
    QString type = server.attributes().namedItem(QStringLiteral("type")).nodeValue();
    m_type = getServerType(type);
    m_hostname = server.firstChildElement(QStringLiteral("hostname")).text();
    m_port = server.firstChildElement(QStringLiteral("port")).text().toInt();
    QString sock = server.firstChildElement(QStringLiteral("socketType")).text();
    m_socket = getSocketType(sock);
    QString ptext = server.firstChildElement(QStringLiteral("username")).text();
    m_username = getPlaceHolderType(ptext);
    QString auth = server.firstChildElement(QStringLiteral("authentication")).text();
    m_mechanism = getAuthMechanism(auth);
    m_password = server.firstChildElement(QStringLiteral("password")).text();
}

ServerConfig::ServerType ServerConfig::getServerType(const QString &type)
{
    if (type == QStringLiteral("imap")) {
        return ServerType::IMAP;
    } else if (type == QStringLiteral("smtp")) {
        return ServerType::SMTP;
    } else if (type == QStringLiteral("pop3")) {
        return ServerType::POP3;
    } else {
        return ServerType::UNKNOWN;
    }
}

ServerConfig::SocketType ServerConfig::getSocketType(const QString &type)
{
    if (type == QStringLiteral("plain")) {
        return SocketType::PLAIN;
    } else if (type == QStringLiteral("STARTTLS")) {
        return SocketType::STARTTLS;
    } else if (type == QStringLiteral("SSL")) {
        return SocketType::SSL;
    } else {
        return SocketType::PLAIN;
    }
}

ServerConfig::PlaceHolder ServerConfig::getPlaceHolderType(const QString &placeHolder)
{
    if (placeHolder == QStringLiteral("%EMAILADDRESS%")) {
        return PlaceHolder::EMAIL_ADDRESS;
    } else if (placeHolder == QStringLiteral("%EMAILLOCALPART%")) {
        return PlaceHolder::EMAIL_LOCAL_PART;
    } else if (placeHolder == QStringLiteral("%EMAILDOMAIN%")) {
        return PlaceHolder::EMAIL_DOMAIN;
    } else if (placeHolder == QStringLiteral("%REALNAME%")) {
        return PlaceHolder::REAL_NAME;
    } else {
        return PlaceHolder::NONE;
    }
}

ServerConfig::AuthMechanism ServerConfig::getAuthMechanism(const QString &authMech)
{
    if (authMech == QStringLiteral("password-cleartext") || authMech == QStringLiteral("plain")) {
        return AuthMechanism::LOGIN;
    } else if (authMech == QStringLiteral("password-encrypted") || authMech == QStringLiteral("secure")) {
        return AuthMechanism::CRAM_MD5;
    } else if (authMech ==  QStringLiteral("NTLM")){
        return AuthMechanism::NTLM;
    } else if (authMech == QStringLiteral("GSSAPI")) {
        return AuthMechanism::GSSAPI;
    } else if (authMech == QStringLiteral("client-IP-address")) {
        return AuthMechanism::CLIENT_IP;
    } else {
        return AuthMechanism::INVALID;
    }
}
