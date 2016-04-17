#include "GlobalSettings.h"
#include <QPointer>
#include <Paths.h>
#include "SettingsFileBase.h"

const QString GlobalSettingsKeys::defaultsCreated = QStringLiteral("defaultscreated");
// Supported styles "default" and "clean"
const QString GlobalSettingsKeys::messageViewStyle = QStringLiteral("messageview.style");
const QString GlobalSettingsKeys::allowRemoteContent = QStringLiteral("messageview.security.allowRemoteContent");
const QString GlobalSettingsKeys::messageViewMonospaceFont = QStringLiteral("messageview.useMonospaceFont");

static QPointer<SettingsFileBase> globalSettingsFile;

GlobalSettings::GlobalSettings(QObject *parent) : SettingsObjectBase(parent)
{
    if (globalSettingsFile.isNull()) {
        QString error;
        QScopedPointer<SettingsFileBase> file(new SettingsFileBase);
        QLockFile *lock = 0;
        QString globalSettingsLock = Paths::configLocationForFile("globalsettings.json.lock");
        if (Paths::checkForStaleLockFile(&lock, globalSettingsLock, error)) {
            qCritical() << error;
            qApp->quit();
        }
        globalSettingsFile = new SettingsFileBase();
        globalSettingsFile->setPath(Paths::configLocationForFile("globalsettings.json"));
    }
    setFilePath(globalSettingsFile.data());
    if (m_file) {
        setPath(QStringLiteral("default"));
        createDefaultsIfNotExist();
    }
}

QString GlobalSettings::keyValue(GlobalSettings::Keys key)
{
    switch(key) {
    case DefaultsCreated:
        return GlobalSettingsKeys::defaultsCreated;
    case MessageViewStyle:
        return GlobalSettingsKeys::messageViewStyle;
    case MessageViewMonospaceFont:
        return GlobalSettingsKeys::messageViewMonospaceFont;
    case AllowRemoteContent:
        return GlobalSettingsKeys::allowRemoteContent;
    }
    Q_UNREACHABLE();
}

QVariant GlobalSettings::get(GlobalSettings::Keys key)
{
    return read(keyValue(key)).toVariant();
}

void GlobalSettings::set(GlobalSettings::Keys key, const QJsonValue &value)
{
    write(keyValue(key), value);
}

void GlobalSettings::createDefaultsIfNotExist()
{
    if (read(keyValue(DefaultsCreated)).toBool()) {
        return;
    }
    write(keyValue(MessageViewStyle), QStringLiteral("default"));
    write(keyValue(AllowRemoteContent), true);
    write(keyValue(MessageViewMonospaceFont), false);
    write(keyValue(DefaultsCreated), true);
}


