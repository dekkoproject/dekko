#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QObject>
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QScopedPointer>
#include <QColor>
#include <QTextCharFormat>
#include "MarkdownTokenizer.h"

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    MarkdownHighlighter(QTextDocument *document);

signals:
    void highlightBlockAtPosition(int position);

    // QSyntaxHighlighter interface
protected:
    virtual void highlightBlock(const QString &text) override;

private slots:
    void onHighlightBlockAtPosition(int pos);
private:
    void setupTokenColors();
    void setupHeadingFontSize(bool useLargeHeadings);
    bool isHeadingBlockState(MarkdownTokenizer::TokenState state) const;
    void applyFormattingForToken(const MarkdownToken &token);

    QScopedPointer<MarkdownTokenizer> m_tokenizer;
    bool m_underlineForEmp;
    bool m_inBlockquote;
    QColor m_textColor;
    QColor m_background;
    QColor m_markup;
    QColor m_link;
    QTextCharFormat defaultFormat;
    bool applyStyleToMarkup[MarkdownToken::Last];
    QColor colorForToken[MarkdownToken::Last];
    bool emphasizeToken[MarkdownToken::Last];
    bool strongToken[MarkdownToken::Last];
    bool strongMarkup[MarkdownToken::Last];
    bool strikethroughToken[MarkdownToken::Last];
    int fontSizeIncrease[MarkdownToken::Last];
    int defaultFontSize;
};

#endif // MARKDOWNHIGHLIGHTER_H
