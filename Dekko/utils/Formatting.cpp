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
#include "Formatting.h"
#include <QDebug>
#include <QStack>
#include <QPair>
#include <QColor>
#include <QPalette>
#include <QStringList>
#include <QTextDocument>
#include <QDir>
#include <QFontInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#define SIGNATURE_SPERATOR -2

const QString Formatting::Regex::hyperlink = QStringLiteral("(https?://(?:[;/?:@=$\\-_.+!',0-9a-zA-Z%#~\\[\\]\\(\\)\\*]|&amp;)+(?:[/@=$\\-_+'0-9a-zA-Z%#~]|&amp;))");
const QString Formatting::Regex::email = QStringLiteral("((?:[a-zA-Z0-9_\\.!#$%'\\*\\+\\-/=?^`\\{|\\}~]|&amp;)+@[a-zA-Z0-9\\.\\-_]+)");
const QString Formatting::Regex::bold = QStringLiteral("((^|[\\s\\(\\[\\{])\\*((?!\\*)\\S+)\\*($|[\\s\\),;.\\]\\}]))");
const QString Formatting::Regex::italic = QStringLiteral("((^|[\\s\\(\\[\\{])/((?!/)\\S+)/($|[\\s\\),;.\\]\\}]))");
const QString Formatting::Regex::underline = QStringLiteral("((^|[\\s\\(\\[\\{])_((?!_)\\S+)_($|[\\s\\),;.\\]\\}]))");
// We want this to be an exact match so we have to stick the regex between anchoring expression ^(regex)$
const QString Formatting::Regex::signatureSeperator = QStringLiteral("^(-- |_{45,})(\\r)?$");
const QString Formatting::Regex::quotePlain = QStringLiteral("^( >|>)+");
const QString Formatting::Regex::quoteFlowed = QStringLiteral("^>+");

struct TextInfo {
    int depth;
    QString text;
    TextInfo(const int depth, const QString &text): depth(depth), text(text){}
};

