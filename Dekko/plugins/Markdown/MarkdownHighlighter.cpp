#include "MarkdownHighlighter.h"
#include <QFont>

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *document, FormattingOptions *options) : QSyntaxHighlighter(document),
    m_tokenizer(new MarkdownTokenizer),
    m_inBlockquote(false),
    m_options(options)
{
    connect(this, &MarkdownHighlighter::highlightBlockAtPosition, this, &MarkdownHighlighter::onHighlightBlockAtPosition);

    QFont font;
    font.setFamily(m_options->get_fontFamily());
    font.setWeight(m_options->get_fontWeight());
    font.setItalic(false);
    font.setPointSizeF(12.0);
    font.setStyleStrategy(QFont::PreferAntialias);
    defaultFormat.setFont(font);
    defaultFormat.setForeground(QBrush(m_options->get_textColor()));

    setupTokenColors();

    for (int i = 0; i < MarkdownToken::Last; i++)
    {
        applyStyleToMarkup[i] = false;
        emphasizeToken[i] = false;
        strongToken[i] = false;
        strongMarkup[i] = false;
        strikethroughToken[i] = false;
        fontSizeIncrease[i] = 0;
    }

    for (int i = MarkdownToken::AtxHeading1; i <= MarkdownToken::AtxHeading6; i++)
    {
        applyStyleToMarkup[i] = true;
    }

    applyStyleToMarkup[MarkdownToken::Emphasis] = true;
    applyStyleToMarkup[MarkdownToken::Strong] = true;
    applyStyleToMarkup[MarkdownToken::AtxHeading1] = true;
    applyStyleToMarkup[MarkdownToken::AtxHeading2] = true;
    applyStyleToMarkup[MarkdownToken::AtxHeading3] = true;
    applyStyleToMarkup[MarkdownToken::AtxHeading4] = true;
    applyStyleToMarkup[MarkdownToken::AtxHeading5] = true;
    applyStyleToMarkup[MarkdownToken::AtxHeading6] = true;

    emphasizeToken[MarkdownToken::Emphasis] = true;
    emphasizeToken[MarkdownToken::Blockquote] = false;
    strongToken[MarkdownToken::Strong] = true;
    strongToken[MarkdownToken::Mention] = true;
    strongToken[MarkdownToken::AtxHeading1] = true;
    strongToken[MarkdownToken::AtxHeading2] = true;
    strongToken[MarkdownToken::AtxHeading3] = true;
    strongToken[MarkdownToken::AtxHeading4] = true;
    strongToken[MarkdownToken::AtxHeading5] = true;
    strongToken[MarkdownToken::AtxHeading6] = true;
    strongToken[MarkdownToken::SetextHead1Line1] = true;
    strongToken[MarkdownToken::SetextHead2Line1] = true;
    strongToken[MarkdownToken::SetextHead1Line2] = true;
    strongToken[MarkdownToken::SetextHead2Line2] = true;
    strongToken[MarkdownToken::TableHeader] = true;
    strikethroughToken[MarkdownToken::Strikethrough] = true;

    setupHeadingFontSize(m_options->get_enableLargeHeadingSizes());

    strongMarkup[MarkdownToken::NumberedList] = true;
    strongMarkup[MarkdownToken::Blockquote] = true;
    strongMarkup[MarkdownToken::BulletList] = true;
}

