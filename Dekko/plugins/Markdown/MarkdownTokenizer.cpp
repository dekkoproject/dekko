#include "MarkdownTokenizer.h"

MarkdownTokenizer::MarkdownTokenizer():
    m_state(TokenState::Unknown),
    m_startEarlier(false)
{
    paragraphBreakRegex.setPattern("^\\s*$");
    heading1SetextRegex.setPattern("^===+\\s*$");
    heading2SetextRegex.setPattern("^---+\\s*$");
    blockquoteRegex.setPattern("^ {0,3}>.*$");
    githubCodeFenceStartRegex.setPattern("^```+.*$");
    githubCodeFenceEndRegex.setPattern("^```+\\s*$");
    pandocCodeFenceStartRegex.setPattern("^~~~+.*$");
    pandocCodeFenceEndRegex.setPattern("^~~~+\\s*$");
    numberedListRegex.setPattern("^ {0,3}[0-9]+[.)]\\s+.*$");
    numberedNestedListRegex.setPattern("^\\s*[0-9]+[.)]\\s+.*$");
    hruleRegex.setPattern("\\s*(\\*\\s*){3,}|(\\s*(_\\s*){3,})|((\\s*(-\\s*){3,}))");
    emphasisRegex.setPattern("(\\*(?![\\s*]).*[^\\s*]\\*)|_(?![\\s_]).*[^\\s_]_");
    emphasisRegex.setMinimal(true);
    strongRegex.setPattern("\\*\\*(?=\\S).*\\S\\*\\*(?!\\*)|__(?=\\S).*\\S__(?!_)");
    strongRegex.setMinimal(true);
    strikethroughRegex.setPattern("~~[^\\s]+.*[^\\s]+~~");
    strikethroughRegex.setMinimal(true);
    verbatimRegex.setPattern("`+");
    htmlTagRegex.setPattern("<[^<>]+>");
    htmlTagRegex.setMinimal(true);
    htmlEntityRegex.setPattern("&[a-zA-Z]+;|&#x?[0-9]+;");
    automaticLinkRegex.setPattern("(<[a-zA-Z]+\\:.+>)|(<.+@.+>)");
    automaticLinkRegex.setMinimal(true);
    inlineLinkRegex.setPattern("\\[.+\\]\\(.+\\)");
    inlineLinkRegex.setMinimal(true);
    referenceLinkRegex.setPattern("\\[(.+)\\]");
    referenceLinkRegex.setMinimal(true);
    referenceDefinitionRegex.setPattern("^\\s*\\[.+\\]:");
    imageRegex.setPattern("!\\[.*\\]\\(.+\\)");
    imageRegex.setMinimal(true);
    htmlInlineCommentRegex.setPattern("<!--.*-->");
    htmlInlineCommentRegex.setMinimal(true);
    mentionRegex.setPattern("\\B@\\w+(\\-\\w+)*(/\\w+(\\-\\w+)*)?");
    pipeTableDividerRegex.setPattern("^ {0,3}(\\|[ :]?)?-{3,}([ :]?\\|[ :]?-{3,}([ :]?\\|)?)+\\s*$");
}

void MarkdownTokenizer::tokenize(const QString &text, MarkdownTokenizer::TokenState current, MarkdownTokenizer::TokenState previous, MarkdownTokenizer::TokenState next)
{
    m_current = current;
    m_previous = previous;
    m_next = next;

    if (m_previous == TokenState::GFMCodeFence && tokenizeCodeBlock(text)) {
        // Nothing to do.
    } else if (m_previous != TokenState::Comment && paragraphBreakRegex.exactMatch(text)) {
        // Let's look for a list first
        if (m_previous == TokenState::ListLineBreak ||
                m_previous == TokenState::NumList ||
                m_previous == TokenState::BulletList) {
            setState(TokenState::ListLineBreak);
        } else if (m_previous != TokenState::CodeBlock ||
                   (!text.startsWith(QChar('\t')) && !text.endsWith("    "))) {
            setState(TokenState::ParagraphBreak);
        }
    } else if (tokenizeSetextHeadingLine2(text)
               || tokenizeCodeBlock(text)
               || tokenizeMultilineComment(text)
               || tokenizeHorizontalRule(text)
               || tokenizeTableDivider(text)) {
        // DO NOTHING
    } else if (tokenizeAtxHeading(text)
               || tokenizeSetextHeadingLine1(text)
               || tokenizeBlockquote(text)
               || tokenizeNumberedList(text)
               || tokenizeBulletPointList(text)) {
        tokenizeInline(text);
    } else {
        if (m_previous == TokenState::ListLineBreak ||
                m_previous == TokenState::NumList ||
                m_previous == TokenState::BulletList) {

            if (!tokenizeNumberedList(text) &&
                    !tokenizeBulletPointList(text) &&
                    (text.startsWith(QChar('\t')) || text.startsWith("    "))) {
                setState(m_previous);
            } else {
                setState(TokenState::Paragraph);
            }

        } else {
            setState(TokenState::Paragraph);
        }
        tokenizeInline(text);
    }

    const bool reprocess1 = (m_previous == TokenState::SetextHead1Line1 && m_state != TokenState::SetextHead1Line2);
    const bool reprocess2 = (m_previous == TokenState::SetextHead2Line1 && m_state != TokenState::SetextHead2Line2);
    if (reprocess1 || reprocess2) {
        setStartEarlier(true);
    } else {
        setStartEarlier(false);
    }
}

MarkdownTokenizer::TokenPositionMap MarkdownTokenizer::tokens() const
{
    return m_tokens;
}

MarkdownTokenizer::TokenState MarkdownTokenizer::state() const
{
    return m_state;
}

bool MarkdownTokenizer::shouldBackTrack() const
{
    return m_startEarlier;
}

void MarkdownTokenizer::clear()
{
    m_tokens.clear();
    m_startEarlier = false;
    m_state = TokenState::Unknown;
}

void MarkdownTokenizer::addToken(const MarkdownToken &token)
{
    m_tokens.insertMulti(token.position(), token);
}

bool MarkdownTokenizer::tokenizeSetextHeadingLine1(const QString &text)
{
    int level = 0;
    MarkdownToken token;

    if (m_next == TokenState::SetextHead1Line2) {
        level = 1;
        setState(TokenState::SetextHead1Line1);
        token.setType(MarkdownToken::SetextHead1Line1);
    }
    else if (m_next == TokenState::SetextHead2Line2) {
        level = 2;
        setState(TokenState::SetextHead2Line1);
        token.setType(MarkdownToken::SetextHead2Line1);
    }

    if (level > 0)
    {
        token.setLength(text.length());
        token.setPosition(0);
        addToken(token);
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeSetextHeadingLine2(const QString &text)
{

    return false;
}

bool MarkdownTokenizer::tokenizeAtxHeading(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeNumberedList(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeBulletPointList(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeHorizontalRule(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeBlockquote(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeCodeBlock(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeMultilineComment(const QString &text)
{
    return false;
}

bool MarkdownTokenizer::tokenizeInline(const QString &text)
{
    return false;
}

void MarkdownTokenizer::tokenizeVerbatim(QString &text)
{

}

void MarkdownTokenizer::tokenizeHtmlComments(QString &text)
{

}

void MarkdownTokenizer::tokenizeTableHeaderRow(QString &text)
{

}

bool MarkdownTokenizer::tokenizeTableDivider(const QString &text)
{
    return false;
}

void MarkdownTokenizer::tokenizeTableRow(QString &text)
{

}