// ptext-2-html passes
namespace {
static QString lineWithoutTrailingCr(const QString &line) {
    return line.endsWith(QLatin1Char('\r')) ? line.left(line.size() - 1) : line;
}

static QRegularExpression quoteMarks(const FlowedFormat &format) {
    return QRegularExpression(format == FlowedFormat::Plain ?
                                  Formatting::Regex::quotePlain :
                                  Formatting::Regex::quoteFlowed);
}

static QString firstNLines(const QString &input, int numLines, const int charsPerLine)
{
    Q_ASSERT(numLines >= 2);
    QString out = input.section(QLatin1Char('\n'), 0, numLines - 1, QString::SectionSkipEmpty);
    const int cutoff = numLines * charsPerLine;
    if (out.size() >= cutoff) {
        int pos = input.indexOf(QLatin1Char(' '), cutoff);
        if (pos != -1)
            return out.left(pos - 1);
    }
    return out;
}

static void firstPass(std::vector<TextInfo> &buffer, QStringList &lines, bool &sigSeen, const FlowedFormat &format)
{

    static QRegularExpression sig(Formatting::Regex::signatureSeperator);

    Q_FOREACH(const QString &line, lines) {

        if (line.isEmpty()) {
            buffer.emplace_back(0, line);
            continue;
        }

        if (sig.match(line, 0).hasMatch()) {
            buffer.emplace_back(SIGNATURE_SPERATOR, lineWithoutTrailingCr(line));
            sigSeen = true;
            continue;
        }

        int ql = 0;
        QRegularExpressionMatch match = quoteMarks(format).match(line);
        if (!sigSeen && match.hasMatch() && match.capturedStart() == 0) {
            ql = match.captured(0).count(QStringLiteral(">"));
        }
        buffer.emplace_back(ql, lineWithoutTrailingCr(line));
    }
}

static void secondPass(std::vector<TextInfo> &buffer, const FlowedFormat &format)
{
    auto it = buffer.begin();
    while (it < buffer.end() && it->depth != SIGNATURE_SPERATOR) {
        it->text.remove(quoteMarks(format));

        switch(format) {
        case FlowedFormat::Flowed:
        case FlowedFormat::Flowed_Delsp:
        {
            if (it->text.startsWith(QLatin1Char(' '))) {
                it->text.remove(0, 1);
            }

            // quirk: fix a flowed line which actually isn't flowed
            if (it->text.endsWith(QLatin1Char(' ')) && (
                        it+1 == buffer.end() || // end-of-document
                        (it+1)->depth == SIGNATURE_SPERATOR || // right in front of the separator
                        (it+1)->depth != it->depth // end of paragraph
                        )) {
                it->text.chop(1);
            }
            break;

        }
        case FlowedFormat::Plain:
        {
            if (it->depth > 0 && it->text.startsWith(QLatin1Char(' '))) {
                // Because the space is re-added when we prepend the quotes. Adding that space is done
                // in order to make it look nice, i.e. to prevent lines like ">>something".
                it->text.remove(0, 1);
            }
            break;
        }
        }

        if (it == buffer.begin()) {
            // No "previous line"
            ++it;
            continue;
        }
        // Check for the line joining
        auto prev = it - 1;
        if (prev->depth == it->depth) {

            QString separator = QStringLiteral("\n");
            switch (format) {
            case FlowedFormat::Plain:
                // nothing fancy to do here, we cannot really join lines
                break;
            case FlowedFormat::Flowed:
            case FlowedFormat::Flowed_Delsp:
                // CR LF trailing is stripped already (LFs by the split into lines, CRs by lineWithoutTrailingCr in pass #1),
                // so we only have to check for the trailing space
                if (prev->text.endsWith(QLatin1Char(' '))) {

                    // implement the DelSp thingy
                    if (format == FlowedFormat::Flowed_Delsp) {
                        prev->text.chop(1);
                    }

                    if (it->text.isEmpty() || prev->text.isEmpty()) {
                        // This one or the previous line is a blank one, so we cannot really join them
                    } else {
                        separator = QString();
                    }
                }
                break;
            }
            prev->text += separator + it->text;
            it = buffer.erase(it);
        } else {
            ++it;
        }
    }
}

static QString finalPass(std::vector<TextInfo> &lineBuffer)
{
    bool signatureSeparatorSeen = false;
    int quoteLevel = 0;
    QStringList markup;
    int interactiveControlsId = 0;
    QStack<QPair<int,int> > controlStack;
    auto closeQuotesUpTo = [](QStringList &markup, QStack<QPair<int, int> > &controlStack, int &quoteLevel, const int finalQuoteLevel)
    {
        static QString closingLabel(QStringLiteral("<label for=\"q%1\"></label>"));
        static QLatin1String closeSingleQuote("</blockquote>");
        static QLatin1String closeQuoteBlock("</span></span>");

        Q_ASSERT(quoteLevel >= finalQuoteLevel);

        while (quoteLevel > finalQuoteLevel) {
            // Check whether an interactive control element is supposed to be present here
            bool controlBlock = !controlStack.isEmpty() && (quoteLevel == controlStack.top().first);
            if (controlBlock) {
                markup << closingLabel.arg(controlStack.pop().second);
            }
            markup << closeSingleQuote;
            --quoteLevel;
            if (controlBlock) {
                markup << closeQuoteBlock;
            }
        }
    };
    for (auto it = lineBuffer.begin(); it != lineBuffer.end(); ++it) {

        if (it->depth == SIGNATURE_SPERATOR && !signatureSeparatorSeen) {
            // The first signature separator
            signatureSeparatorSeen = true;
            closeQuotesUpTo(markup, controlStack, quoteLevel, 0);
            markup << QStringLiteral("<span class=\"signature\">") + Formatting::singleLinePlainTextToHtml(it->text);
            markup << QStringLiteral("\n");
            continue;
        }

        if (signatureSeparatorSeen) {
            // Just copy the data
            markup << Formatting::singleLinePlainTextToHtml(it->text);
            if (it+1 != lineBuffer.end())
                markup << QStringLiteral("\n");
            continue;
        }

        Q_ASSERT(quoteLevel == 0 || quoteLevel != it->depth);

        if (quoteLevel > it->depth) {
            // going back in the quote hierarchy
            closeQuotesUpTo(markup, controlStack, quoteLevel, it->depth);
        }

        // Pretty-formatted block of the ">>>" characters
        QString quotemarks;

        if (it->depth) {
            quotemarks += QLatin1String("<span class=\"quotemarks\">");
            for (int i = 0; i < it->depth; ++i) {
                quotemarks += QLatin1String("&gt;");
            }
            quotemarks += QLatin1String(" </span>");
        }

        static const int previewLines = 5;
        static const int charsPerLineEquivalent = 160;
        static const int forceCollapseAfterLines = 10;

        if (quoteLevel < it->depth) {
            // We're going deeper in the quote hierarchy
            QString line;
            while (quoteLevel < it->depth) {
                ++quoteLevel;

                // Check whether there is anything at the newly entered level of nesting
                bool anythingOnJustThisLevel = false;

                // A short summary of the quotation
                QString preview;

                auto runner = it;
                while (runner != lineBuffer.end()) {
                    if (runner->depth == quoteLevel) {
                        anythingOnJustThisLevel = true;

                        ++interactiveControlsId;
                        controlStack.push(qMakePair(quoteLevel, interactiveControlsId));

                        QString omittedStuff;
                        QString previewPrefix, previewSuffix;
                        QString currentChunk = firstNLines(runner->text, previewLines, charsPerLineEquivalent);
                        QString omittedPrefix, omittedSuffix;
                        QString previewQuotemarks;

                        if (runner != it ) {
                            // we have skipped something, make it obvious to the user

                            // Find the closest level which got collapsed
                            int closestDepth = std::numeric_limits<int>::max();
                            auto depthRunner(it);
                            while (depthRunner != runner) {
                                closestDepth = std::min(closestDepth, depthRunner->depth);
                                ++depthRunner;
                            }

                            // The [...] marks shall be prefixed by the closestDepth quote markers
                            omittedStuff = QStringLiteral("<span class=\"quotemarks\">");
                            for (int i = 0; i < closestDepth; ++i) {
                                omittedStuff += QLatin1String("&gt;");
                            }
                            for (int i = runner->depth; i < closestDepth; ++i) {
                                omittedPrefix += QLatin1String("<blockquote>");
                                omittedSuffix += QLatin1String("</blockquote>");
                            }
                            omittedStuff += QStringLiteral(" </span><label for=\"q%1\">...</label>").arg(interactiveControlsId);

                            // Now produce the proper quotation for the preview itself
                            for (int i = quoteLevel; i < runner->depth; ++i) {
                                previewPrefix.append(QLatin1String("<blockquote>"));
                                previewSuffix.append(QLatin1String("</blockquote>"));
                            }
                        }

                        previewQuotemarks = QStringLiteral("<span class=\"quotemarks\">");
                        for (int i = 0; i < runner->depth; ++i) {
                            previewQuotemarks += QLatin1String("&gt;");
                        }
                        previewQuotemarks += QLatin1String(" </span>");

                        preview = previewPrefix
                                + omittedPrefix + omittedStuff + omittedSuffix
                                + previewQuotemarks
                                + Formatting::singleLinePlainTextToHtml(currentChunk)
                                .replace(QLatin1String("\n"), QStringLiteral("\n") + previewQuotemarks)
                                + previewSuffix;

                        break;
                    }
                    if (runner->depth < quoteLevel) {
                        // This means that we have left the current level of nesting, so there cannot possible be anything else
                        // at the current level of nesting *and* in the current quote block
                        break;
                    }
                    ++runner;
                }

                // Is there nothing but quotes until the end of mail or until the signature separator?
                bool nothingButQuotesAndSpaceTillSignature = true;
                runner = it;
                while (++runner != lineBuffer.end()) {
                    if (runner->depth == SIGNATURE_SPERATOR)
                        break;
                    if (runner->depth > 0)
                        continue;
                    if (runner->depth == 0 && !runner->text.isEmpty()) {
                        nothingButQuotesAndSpaceTillSignature = false;
                        break;
                    }
                }

                // Size of the current level, including the nested stuff
                int currentLevelCharCount = 0;
                int currentLevelLineCount = 0;
                runner = it;
                while (runner != lineBuffer.end() && runner->depth >= quoteLevel) {
                    currentLevelCharCount += runner->text.size();
                    // one for the actual block
                    currentLevelLineCount += runner->text.count(QLatin1Char('\n')) + 1;
                    ++runner;
                }


                if (!anythingOnJustThisLevel) {
                    // no need for fancy UI controls
                    line += QLatin1String("<blockquote>");
                    continue;
                }

                if (quoteLevel == it->depth
                        && currentLevelCharCount <= charsPerLineEquivalent * previewLines
                        && currentLevelLineCount <= previewLines) {
                    // special case: the quote is very short, no point in making it collapsible
                    line += QStringLiteral("<span class=\"level\"><input type=\"checkbox\" id=\"q%1\"/>").arg(interactiveControlsId)
                            + QStringLiteral("<span class=\"shortquote\"><blockquote>") + quotemarks
                            + Formatting::singleLinePlainTextToHtml(it->text).replace(QLatin1String("\n"), QStringLiteral("\n") + quotemarks);
                } else {
                    bool collapsed = nothingButQuotesAndSpaceTillSignature
                            || quoteLevel > 1
                            || currentLevelCharCount >= charsPerLineEquivalent * forceCollapseAfterLines
                            || currentLevelLineCount >= forceCollapseAfterLines;

                    line += QStringLiteral("<span class=\"level\"><input type=\"checkbox\" id=\"q%1\" %2/>")
                            .arg(QString::number(interactiveControlsId),
                                 collapsed ? QStringLiteral("checked=\"checked\"") : QString())
                            + QStringLiteral("<span class=\"short\"><blockquote>")
                            + preview
                            + QStringLiteral(" <label for=\"q%1\">...</label>").arg(interactiveControlsId)
                            + QStringLiteral("</blockquote></span>")
                            + QStringLiteral("<span class=\"full\"><blockquote>");
                    if (quoteLevel == it->depth) {
                        // We're now finally on the correct level of nesting so we can output the current line
                        line += quotemarks + Formatting::singleLinePlainTextToHtml(it->text)
                                .replace(QLatin1String("\n"), QStringLiteral("\n") + quotemarks);
                    }
                }
            }
            markup << line;
        } else {
            // Either no quotation or we're continuing an old quote block and there was a nested quotation before
            markup << quotemarks + Formatting::singleLinePlainTextToHtml(it->text)
                      .replace(QLatin1String("\n"), QStringLiteral("\n") + quotemarks);
        }

        auto next = it + 1;
        if (next != lineBuffer.end()) {
            if (next->depth >= 0 && next->depth < it->depth) {
                // Decreasing the quotation level -> no starting <blockquote>
                markup << QStringLiteral("\n");
            } else if (it->depth == 0) {
                // Non-quoted block which is not enclosed in a <blockquote>
                markup << QStringLiteral("\n");
            }
        }
    }

    if (signatureSeparatorSeen) {
        // Terminate the signature
        markup << QStringLiteral("</span>");
    }

    if (quoteLevel) {
        // Terminate the quotes
        closeQuotesUpTo(markup, controlStack, quoteLevel, 0);
    }

    Q_ASSERT(controlStack.isEmpty());

    return markup.join(QString());
}

}

