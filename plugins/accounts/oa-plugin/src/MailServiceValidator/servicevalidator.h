/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef SERVICEVALIDATOR_H
#define SERVICEVALIDATOR_H

#include <QObject>
#include <QPointer>
#include "mailservicetask.h"

class ServiceValidator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_ENUMS(Status)

public:
    explicit ServiceValidator(QObject *parent = 0);

    enum Status {
        INVALID,
        RUNNING,
        FINISHED
    };

    quint32 accountId() const;
    Status status() const;

signals:
    void accountIdChanged();
    void validated();
    void failed(const QVariantMap &map);
    void invalidAccountId();
    void statusChanged();
    void run();
    void verifyCertificate(const QVariantMap &map);

public slots:
    void setAccountId(const quint32 &id);
    void validate(QObject *account, const bool hasSmtp);
private slots:
    void handleResult(const QVariantMap &result);

private:
    void setStatus(const Status &status);
    quint32 m_accountId;
    Status m_status;
    QVariantMap m_imapResult;
    QVariantMap m_smtpResult;
    QPointer<MailServiceTask> m_imapTask;
    QPointer<MailServiceTask> m_smtpTask;
    bool m_hasSmtp;
};

#endif // SERVICEVALIDATOR_H
