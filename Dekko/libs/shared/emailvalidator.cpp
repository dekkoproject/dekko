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
#include "emailvalidator.h"
#include <QDebug>

EmailValidator::EmailValidator(QObject *parent) :
    QObject(parent), m_index(0)
{
}

bool EmailValidator::validate(const QString &emailAddress)
{
    m_index = 0;

    if (emailAddress.isEmpty()) {
        return false;
    }

    if (!skipWord(emailAddress) || m_index >= emailAddress.length()) {
        return false;
    }

    while (emailAddress.at(m_index) == QChar('.')) {
        ++m_index;
        if (m_index >= emailAddress.length()) {
            return false;
        }
        if (!skipWord(emailAddress)) {
            return false;
        }
        if (m_index >= emailAddress.length()) {
            return false;
        }
    }
    if (m_index + 1 >= emailAddress.length() || emailAddress.at(m_index) != QChar('@')) {
        return false;
    }
    m_index++;
    if (emailAddress.at(m_index) != QChar('[')) {
        if (!skipDomain(emailAddress)) {
            return false;
        }
        return m_index == emailAddress.length();
    }
    m_index++;
    if (m_index + 8 >= emailAddress.length()) {
        return false;
    }
    QString ipv6 = emailAddress.mid(m_index, 5);
    if (ipv6.toLower() == QLatin1String("ipv6:")) {
        m_index += QString("IPv6").length();
        if (!skipIPv6Literal(emailAddress)) {
            return false;
        }
    } else {
        if (!skipIPv4Literal(emailAddress)) {
            return false;
        }
    }

    if (m_index >= emailAddress.length() || emailAddress.at(m_index++) != QChar(']')) {
        return false;
    }
    qDebug() << "2nd INDEX == length: " << (m_index == emailAddress.length());
    return m_index == emailAddress.length();

}

const QString EmailValidator::AtomCharacters = QLatin1String("!#$%&'*+-/=?^_`{|}~");

bool EmailValidator::isLetterOrDigit(const QChar &c)
{
    return (c >= QChar('A') && c <= QChar('Z')) ||
            (c >= QChar('a') && c <= QChar('z')) ||
            (c >= QChar('0') && c <= QChar('9'));
}

bool EmailValidator::isAtom(const QChar &c)
{
    return isLetterOrDigit(c) || AtomCharacters.indexOf(c) != -1;
}

bool EmailValidator::isDomain(const QChar &c)
{
    return isLetterOrDigit(c) || c == QChar('-');
}

bool EmailValidator::isHexDigit(const QChar &c)
{
    return (c >= QChar('A') && c <= QChar('F')) || (c >= QChar('a') && c <= QChar('f')) || (c >= QChar('0') && c <= QChar('9'));
}

bool EmailValidator::skipAtom(const QString &txt)
{
    int start = m_index;
    while (m_index < txt.length() && isAtom(txt.at(m_index))) {
        ++m_index;
    }
    return m_index > start;
}

bool EmailValidator::skipSubDomain(const QString &txt)
{
    if (!isDomain(txt.at(m_index)) || txt.at(m_index) == QChar('-')) {
        return false;
    }
    m_index++;
    while (m_index < txt.length() && isDomain(txt.at(m_index))) {
        m_index++;
    }
    return true;
}

bool EmailValidator::skipDomain(const QString &txt)
{
    if (!skipSubDomain(txt)) {
        return false;
    }
    while (m_index < txt.length() && txt.at(m_index) == QChar('.')) {
        m_index++;
        if (m_index == txt.length()) {
            return false;
        }
        if (!skipSubDomain(txt)) {
            return false;
        }
    }
    return true;
}

bool EmailValidator::skipQuoted(const QString &txt)
{
    bool escaped = false;
    m_index++;
    while (m_index < txt.length()) {
        if (txt.at(m_index) >= QChar(128)) {
            return false;
        }
        if (txt.at(m_index) == QChar('\\')) {
            escaped = !escaped;
        } else if (!escaped) {
            if (txt.at(m_index) == QChar('"')) {
                break;
            }
        } else {
            escaped = false;
        }
        m_index++;
    }
    if (m_index >= txt.length() || txt.at(m_index) != QChar('"')) {
        return false;
    }
    m_index++;
    return true;

}

bool EmailValidator::skipWord(const QString &txt)
{
    if (txt.at(m_index) == QChar('"')) {
        return skipQuoted(txt);
    }
    return skipAtom(txt);
}

bool EmailValidator::skipIPv4Literal(const QString &txt)
{
    int groups = 0;
    qDebug() << "IPV4 start: " << txt.at(m_index);
    while (m_index < txt.length() && groups < 4) {
        int startIdx = m_index;
        int value = 0;

        while (m_index < txt.length() && txt.at(m_index) >= QChar('0') && txt.at(m_index) <= QChar('9')) {
            value = (value * 10) +  (txt.at(m_index).digitValue() - QChar('0').digitValue());
            m_index++;
        }
        if (m_index == startIdx || m_index - startIdx > 3 || value > 255) {
            return false;
        }
        groups++;
        if (groups < 4 && m_index < txt.length() && txt.at(m_index) == '.') {
            m_index++;
        }
    }
    return groups == 4;
}

bool EmailValidator::skipIPv6Literal(const QString &txt)
{
    bool compact = false;
    int colons = 0;

    while (m_index < txt.length()) {
        int startIndex = m_index;

        while (m_index < txt.length() && isHexDigit(txt.at(m_index))) {
            m_index++;
        }

        if (m_index >= txt.length()) {
            break;
        }

        if (m_index > startIndex && colons > 2 && txt.at(m_index) == QChar('.')) {
            // IPv6v4
            m_index = startIndex;

            if (!skipIPv4Literal(txt))
                return false;

            break;
        }

        int count = m_index - startIndex;
        if (count > 4)
            return false;

        if (txt.at(m_index) != QChar(':'))
            break;

        startIndex = m_index;
        while (m_index < txt.length() && txt.at(m_index) == QChar(':'))
            m_index++;

        count = m_index - startIndex;
        if (count > 2)
            return false;

        if (count == 2) {
            if (compact)
                return false;

            compact = true;
            colons += 2;
        } else {
            colons++;
        }
    }

    if (colons < 2)
        return false;

    if (compact)
        return colons < 6;

    return colons < 7;
}

QObject *EmailValidator::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    EmailValidator *validator = new EmailValidator(reinterpret_cast<QObject*>(engine));
    return validator;
}

static QWeakPointer<EmailValidator> validator;
QSharedPointer<EmailValidator> EmailValidator::instance()
{
    QSharedPointer<EmailValidator> val = validator.toStrongRef();
    if (val.isNull()) {
            val = QSharedPointer<EmailValidator>(new EmailValidator);
            validator = val;
    }
    return validator;
}
