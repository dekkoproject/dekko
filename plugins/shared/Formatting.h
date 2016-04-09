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
    };

    static QString singleLinePlainTextToHtml(QString line);
    static QString plainTextToHtml(const QString &text);
    static QString markupPlainTextToHtml(const QString &text);
};

#endif // FORMATTING_H