void MarkdownHighlighter::highlightBlock(const QString &text)
{
    MarkdownTokenizer::TokenState lastState = (MarkdownTokenizer::TokenState)currentBlockState();

    setFormat(0, text.length(), defaultFormat);

    if (!m_tokenizer.isNull())
    {
        m_tokenizer.data()->clear();

        QTextBlock block = currentBlock();
        MarkdownTokenizer::TokenState nextState = MarkdownTokenizer::Unknown;
        MarkdownTokenizer::TokenState previousState = (MarkdownTokenizer::TokenState)previousBlockState();

        if (block.next().isValid())
        {
            nextState = (MarkdownTokenizer::TokenState)block.next().userState();
        }

        m_tokenizer->tokenize(text, lastState, previousState, nextState);
        setCurrentBlockState(m_tokenizer->state());

        if (m_tokenizer->state() == MarkdownTokenizer::Blockquote)
        {
            m_inBlockquote = true;
        }
        else
        {
            m_inBlockquote = false;
        }

        for (const auto token : m_tokenizer->tokens()) {
            switch (token.type())
            {
                case MarkdownToken::Unknown:
                    qWarning("Highlighter found unknown token type in text block.");
                    break;
                default:
                    applyFormattingForToken(token);
                    break;
            }
        }

        if (m_tokenizer->shouldBackTrack())
        {
            QTextBlock previous = currentBlock().previous();
            emit highlightBlockAtPosition(previous.position());
        }
    }
}

void MarkdownHighlighter::onHighlightBlockAtPosition(int pos)
{
    QTextBlock block = document()->findBlock(pos);
    rehighlightBlock(block);
}

void MarkdownHighlighter::setupTokenColors()
{
    for (int i = 0; i < MarkdownToken::Last; i++)
    {
        colorForToken[i] = m_options->get_textColor();
    }

    colorForToken[MarkdownToken::HtmlTag] = m_options->get_markupColor();
    colorForToken[MarkdownToken::HtmlEntity] = m_options->get_markupColor();
    colorForToken[MarkdownToken::AutomaticLink] = m_options->get_linkColor();
    colorForToken[MarkdownToken::InlineLink] = m_options->get_linkColor();
    colorForToken[MarkdownToken::ReferenceLink] = m_options->get_linkColor();
    colorForToken[MarkdownToken::ReferenceDefinition] = m_options->get_linkColor();
    colorForToken[MarkdownToken::Image] = m_options->get_linkColor();
    colorForToken[MarkdownToken::Mention] = m_options->get_linkColor();
    colorForToken[MarkdownToken::HtmlComment] = m_options->get_markupColor();
    colorForToken[MarkdownToken::HorizontalRule] = m_options->get_markupColor();
    colorForToken[MarkdownToken::GFMCodeFence] = m_options->get_markupColor();
    colorForToken[MarkdownToken::CodeFenceEnd] = m_options->get_markupColor();
    colorForToken[MarkdownToken::SetextHead1Line2] = m_options->get_markupColor();
    colorForToken[MarkdownToken::SetextHead2Line2] = m_options->get_markupColor();
    colorForToken[MarkdownToken::TableDiv] = m_options->get_markupColor();
    colorForToken[MarkdownToken::TablePipe] = m_options->get_markupColor();
}

void MarkdownHighlighter::setupHeadingFontSize(bool useLargeHeadings)
{
    if (useLargeHeadings)
    {
        fontSizeIncrease[MarkdownToken::SetextHead1Line1] = 6;
        fontSizeIncrease[MarkdownToken::SetextHead2Line1] = 5;
        fontSizeIncrease[MarkdownToken::SetextHead1Line2] = 6;
        fontSizeIncrease[MarkdownToken::SetextHead2Line2] = 5;
        fontSizeIncrease[MarkdownToken::AtxHeading1] = 6;
        fontSizeIncrease[MarkdownToken::AtxHeading2] = 5;
        fontSizeIncrease[MarkdownToken::AtxHeading3] = 4;
        fontSizeIncrease[MarkdownToken::AtxHeading4] = 3;
        fontSizeIncrease[MarkdownToken::AtxHeading5] = 2;
        fontSizeIncrease[MarkdownToken::AtxHeading6] = 1;
    }
    else
    {
        fontSizeIncrease[MarkdownToken::SetextHead1Line1] = 0;
        fontSizeIncrease[MarkdownToken::SetextHead2Line1] = 0;
        fontSizeIncrease[MarkdownToken::SetextHead1Line2] = 0;
        fontSizeIncrease[MarkdownToken::SetextHead2Line2] = 0;
        fontSizeIncrease[MarkdownToken::AtxHeading1] = 0;
        fontSizeIncrease[MarkdownToken::AtxHeading2] = 0;
        fontSizeIncrease[MarkdownToken::AtxHeading3] = 0;
        fontSizeIncrease[MarkdownToken::AtxHeading4] = 0;
        fontSizeIncrease[MarkdownToken::AtxHeading5] = 0;
        fontSizeIncrease[MarkdownToken::AtxHeading6] = 0;
    }
}

