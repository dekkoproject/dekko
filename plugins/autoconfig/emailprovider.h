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

public:
    explicit EmailProvider(QObject *parent = 0);
    enum Format {Xml, Json};
    static EmailProvider* fromXml(const QByteArray &xmlData);
    static EmailProvider* fromJson(const QByteArray &jsonData);

    void addServerConfig(ServerConfig *cfg);
    void setData(const Format &format, const QByteArray &data);

private:
    void parseXmlData();
    void setXmlVersion(const QDomNode &node);
    void setXmlDomains(const QDomElement &providerNode);
    void setXmlName(const QDomElement &providerNode);

    void parseJsonData();
    Format m_format;
    QByteArray m_data;
};

class ServerConfig : public QObject
{
    Q_OBJECT
public:
    explicit ServerConfig(QObject *parent = 0) : QObject(parent){}

    enum ServerType {
        INCOMING,
        OUTGOING
    };

    enum PlaceHolder {
        EMAIL_ADDRESS,
        EMAIL_LOCAL_PART,
        EMAIL_DOMAIN,
        REAL_NAME
    };

private:
    Q_ENUMS(ServerType)
    QML_READONLY_AUTO_PROPERTY(ServerType, type)

};

#endif // EMAILPROVIDER_H
