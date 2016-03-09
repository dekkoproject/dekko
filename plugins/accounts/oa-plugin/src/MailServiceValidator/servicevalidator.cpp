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
#include "servicevalidator.h"
#include "taskfactory.h"
#include <QDebug>

ServiceValidator::ServiceValidator(QObject *parent) : QObject(parent),
    m_accountId(0),
    m_status(Status::INVALID),
    m_hasSmtp(false)
{
}

quint32 ServiceValidator::accountId() const
{
    return m_accountId;
}

ServiceValidator::Status ServiceValidator::status() const
{
    return m_status;
}

void ServiceValidator::setAccountId(const quint32 &id)
{
    if (id == 0) {
        // Let a call to validate() emit the invalidAcountId signal
        return;
    }
    if (id != m_accountId) {
        m_accountId = id;
        emit accountIdChanged();
    }
}

void ServiceValidator::validate(QObject *account, const bool hasSmtp)
{
    if (m_accountId == 0) {
        emit invalidAccountId();
        return;
    }
    m_hasSmtp = hasSmtp;
    m_imapTask = TaskFactory::createTask(this, account, Task::IMAP);
    Q_ASSERT(m_imapTask);
    connect(m_imapTask.data(), &MailServiceTask::result, this, &ServiceValidator::handleResult);
    connect(m_imapTask.data(), &MailServiceTask::verifyCertificate, this, &ServiceValidator::verifyCertificate);
    connect(this, SIGNAL(run()), m_imapTask.data(), SLOT(run()));
    if (m_hasSmtp) {
        m_smtpTask = TaskFactory::createTask(this, account, Task::SMTP);
        Q_ASSERT(m_smtpTask);
        connect(this, SIGNAL(run()), m_smtpTask.data(), SLOT(run()));
        connect(m_smtpTask.data(), &MailServiceTask::result, this, &ServiceValidator::handleResult);
        connect(m_smtpTask.data(), &MailServiceTask::verifyCertificate, this, &ServiceValidator::verifyCertificate);
    }
    setStatus(Status::RUNNING);
}

void ServiceValidator::handleResult(const QVariantMap &result)
{
    qDebug() << "Task Result: " << result;
    if (result.value("protocol").toString() == "imap") {
        m_imapResult = result;
    } else if (result.value("protocol").toString() == "smtp") {
        m_smtpResult = result;
    } else if (!result.value("success").toBool()) {
        // This is probably a socket or signon error
        emit failed(result);
        return;
    }
    if ((!m_imapResult.isEmpty() && !m_hasSmtp) || (!m_imapResult.isEmpty() && !m_smtpResult.isEmpty())) {
        bool success = true;
        if (!m_imapResult.value("success").toBool()) {
            success = false;
            emit failed(m_imapResult);
        }
        if (m_hasSmtp && !m_smtpResult.value("success").toBool()) {
            success = false;
            emit failed(m_smtpResult);
        }
        setStatus(Status::FINISHED);
        if (success) {
            emit validated();
        }
    }
}

void ServiceValidator::setStatus(const ServiceValidator::Status &status)
{
    m_status = status;
    if (m_status == Status::RUNNING) {
        emit run();
    }
    emit statusChanged();
}

