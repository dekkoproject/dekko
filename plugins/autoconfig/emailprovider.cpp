#include "emailprovider.h"
#include <QDomDocument>
#include <QXmlStreamReader>

#define NO_VERSION "0.0"

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
    provider->setData(Xml, xmlData);
    return provider;
}

EmailProvider *EmailProvider::fromJson(const QByteArray &jsonData)
{
    EmailProvider *provider = new EmailProvider();
    if (jsonData.isEmpty()) {
        return Q_NULLPTR;
    }
    provider->setData(Json, jsonData);
    return provider;
}

void EmailProvider::addServerConfig(ServerConfig *cfg)
{
    if (cfg->type() == ServerConfig::INCOMING) {
        m_incoming->append(cfg);
    } else if (cfg->type() == ServerConfig::OUTGOING) {
        m_outgoing->append(cfg);
    }
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

void EmailProvider::parseXmlData()
{
    if (m_format == Xml) {
        QDomDocument doc;
        doc.setContent(m_data);
        QDomNodeList nodeList = doc.elementsByTagName(QStringLiteral("clientConfig"));
        QDomNode node = nodeList.at(0);
        setXmlVersion(node);
        if (m_version == QStringLiteral(NO_VERSION)) {
            // We can't do anything with this
            return;
        }
        QDomElement provider = node.firstChildElement(QStringLiteral("emailProvider"));
        setXmlDomains(provider);
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
        m_domains << providers.at(i).nodeValue();
    }
}

void EmailProvider::setXmlName(const QDomElement &providerNode)
{
    QDomNodeList dnList = providerNode.elementsByTagName(QStringLiteral("displayName"));
    if (!dnList.isEmpty())
        m_displayName = dnList.at(0).nodeValue();
    QDomNodeList snList = providerNode.elementsByTagName(QStringLiteral("displayShortName"));
    if (!snList.isEmpty())
        m_shortName = snList.at(0).nodeValue();
}

void EmailProvider::parseJsonData()
{

}

