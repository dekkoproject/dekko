#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QObject>
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QScopedPointer>
#include <QColor>
#include <QTextCharFormat>
#include "MarkdownTokenizer.h"
#include "FormattingOptions.h"

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    MarkdownHighlighter(QTextDocument *document, FormattingOptions *options);

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
    bool m_inBlockquote;
    QTextCharFormat defaultFormat;
    bool applyStyleToMarkup[MarkdownToken::Last];
    QColor colorForToken[MarkdownToken::Last];
    bool emphasizeToken[MarkdownToken::Last];
    bool strongToken[MarkdownToken::Last];
    bool strongMarkup[MarkdownToken::Last];
    bool strikethroughToken[MarkdownToken::Last];
    int fontSizeIncrease[MarkdownToken::Last];
    int defaultFontSize;
    FormattingOptions *m_options;
};

#endif // MARKDOWNHIGHLIGHTER_H