QString Formatting::singleLinePlainTextToHtml(QString line)
{
    line = line.toHtmlEscaped();

    auto htmlifyLink = [](QString &line, const QString &link) {
        //        qDebug() << "Link: " << link;
        QString urlreplace(QStringLiteral("<a href=%1>%1</a>").arg(link));
        line.replace(link, urlreplace);
    };

    auto htmlifyEmail = [](QString &line, const QString &mail) {
        //        qDebug() << "Mailto: " << mail;
        QString urlreplace(QStringLiteral("<a href=\"mailto:%1\">%1</a>").arg(mail));
        line.replace(mail, urlreplace);
    };

    auto htmlifySpecials = [](QString &line, const QString &value, const QString &tag) {
        //        qDebug() << "Specials: " << value << tag;
        QString urlreplace(QStringLiteral("<%1>%2</%1>").arg(tag, value));
        line.replace(value, urlreplace);
    };

    auto regexToStringList = [](QString &line, const QString &regex) -> QStringList {
        QRegularExpression rx(regex);
        QRegularExpressionMatchIterator it = rx.globalMatch(line);
        QStringList result;
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            result << match.captured(1).trimmed();
        }
        return result;
    };

    // hyperlinks first
    foreach(auto link, regexToStringList(line, Formatting::Regex::hyperlink)) { htmlifyLink(line, link); }
    // mailto's
    foreach(auto email, regexToStringList(line, Formatting::Regex::email)) { htmlifyEmail(line, email); }
    // bold's
    foreach(auto bold, regexToStringList(line, Formatting::Regex::bold)) { htmlifySpecials(line, bold, QStringLiteral("strong")); }
    // italic's
    foreach(auto italic, regexToStringList(line, Formatting::Regex::italic)) { htmlifySpecials(line, italic, QStringLiteral("emp")); }
    // underscore's
    foreach(auto underscore, regexToStringList(line, Formatting::Regex::underline)) { htmlifySpecials(line, underscore, QStringLiteral("u")); }

    return line;
}

