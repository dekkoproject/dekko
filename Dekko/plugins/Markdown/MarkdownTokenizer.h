#ifndef MARKDOWNTOKENIZER_H
#define MARKDOWNTOKENIZER_H

#include <QString>
#include <QList>
#include <QMap>
#include <QRegExp>
#include "MarkdownToken.h"

class MarkdownTokenizer
{
public:
    MarkdownTokenizer();
    ~MarkdownTokenizer();

    enum TokenState {
        Unknown = -1,
        Paragraph,
        ParagraphBreak,
        ListLineBreak,
        AtxHeading1,
        AtxHeading2,
        AtxHeading3,
        AtxHeading4,
        AtxHeading5,
        AtxHeading6,
        Blockquote,
        CodeBlock,
        GFMCodeFence,
        CodeFenceEnd,
        Comment,
        HorizontalRule,
        NumList,
        BulletList,
        SetextHead1Line1,
        SetextHead1Line2,
        SetextHead2Line1,
        SetextHead2Line2,
        TableHeader,
        TableDiv,
        TableRow
    };


    void tokenize(const QString &text, TokenState current, TokenState previous, TokenState next);

    typedef QMap<int, MarkdownToken> TokenPositionMap;

    TokenPositionMap tokens() const;

    TokenState state() const;

    bool shouldBackTrack() const;

    void clear();

private:
    void tokenizeMatches(
        MarkdownToken::TokenType tokenType,
        QString& text,
        QRegExp& regex,
        const int markupStartCount = 0,
        const int markupEndCount = 0,
        const bool replaceMarkupChars = false,
        const bool replaceAllChars = false
    );
    void addToken(const MarkdownToken &token);
    void setState(TokenState state);
    void setStartEarlier(const bool startEarlier);

    bool tokenizeSetextHeadingLine1(const QString& text);
    bool tokenizeSetextHeadingLine2(const QString& text);
    bool tokenizeAtxHeading(const QString& text);

    bool tokenizeNumberedList(const QString& text);
    bool tokenizeBulletPointList(const QString& text);
    bool tokenizeHorizontalRule(const QString& text);
    bool tokenizeBlockquote(const QString& text);
    bool tokenizeCodeBlock(const QString& text);
    bool tokenizeMultilineComment(const QString& text);

    bool tokenizeInline(const QString& text);
    void tokenizeVerbatim(QString& text);
    void tokenizeHtmlComments(QString& text);
    void tokenizeTableHeaderRow(QString& text);
    bool tokenizeTableDivider(const QString& text);
    void tokenizeTableRow(QString& text);

    TokenState m_current;
    TokenState m_previous;
    TokenState m_next;
    TokenState m_state;
    TokenPositionMap m_tokens;
    bool m_startEarlier;

    QRegExp paragraphBreakRegex;
    QRegExp heading1SetextRegex;
    QRegExp heading2SetextRegex;
    QRegExp blockquoteRegex;
    QRegExp githubCodeFenceStartRegex;
    QRegExp githubCodeFenceEndRegex;
    QRegExp pandocCodeFenceStartRegex;
    QRegExp pandocCodeFenceEndRegex;
    QRegExp numberedListRegex;
    QRegExp numberedNestedListRegex;
    QRegExp hruleRegex;
    QRegExp emphasisRegex;
    QRegExp strongRegex;
    QRegExp strikethroughRegex;
    QRegExp verbatimRegex;
    QRegExp htmlTagRegex;
    QRegExp htmlEntityRegex;
    QRegExp automaticLinkRegex;
    QRegExp inlineLinkRegex;
    QRegExp referenceLinkRegex;
    QRegExp referenceDefinitionRegex;
    QRegExp imageRegex;
    QRegExp htmlInlineCommentRegex;
    QRegExp mentionRegex;
    QRegExp pipeTableDividerRegex;

};

#endif // MARKDOWNTOKENIZER_H
