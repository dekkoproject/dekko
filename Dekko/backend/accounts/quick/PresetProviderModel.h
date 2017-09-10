/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

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
#ifndef PRESETPROVIDERMODEL_H
#define PRESETPROVIDERMODEL_H

#include <QObject>
#include <QSettings>
#include <QmlObjectListModel.h>
#include <Helpers.h>

typedef QSettings ProviderFile;

class Provider : public QObject
{
public:
    Provider(QObject *parent, const QString &description, const int &type);
    Provider(QObject *parent, const QString &providerFilePath, const QString &group);

    enum Type {
        INVALID,
        PRESET,
        IMAP,
        POP3,
        SMTP
    };
    QString icon();

private:
    Q_OBJECT
    Q_ENUMS(Type)
    QML_READONLY_PROPERTY(QString, description)
    QML_READONLY_PROPERTY(QString, domain)
    QML_READONLY_PROPERTY(QString, imapHost)
    QML_READONLY_PROPERTY(int, imapPort)
    QML_READONLY_PROPERTY(bool, imapUseSSL)
    QML_READONLY_PROPERTY(bool, imapStartTLS)
    QML_READONLY_PROPERTY(QString, smtpHost)
    QML_READONLY_PROPERTY(int, smtpPort)
    QML_READONLY_PROPERTY(bool, smtpUseSSL)
    QML_READONLY_PROPERTY(bool, smtpStartTLS)
    QML_READONLY_PROPERTY(Type, type)
    Q_PROPERTY(QString icon READ icon CONSTANT)

};

class PresetProviderModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
public:
    explicit PresetProviderModel(QObject *parent = 0);

    QObject *model() const { return m_model; }

signals:
    void modelChanged();
public slots:
    void init();

private:
    QQmlObjectListModel<Provider> *m_model;
    QString m_providers;
};

#endif // PRESETPROVIDERMODEL_H