QString Formatting::plainTextToHtml(const QString &text)
{
    auto lines = text.split('\n');
    std::vector<TextInfo> lineBuffer;
    lineBuffer.reserve(lines.size());
    bool signatureSeen = false;
    firstPass(lineBuffer, lines, signatureSeen, FlowedFormat::Plain); // TODO: add format arg.
    secondPass(lineBuffer, FlowedFormat::Plain);
    return finalPass(lineBuffer);
}
#include "Paths.h"
QString Formatting::markupPlainTextToHtml(const QString &text)
{
    static const QString defaultStyle = QString::fromUtf8(
                "pre{word-wrap: break-word; white-space: pre-wrap;}"
                // The following line, sadly, produces a warning "QFont::setPixelSize: Pixel size <= 0 (0)".
                // However, if it is not in place or if the font size is set higher, even to 0.1px, WebKit reserves space for the
                // quotation characters and therefore a weird white area appears. Even width: 0px doesn't help, so it looks like
                // we will have to live with this warning for the time being.
                ".quotemarks{color:transparent;font-size:0px;}"

                // Cannot really use the :dir(rtl) selector for putting the quote indicator to the "correct" side.
                // It's CSS4 and it isn't supported yet.
                "blockquote{margin: 0 0 0 .8ex; border-left: 1px #ccc solid; unicode-bidi: -webkit-plaintext; padding-left:1ex}"

                // Stop the font size from getting smaller after reaching two levels of quotes
                // (ie. starting on the third level, don't make the size any smaller than what it already is)
                "blockquote blockquote blockquote {}"
                ".signature{opacity: 0.6;}"

                // Dynamic quote collapsing via pure CSS, yay
                "input {display: none}"
                "input ~ span.full {display: block}"
                "input ~ span.short {display: none}"
                "input:checked ~ span.full {display: block}"
                "input:checked ~ span.short {display: none}"
                "label {display: none; border: 1px solid %2; border-radius: 5px; padding: 0px 4px 0px 4px; white-space: nowrap}"
                // BLACK UP-POINTING SMALL TRIANGLE (U+25B4)
                // BLACK DOWN-POINTING SMALL TRIANGLE (U+25BE)
                "span.full > blockquote > label:before {content: \"\u25b4\"}"
                "span.short > blockquote > label:after {content: \" \u25be\"}"
                "span.shortquote > blockquote > label {display: none}"
                );


    QPalette palette = QGuiApplication::palette();
    QString textColors = QString::fromUtf8("body { color: #5D5D5D; font-family: Ubuntu !important; font-size: 1.1rem; weight: light }"
                                           "a:link { color: #19b6ee } a:visited { color: #19b6ee } a:hover { color: #19b6ee }");
    // looks like there's no special color for hovered links in Qt

    // build stylesheet and html header
    QColor tintForQuoteIndicator = palette.base().color();
    tintForQuoteIndicator.setAlpha(0x66);
    static QString stylesheet = defaultStyle + textColors;
    static QFile file(Paths::configLocationForFile(QStringLiteral("message.css")));
    static QDateTime lastVersion;
    QDateTime lastTouched(file.exists() ? QFileInfo(file).lastModified() : QDateTime());
    if (lastVersion < lastTouched) {
        stylesheet = defaultStyle;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString userSheet = QString::fromLocal8Bit(file.readAll().data());
            lastVersion = lastTouched;
            stylesheet += "\n" + userSheet;
            file.close();
        }
    }

    // The dir="auto" is required for WebKit to treat all paragraphs as entities with possibly different text direction.
    // The individual paragraphs unfortunately share the same text alignment, though, as per
    // https://bugs.webkit.org/show_bug.cgi?id=71194 (fixed in Blink already).
    QString htmlHeader("<!DOCTYPE html><html><head><meta name=\"description\" content=\"plaintext\"><style type=\"text/css\"><!--" /*+ textColors + fontSpecification*/ + stylesheet +
                       "--></style></head><body><pre dir=\"auto\">");
    static QString htmlFooter(QStringLiteral("\n</pre></body></html>"));

    QString markup = Formatting::plainTextToHtml(text);
    // and finally set the marked up page.
    return htmlHeader + markup + htmlFooter;
}

