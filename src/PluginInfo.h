#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <QObject>
#include <QVariantMap>
#include <QJsonObject>
#include <QDebug>

class PluginInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString pluginId READ pluginId CONSTANT)
    Q_PROPERTY(QString location READ location CONSTANT)
    Q_PROPERTY(QVariantMap documentation READ documentation CONSTANT)
    Q_PROPERTY(QVariantMap settings READ settings CONSTANT)

public:
    explicit PluginInfo(QObject *parent = 0);

    virtual QString pluginId() const = 0;
    virtual QString location() const = 0;
    virtual QString i18n() const = 0;
    virtual QVariantMap documentation() const = 0;
    virtual QVariantMap settings() const { return QVariantMap();}

};

class DekkoPlugin : public PluginInfo
{
    Q_OBJECT
    Q_PROPERTY(QString component READ component CONSTANT)
public:
    explicit DekkoPlugin(QObject *parent = 0);

    static DekkoPlugin* fromSpec(const QString &specPath, QObject *parent = 0);

    QString pluginId() const;
    QString location() const;
    QString component();
    QString i18n() const;
    QVariantMap documentation() const;
    QVariantMap settings() const;

    QString pluginPath() const;

    void setSpec(const QJsonObject &spec);
    void setPluginDirectory(const QString &path);

private:
    QJsonObject m_spec;
    QString m_pluginPath;
};

class ServicePlugin : public PluginInfo
{
    Q_OBJECT
public:
    explicit ServicePlugin(QObject *parent = 0);

    // PluginInfo interface
public:
    virtual QString pluginId() const override {
        qWarning() << "Not implemented here";
        return QString();
    }
    virtual QString location() const override {
        qWarning() << "Not implemented here";
        return QString();
    }
    virtual QString i18n() const override {
        qWarning() << "Not implemented here";
        return QString();
    }
    virtual QVariantMap documentation() const override {
        qWarning() << "Not implemented here";
        return QVariantMap();
    }

    virtual void start() {}
    virtual void stop() {}
};

#endif // PLUGININFO_H
