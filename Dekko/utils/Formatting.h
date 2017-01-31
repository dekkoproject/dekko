/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef FORMATTING_H
#define FORMATTING_H

#include <QObject>

enum class FlowedFormat {
    Plain,
    Flowed,
    Flowed_Delsp
};

class Formatting
{
public:
    struct Regex {
        static const QString email, hyperlink, bold, italic, underline;
        static const QString signatureSeperator, quotePlain, quoteFlowed;

        static QRegExp sigSeperator();
    };

    static QString singleLinePlainTextToHtml(QString line);
    static QString plainTextToHtml(const QString &text);
    static QString markupPlainTextToHtml(const QString &text);
    static QStringList quoteBody(QStringList bodyLines);
    static QString mangleReplySubject(const QString &subject);
    static QString mangleForwardSubject(const QString &subject);
};

#endif // FORMATTING_H