// pinched from trojita/Composer/PlainTextFormatter.cpp and modified to use QRegularExpression
QStringList Formatting::quoteBody(QStringList bodyLines)
{
    QStringList quote;
    for (QStringList::iterator line = bodyLines.begin(); line != bodyLines.end(); ++line) {
        if (Formatting::Regex::sigSeperator().exactMatch(*line)) {
            // we've hit the top level signature. In Dekko we don't include this
            // but other clients do... but they will be nested in > quote blocks so that's fine and we
            // will just work with that.
            break;
        }
        // rewrap - we need to keep the quotes at < 79 chars, yet the grow with every level
        if (line->length() < 79-2) {
            // this line is short enough, prepend quote mark and continue
            if (line->isEmpty() || line->at(0) == '>')
                line->prepend(">");
            else
                line->prepend("> ");
            quote << *line;
            continue;
        }
        // long line -> needs to be wrapped
        // 1st, detect the quote depth and eventually stript the quotes from the line
        int quoteLevel = 0;
        int contentStart = 0;
        if (line->at(0) == '>') {
            quoteLevel = 1;
            while (quoteLevel < line->length() && line->at(quoteLevel) == '>')
                ++quoteLevel;
            contentStart = quoteLevel;
            if (quoteLevel < line->length() && line->at(quoteLevel) == ' ')
                ++contentStart;
        }

        // 2nd, build a quote string
        QString quotemarks;
        for (int i = 0; i < quoteLevel; ++i)
            quotemarks += QLatin1String(">");
        quotemarks += QLatin1String("> ");

        // 3rd, wrap the line, prepend the quotemarks to each line and add it to the quote text
        int space(contentStart), lastSpace(contentStart), pos(contentStart), length(0);
        while (pos < line->length()) {
            if (line->at(pos) == ' ')
                space = pos+1;
            ++length;
            if (length > 65-quotemarks.length() && space != lastSpace) {
                // wrap
                quote << quotemarks + line->mid(lastSpace, space - lastSpace);
                lastSpace = space;
                length = pos - space;
            }
            ++pos;
        }
        quote << quotemarks + line->mid(lastSpace);
    }
    return quote;
}

