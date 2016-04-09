# QRegularExpression examples for htmlify string

Note: This needs wrapping ip into some tidy functions

```cpp
#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QRegularExpression hyperlinks(QStringLiteral("(https?://(?:[;/?:@=$\\-_.+!',0-9a-zA-Z%#~\\[\\]\\(\\)\\*]|&amp;)+(?:[/@=$\\-_+'0-9a-zA-Z%#~]|&amp;))"));
    QRegularExpression email(QStringLiteral("((?:[a-zA-Z0-9_\\.!#$%'\\*\\+\\-/=?^`\\{|\\}~]|&amp;)+@[a-zA-Z0-9\\.\\-_]+)"));
    QRegularExpression bold(QStringLiteral("((^|[\\s\\(\\[\\{])\\*((?!\\*)\\S+)\\*($|[\\s\\),;.\\]\\}]))"));
    QRegularExpression italic(QStringLiteral("((^|[\\s\\(\\[\\{])/((?!/)\\S+)/($|[\\s\\),;.\\]\\}]))"));
    QRegularExpression underline(QStringLiteral("((^|[\\s\\(\\[\\{])_((?!_)\\S+)_($|[\\s\\),;.\\]\\}]))"));


    QString basicurl("asdasdasdad http://example.com aslkdjalksdjalkd https://people.ubuntu.com/otherones kajsdkjahsd 192.168.0.12/files");
    QString second("alskdalskdhajshfk Dan Chapman <dan@dpniel.com> askjdhkajsdh");
    QString boldstring("askdlkafljhdkljshlkahdjkgh *singleboldword* akjsdhak bold*mid*string alksdjlaksdj");
    QString italicString("kkasjdhkajsdh /italicstring/ aksj/this/shouldn't/dkajhd /somemore/ askdjahsdk /does/this/ ?");
    QString underlineString("hskjdahskjdahskd _underlined_ askjdakd_notunderlined_akjsdaskh _but_this_should_");
    QRegularExpressionMatchIterator i = hyperlinks.globalMatch(basicurl);
    QStringList links;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        links << match.captured(1).trimmed();
    }
    qDebug() << "Links " << links;
    foreach(auto link, links) {
        QString urlreplace(QStringLiteral("<a href=%1>%1</a>").arg(link));
        basicurl.replace(link, urlreplace);
    }
    qDebug() << "BasicUrl: " << basicurl;
    qDebug() << "##################################";
    QRegularExpressionMatchIterator it = email.globalMatch(second);
    QStringList links2;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        links2 << match.captured(1).trimmed();
    }
    qDebug() << "Links2 " << links2;
    foreach(auto email, links2) {
        QString urlreplace(QStringLiteral("<a href=\"mailto:%1\">%1</a>").arg(email));
        second.replace(email, urlreplace);
    }
    qDebug() << "Second: " << second;
    qDebug() << "##################################";
    QRegularExpressionMatchIterator bit = bold.globalMatch(boldstring);
    QStringList boldMatches;
    while (bit.hasNext()) {
        QRegularExpressionMatch match = bit.next();
        boldMatches << match.captured(1).trimmed();
    }
    qDebug() << "Bold matches " << boldMatches;
    foreach(auto b, boldMatches) {
        QString urlreplace(QStringLiteral("<strong>%1</strong>").arg(b.mid(1, b.length() - 2)));
        boldstring.replace(b, urlreplace);
    }
    qDebug() << "Bold replaces: " << boldstring;
    qDebug() << "##################################";
    QRegularExpressionMatchIterator itI = italic.globalMatch(italicString);
    QStringList italicMatches;
    while (itI.hasNext()) {
        QRegularExpressionMatch match = itI.next();
        italicMatches << match.captured(1).trimmed();
    }
    qDebug() << "Italic matches " << italicMatches;
    foreach(auto i, italicMatches) {
        QString urlreplace(QStringLiteral("<emp>%1</emp>").arg(i.mid(1, i.length() - 2)));
        italicString.replace(i, urlreplace);
    }
    qDebug() << "Italic replaces: " << italicString;

    qDebug() << "##################################";
    QRegularExpressionMatchIterator ui = underline.globalMatch(underlineString);
    QStringList uMatches;
    while (ui.hasNext()) {
        QRegularExpressionMatch match = ui.next();
        uMatches << match.captured(1).trimmed();
    }
    qDebug() << "Underline matches " << uMatches;
    foreach(auto u, uMatches) {
        QString urlreplace(QStringLiteral("<u>%1</u>").arg(u.mid(1, u.length() - 2)));
        underlineString.replace(u, urlreplace);
    }
    qDebug() << "Underline replaces: " << underlineString;

    return 0;
}
```
