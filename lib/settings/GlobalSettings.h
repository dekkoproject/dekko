#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QVariantMap>
#include <QLockFile>
#include <QScopedPointer>
#include "SettingsObjectBase.h"

struct GlobalSettingsKeys {
    static const QString defaultsCreated;
    static const QString messageViewStyle;
};

class GlobalSettings : public SettingsObjectBase
{
    Q_OBJECT
    Q_ENUMS(Keys)
public:
    explicit GlobalSettings(QObject *parent = 0);
    enum Keys {
        DefaultsCreated = 0,
        MessageViewStyle
    };

    QString keyValue(Keys key);
    Q_INVOKABLE QVariant get(Keys key);
    Q_INVOKABLE void set(Keys key, const QJsonValue &value);

private:
    void createDefaultsIfNotExist();

    QScopedPointer<QLockFile> m_lock;
};

#endif // GLOBALSETTINGS_H
