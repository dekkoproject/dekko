/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SETTINGSOBJECTBASE_H
#define SETTINGSOBJECTBASE_H

#include <QObject>
#include <QStringList>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDateTime>
#include <QVariantMap>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QLockFile>
#include "SettingsFileBase.h"

class SettingsFileBase;
/**
 * @brief The SettingsObjectBase class
 *
 * Provides a settings object that supports atomic writes and change notifications across
 * multiple instances for a single process.
 *
 * The first created instance holds the lockfile for this process.
 */
class SettingsObjectBase : public QObject
{
    Q_OBJECT
    /**
     * @brief Raw data object stored in the settings file
     *
     * Individual object properties can be access in QML using dot notation
     *
     * @code
     * import QtQuick 2.4
     * import Dekko.Settings 1.0
     *
     * Item {
     *     property string labelVal: settings.data.label.value
     *
     *     SettingsObject {
     *         id: settings
     *         Component.onCompleted: {
     *             write("label.value", "Hello world!")
     *         }
     *     }
     * }
     * @endcode
     *
     * This means you can bind to properties and get change notifications in QML.
     *
     * @accessors %data(), setData()
     */
    Q_PROPERTY(QJsonObject data READ data WRITE setData NOTIFY dataChanged)
    /**
     * @brief The base path used to resolve QJsonObject returned by data()
     *
     * You can position a SettingsObjectBase instance at a specific point in the json hierachy
     * if you are only interested in accessing or getting change notifications for certain properties.
     *
     * Say we have a JSON object that looks like
     *
     * @code
     * {
     *     "accounts": {
     *         "account1" : {
     *             "id": 0,
     *             "name": "Foo Bar"
     *         },
     *         "account2" : {
     *             "id": 1,
     *             "name": "Bar Foo"
     *         }
     *     }
     * }
     * @endcode
     *
     * By setting the path to "accounts.account2" only the account2 object will be returned by data().
     * So in QML you would write something like settings.data.name and not settings.data.account2.name.
     * The result is you will only get change notifications for changes to account2 :-)
     *
     * @accessors %path(), setPath()
     */
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    /**
     * @brief Is the QJsonObject at path() valid
     * @accessors %isValid()
     */
    Q_PROPERTY(bool isValid READ isValid NOTIFY dataChanged)

public:
    explicit SettingsObjectBase(QObject *parent = 0);

    QJsonObject data() const;
    void setData(const QJsonObject &data);
    QString path() const;
    void setPath(const QString &path);
    bool isValid() const;
    bool hasKey() const { return !m_settingsKey.isEmpty(); }

    Q_INVOKABLE QJsonValue read(const QString &key, const QJsonValue &defaultValue = QJsonValue::Undefined) const;
    template<typename T> T read(const QString &key) const;
    Q_INVOKABLE void write(const QString &key, const QJsonValue &value);
    template<typename T> void write(const QString &key, const T &value);

    QJsonValue read(const char *key, const QJsonValue &defaultValue = QJsonValue::Undefined) const
    {
        return read(QString::fromLatin1(key), defaultValue);
    }
    template<typename T> T read(const char *key) const
    {
        return read<T>(QString::fromLatin1(key));
    }
    void write(const char *key, const QJsonValue &value)
    {
        write(QString::fromLatin1(key), value);
    }
    template<typename T> void write(const char *key, const T &value)
    {
        write<T>(QString::fromLatin1(key), value);
    }

    Q_INVOKABLE void undefine();

signals:
    void pathChanged();
    void dataChanged();
    void modified(const QString &path, const QJsonValue &value);
public slots:
    void modified(const QStringList &path, const QJsonValue &value);

protected:
    virtual void createDefaultsIfNotExist();
    void setSettingsKey(const QString &key);

private:
    void setFile(QSharedPointer<SettingsFileBase> file);
    QSharedPointer<SettingsFileBase> m_file;
    QScopedPointer<QLockFile> *m_lock;
    QStringList m_path;
    QJsonObject m_object;
    bool m_invalid;
    QString m_settingsKey;

};
template<typename T> inline void SettingsObjectBase::write(const QString &key, const T &value)
{
    write(key, QJsonValue(value));
}

template<> inline QString SettingsObjectBase::read<QString>(const QString &key) const
{
    return read(key).toString();
}

template<> inline QJsonArray SettingsObjectBase::read<QJsonArray>(const QString &key) const
{
    return read(key).toArray();
}

template<> inline QJsonObject SettingsObjectBase::read<QJsonObject>(const QString &key) const
{
    return read(key).toObject();
}

template<> inline double SettingsObjectBase::read<double>(const QString &key) const
{
    return read(key).toDouble();
}

template<> inline int SettingsObjectBase::read<int>(const QString &key) const
{
    return read(key).toInt();
}

template<> inline bool SettingsObjectBase::read<bool>(const QString &key) const
{
    return read(key).toBool();
}

template<> inline QDateTime SettingsObjectBase::read<QDateTime>(const QString &key) const
{
    QString value = read(key).toString();
    if (value.isEmpty())
        return QDateTime();
    return QDateTime::fromString(value, Qt::ISODate).toLocalTime();
}

template<> inline void SettingsObjectBase::write<QDateTime>(const QString &key, const QDateTime &value)
{
    write(key, QJsonValue(value.toUTC().toString(Qt::ISODate)));
}

#endif // SETTINGSOBJECTBASE_H