bool MarkdownHighlighter::isHeadingBlockState(MarkdownTokenizer::TokenState state) const
{
    switch (state) {
    case MarkdownTokenizer::AtxHeading1:
    case MarkdownTokenizer::AtxHeading2:
    case MarkdownTokenizer::AtxHeading3:
    case MarkdownTokenizer::AtxHeading4:
    case MarkdownTokenizer::AtxHeading5:
    case MarkdownTokenizer::AtxHeading6:
    case MarkdownTokenizer::SetextHead1Line1:
    case MarkdownTokenizer::SetextHead2Line1:
        return true;
    default:
        return false;
    }
}

void MarkdownHighlighter::applyFormattingForToken(const MarkdownToken &token)
{
    if (token.type() != MarkdownToken::Unknown) {
        MarkdownToken::TokenType tokenType = token.type();
        QTextCharFormat fmt = this->format(token.position());

        QColor tokenColor = colorForToken[tokenType];

        fmt.setForeground(QBrush(tokenColor));

        if (strongToken[tokenType])
        {
            fmt.setFontWeight(QFont::Bold);
        }

        if (emphasizeToken[tokenType])
        {
            if (m_options->get_useUnderlineForEmp() && (tokenType != MarkdownToken::Blockquote))
            {
                fmt.setFontUnderline(true);
            }
            else
            {
                fmt.setFontItalic(true);
            }
        }

        if (strikethroughToken[tokenType])
        {
            fmt.setFontStrikeOut(true);
        }

        fmt.setFontPointSize(fmt.fontPointSize()
            + (qreal) fontSizeIncrease[tokenType]);

        QTextCharFormat markupFormat;

        if
        (
            applyStyleToMarkup[tokenType] &&
            (!emphasizeToken[tokenType] || !m_options->get_useUnderlineForEmp())
        )
        {
            markupFormat = fmt;
        }
        else
        {
            markupFormat = this->format(token.position());
        }

        QColor adjustedMarkupColor = m_options->get_markupColor().lighter();

        markupFormat.setForeground(QBrush(adjustedMarkupColor));

        if (strongMarkup[tokenType])
        {
            markupFormat.setFontWeight(QFont::Bold);
        }

        if (token.openingLength() > 0)
        {
            if
            (
                (MarkdownToken::Blockquote == tokenType)
            )
            {
                markupFormat.setBackground(QBrush(adjustedMarkupColor.lighter()));
                QString text = currentBlock().text();

                for (int i = token.position(); i < token.openingLength(); i++)
                {
                    if (!text[i].isSpace())
                    {
                        setFormat
                        (
                            i,
                            1,
                            markupFormat
                        );
                    }
                }
            }
            else
            {
                setFormat
                (
                    token.position(),
                    token.openingLength(),
                    markupFormat
                );
            }
        }

        setFormat
        (
            token.position() + token.openingLength(),
            token.length()
                - token.openingLength()
                - token.closingLength(),
            fmt
        );

        if (token.closingLength() > 0)
        {
            setFormat
            (
                token.position() + token.length()
                    - token.closingLength(),
                token.closingLength(),
                markupFormat
            );
        }
    }
    else
    {
        qWarning("Highlighter::applyFormattingForToken() was passed in a "
            "token of unknown type.");
    }
}