QString Formatting::mangleReplySubject(const QString &subject)
{
    // These operations should *not* check for internationalized variants of "Re"; these are evil.

#define RE_PREFIX_RE "(?:(?:Re:\\s*)*)"
#define RE_PREFIX_ML "(?:(\\[[^\\]]+\\]\\s*)?)"

    static QRegExp rePrefixMatcher(QLatin1String("^"
                                                 RE_PREFIX_RE // a sequence of "Re: " prefixes
                                                 RE_PREFIX_ML // something like a mailing list prefix
                                                 RE_PREFIX_RE // a sequence of "Re: " prefixes
                                                 ), Qt::CaseInsensitive);
    rePrefixMatcher.setPatternSyntax(QRegExp::RegExp2);
    QLatin1String correctedPrefix("Re: ");

    if (rePrefixMatcher.indexIn(subject) == -1) {
        // Our regular expression has failed, so better play it safe and blindly prepend "Re: "
        return correctedPrefix + subject;
    } else {
        QStringList listPrefixes;
        int pos = 0;
        int oldPos = 0;
        while ((pos = rePrefixMatcher.indexIn(subject, pos, QRegExp::CaretAtOffset)) != -1) {
            if (rePrefixMatcher.matchedLength() == 0)
                break;
            pos += rePrefixMatcher.matchedLength();
            if (!listPrefixes.contains(rePrefixMatcher.cap(1)))
                listPrefixes << rePrefixMatcher.cap(1);
            oldPos = pos;
        }

        QString mlPrefix = listPrefixes.join(QString()).trimmed();
        QString baseSubject = subject.mid(oldPos + qMax(0, rePrefixMatcher.matchedLength()));

        if (!mlPrefix.isEmpty() && !baseSubject.isEmpty())
            mlPrefix += QLatin1Char(' ');

        return correctedPrefix + mlPrefix + baseSubject;
    }
}

QString Formatting::mangleForwardSubject(const QString &subject)
{
    QLatin1String forwardPrefix("Fwd: ");
    return forwardPrefix + subject;
}


QRegExp Formatting::Regex::sigSeperator()
{
    // "-- " is the standards-compliant signature separator.
    // "Line of underscores" is non-standard garbage which Mailman happily generates. Yes, it's nasty and ugly.
    return QRegExp(QLatin1String("(-- |_{45,})(\\r)?"));
}
