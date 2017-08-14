#include "MarkdownTokenizer.h"

// This character is used to replace escape characters and other characters
// with special meaning in a dummy copy of the current line being parsed,
// for ease of parsing.
//
static const QChar DUMMY_CHAR('$');

static const int MAX_MARKDOWN_HEADING_LEVEL = 6;

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

MarkdownTokenizer::~MarkdownTokenizer()
{

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

void MarkdownTokenizer::tokenizeMatches(MarkdownToken::TokenType tokenType, QString &text, QRegExp &regex, const int markupStartCount, const int markupEndCount, const bool replaceMarkupChars, const bool replaceAllChars)
{
    int index = text.indexOf(regex);

    while (index >= 0)
    {
        int length = regex.matchedLength();
        MarkdownToken token;

        token.setType(tokenType);
        token.setPosition(index);
        token.setLength(length);

        if (markupStartCount > 0)
        {
            token.setOpeningLenth(markupStartCount);
        }

        if (markupEndCount > 0)
        {
            token.setClosingLength(markupEndCount);
        }

        if (replaceAllChars)
        {
            for (int i = index; i < (index + length); i++)
            {
                text[i] = DUMMY_CHAR;
            }
        }
        else if (replaceMarkupChars)
        {
            for (int i = index; i < (index + markupStartCount); i++)
            {
                text[i] = DUMMY_CHAR;
            }

            for (int i = (index + length - markupEndCount); i < (index + length); i++)
            {
                text[i] = DUMMY_CHAR;
            }
        }

        addToken(token);
        index = text.indexOf(regex, index + length);
    }
}

void MarkdownTokenizer::addToken(const MarkdownToken &token)
{
    m_tokens.insertMulti(token.position(), token);
}

void MarkdownTokenizer::setState(MarkdownTokenizer::TokenState state)
{
    m_state = state;
}

void MarkdownTokenizer::setStartEarlier(const bool startEarlier)
{
    m_startEarlier = startEarlier;
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
    int level = 0;
    bool isMatch = false;
    MarkdownToken token;

    if (m_previous == TokenState::SetextHead1Line1) {
        level = 1;
        isMatch = heading1SetextRegex.exactMatch(text);
        setState(TokenState::SetextHead1Line2);
        token.setType(MarkdownToken::SetextHead1Line2);
    } else if (m_previous == TokenState::SetextHead2Line1) {
        level = 2;
        isMatch = heading2SetextRegex.exactMatch(text);
        setState(TokenState::SetextHead2Line2);
        token.setType(MarkdownToken::SetextHead2Line2);
    } else if (m_previous == TokenState::Paragraph) {
        bool h1Line2 = heading1SetextRegex.exactMatch(text);
        bool h2Line2 = heading2SetextRegex.exactMatch(text);

        if (h1Line2 || h2Line2) {
            setStartEarlier(true);
            token.setLength(text.length());
            token.setPosition(0);

            if (h1Line2) {
                setState(TokenState::SetextHead1Line2);
                token.setType(MarkdownToken::SetextHead1Line2);
            } else {
                setState(TokenState::SetextHead2Line2);
                token.setType(MarkdownToken::SetextHead2Line2);
            }
            addToken(token);
            return true;
        }
    }

    if (level > 0) {
        if (isMatch) {
            token.setLength(text.length());
            token.setPosition(0);
            addToken(token);
            return true;
        } else {
            setStartEarlier(true);
            return false;
        }
    }
    return false;
}

namespace {

QString dummyOutEscapeCharacters(const QString& text)
{
    bool escape = false;
    QString escapedText = text;

    for (int i = 0; i < text.length(); i++)
    {
        if (escape)
        {
            escapedText[i] = DUMMY_CHAR; // Use a dummy character.
            escape = false;
        }
        else if (QChar('\\') == text[i])
        {
            escape = true;
        }
    }

    return escapedText;
}

}

bool MarkdownTokenizer::tokenizeAtxHeading(const QString &text)
{
    QString escapedText = dummyOutEscapeCharacters(text);
    int trailingPoundCount = 0;

    int level = 0;

    // Count the number of pound signs at the front of the string,
    // up to the maximum allowed, to determine the heading level.
    //
    for
            (
             int i = 0;
             ((i < escapedText.length()) && (i < MAX_MARKDOWN_HEADING_LEVEL));
             i++
             )
    {
        if (QChar('#') == escapedText[i])
        {
            level++;
        }
        else
        {
            // We're done counting, as no more pound signs are available.
            break;
        }
    }

    if ((level > 0) && (level < text.length()))
    {
        // Count how many pound signs are at the end of the text.
        for (int i = escapedText.length() - 1; i > level; i--)
        {
            if (QChar('#') == escapedText[i])
            {
                trailingPoundCount++;
            }
            else
            {
                // We're done counting, as no more pound signs are available.
                break;
            }
        }

        MarkdownToken token;
        token.setPosition(0);
        token.setLength(text.length());
        token.setType((MarkdownToken::TokenType) (MarkdownToken::AtxHeading1 + level - 1));
        token.setOpeningLenth(level);
        token.setClosingLength(trailingPoundCount);
        addToken(token);
        setState((TokenState)(TokenState::AtxHeading1 + level - 1));
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeNumberedList(const QString &text)
{
    if ((m_previous == TokenState::ParagraphBreak ||
         m_previous == TokenState::Unknown ||
         m_previous == TokenState::CodeBlock ||
         m_previous == TokenState::CodeFenceEnd &&
         numberedListRegex.exactMatch(text)) ||
        (m_previous == TokenState::ListLineBreak ||
         m_previous == TokenState::NumList ||
         m_previous == TokenState::BulletList &&
         numberedNestedListRegex.exactMatch(text))) {

        int periodIndex = text.indexOf(QChar('.'));
        int parenthIndex = text.indexOf(QChar(')'));
        int index = -1;

        if (periodIndex < 0)
        {
            index = parenthIndex;
        }
        else if (parenthIndex < 0)
        {
            index = periodIndex;
        }
        else if (parenthIndex > periodIndex)
        {
            index = periodIndex;
        }
        else
        {
            index = parenthIndex;
        }

        if (index >= 0)
        {
            MarkdownToken token;
            token.setType(MarkdownToken::NumberedList);
            token.setPosition(0);
            token.setLength(text.length());
            token.setOpeningLenth(index + 1);
            addToken(token);
            setState(TokenState::NumList);
            return true;
        }

        return false;
    }
    return false;
}

bool MarkdownTokenizer::tokenizeBulletPointList(const QString &text)
{
    bool foundBulletChar = false;
    int bulletCharIndex = -1;
    int spaceCount = 0;
    bool whitespaceFoundAfterBulletChar = false;

    if (m_previous != TokenState::Unknown &&
            m_previous != TokenState::ParagraphBreak &&
            m_previous != TokenState::ListLineBreak &&
            m_previous != TokenState::NumList &&
            m_previous != TokenState::BulletList &&
            m_previous != TokenState::CodeBlock &&
            m_previous != TokenState::CodeFenceEnd) {
        return false;
    }

    // Search for the bullet point character, which can
    // be either a '+', '-', or '*'.
    //
    for (int i = 0; i < text.length(); i++)
    {
        if (QChar(' ') == text[i])
        {
            if (foundBulletChar)
            {
                // We've confirmed it's a bullet point by the whitespace that
                // follows the bullet point character, and can now exit the
                // loop.
                //
                whitespaceFoundAfterBulletChar = true;
                break;
            }
            else
            {
                spaceCount++;

                // If this list item is the first in the list, ensure the
                // number of spaces preceeding the bullet point does not
                // exceed three, as that would indicate a code block rather
                // than a bullet point list.
                //
                if
                        (
                         (spaceCount > 3)
                         && (m_previous != TokenState::NumList)
                         && (m_previous != TokenState::BulletList)
                         && (m_previous != TokenState::ListLineBreak)
                         &&
                         (
                             (m_previous == TokenState::ParagraphBreak)
                             || (m_previous == TokenState::Unknown)
                             || (m_previous == TokenState::CodeBlock)
                             || (m_previous == TokenState::CodeFenceEnd)
                             )
                         )
                {
                    return false;
                }
            }
        }
        else if (QChar('\t') == text[i])
        {
            if (foundBulletChar)
            {
                // We've confirmed it's a bullet point by the whitespace that
                // follows the bullet point character, and can now exit the
                // loop.
                //
                whitespaceFoundAfterBulletChar = true;
                break;
            }
            else if
                    (
                     (m_previous == TokenState::ParagraphBreak)
                     || (m_previous == TokenState::Unknown)
                     )
            {
                // If this list item is the first in the list, ensure that
                // no tab character preceedes the bullet point, as that would
                // indicate a code block rather than a bullet point list.
                //
                return false;
            }
        }
        else if
                (
                 (QChar('+') == text[i])
                 || (QChar('-') == text[i])
                 || (QChar('*') == text[i])
                 )
        {
            foundBulletChar = true;
            bulletCharIndex = i;
        }
        else
        {
            return false;
        }
    }

    if ((bulletCharIndex >= 0) && whitespaceFoundAfterBulletChar)
    {
        MarkdownToken token;
        token.setType(MarkdownToken::BulletList);
        token.setPosition(0);
        token.setLength(text.length());
        token.setOpeningLenth(bulletCharIndex + 1);
        addToken(token);
        setState(TokenState::BulletList);
        return true;
    }
    return false;
}

bool MarkdownTokenizer::tokenizeHorizontalRule(const QString &text)
{
    if (hruleRegex.exactMatch(text))
    {
        MarkdownToken token;
        token.setType(MarkdownToken::HorizontalRule);
        token.setPosition(0);
        token.setLength(text.length());
        addToken(token);
        setState(TokenState::HorizontalRule);
        return true;
    }
    return false;
}

bool MarkdownTokenizer::tokenizeBlockquote(const QString &text)
{
    if (m_previous == TokenState::Blockquote || blockquoteRegex.exactMatch(text)) {
        // Find any '>' characters at the front of the line.
        int markupLength = 0;

        for (int i = 0; i < text.length(); i++)
        {
            if (QChar('>') == text[i])
            {
                markupLength = i + 1;
            }
            else if (!text[i].isSpace())
            {
                // There are no more '>' characters at the front of the line,
                // so stop processing.
                //
                break;
            }
        }

        MarkdownToken token;
        token.setType(MarkdownToken::Blockquote);
        token.setPosition(0);
        token.setLength(text.length());

        if (markupLength > 0)
        {
            token.setOpeningLenth(markupLength);
        }

        addToken(token);
        setState(TokenState::Blockquote);
        return true;
    }
    return false;
}

bool MarkdownTokenizer::tokenizeCodeBlock(const QString &text)
{
    if (m_previous == TokenState::GFMCodeFence) {
        setState(m_previous);

        if (m_previous == TokenState::GFMCodeFence && githubCodeFenceEndRegex.exactMatch(text)) {
            MarkdownToken token;
            token.setType(MarkdownToken::CodeFenceEnd);
            token.setPosition(0);
            token.setLength(text.length());
            addToken(token);
            setState(TokenState::CodeFenceEnd);
        }
        else
        {
            MarkdownToken token;
            token.setType(MarkdownToken::CodeBlock);
            token.setPosition(0);
            token.setLength(text.length());
            addToken(token);
        }

        return true;
    }
    else if ((m_previous == TokenState::CodeBlock ||
              m_previous == TokenState::ParagraphBreak ||
              m_previous == TokenState::Unknown)
             &&
              (text.startsWith(QChar('\t')) || text.startsWith("    "))) {

        MarkdownToken token;
        token.setType(MarkdownToken::CodeBlock);
        token.setPosition(0);
        token.setLength(text.length());
        addToken(token);
        setState(TokenState::CodeBlock);
        return true;
    }
    else if (m_previous == TokenState::ParagraphBreak ||
             m_previous == TokenState::Paragraph ||
             m_previous == TokenState::Unknown ||
             m_previous == TokenState::ListLineBreak) {
        bool foundCodeFenceStart = false;
        MarkdownToken token;

        if (githubCodeFenceStartRegex.exactMatch(text))
        {
            foundCodeFenceStart = true;
            token.setType(MarkdownToken::GFMCodeFence);
            setState(TokenState::GFMCodeFence);
        }

        if (foundCodeFenceStart)
        {
            token.setPosition(0);
            token.setLength(text.length());
            addToken(token);
            return true;
        }
    }
    return false;
}

bool MarkdownTokenizer::tokenizeMultilineComment(const QString &text)
{
    if (m_previous == TokenState::Comment) {
        // Find the end of the comment, if any.
        int index = text.indexOf("-->");
        MarkdownToken token;
        token.setType(MarkdownToken::HtmlComment);
        token.setPosition(0);

        if (index >= 0)
        {
            token.setLength(index + 3);
            addToken(token);

            // Return false so that the rest of the line that isn't within
            // the commented segment can be highlighted as normal paragraph
            // text.
            //
            return false;
        }
        else
        {
            token.setLength(text.length());
            addToken(token);
            setState(TokenState::Comment);
            return true;
        }
    }
    return false;
}

bool MarkdownTokenizer::tokenizeInline(const QString &text)
{
    QString escapedText = dummyOutEscapeCharacters(text);

    // Check if the line is a reference definition.
    if (referenceDefinitionRegex.exactMatch(escapedText))
    {
        int colonIndex = escapedText.indexOf(':');
        MarkdownToken token;
        token.setType(MarkdownToken::ReferenceDefinition);
        token.setPosition(0);
        token.setLength(colonIndex + 1);
        addToken(token);

        // Replace the first bracket so that the '[...]:' reference definition
        // start doesn't get highlighted as a reference link.
        //
        int firstBracketIndex = escapedText.indexOf(QChar('['));

        if (firstBracketIndex >= 0)
        {
            escapedText[firstBracketIndex] = DUMMY_CHAR;
        }
    }

    tokenizeVerbatim(escapedText);
    tokenizeHtmlComments(escapedText);
    tokenizeTableHeaderRow(escapedText);
    tokenizeTableRow(escapedText);
    tokenizeMatches(MarkdownToken::Image, escapedText, imageRegex, 0, 0, false, true);
    tokenizeMatches(MarkdownToken::InlineLink, escapedText, inlineLinkRegex, 0, 0, false, true);
    tokenizeMatches(MarkdownToken::ReferenceLink, escapedText, referenceLinkRegex, 0, 0, false, true);
    tokenizeMatches(MarkdownToken::HtmlEntity, escapedText, htmlEntityRegex);
    tokenizeMatches(MarkdownToken::AutomaticLink, escapedText, automaticLinkRegex, 0, 0, false, true);
    tokenizeMatches(MarkdownToken::Strikethrough, escapedText, strikethroughRegex, 2, 2);
    tokenizeMatches(MarkdownToken::Strong, escapedText, strongRegex, 2, 2, true);
    tokenizeMatches(MarkdownToken::Emphasis, escapedText, emphasisRegex, 1, 1, true);
    tokenizeMatches(MarkdownToken::HtmlTag, escapedText, htmlTagRegex);
    tokenizeMatches(MarkdownToken::Mention, escapedText, mentionRegex, 0, 0, false, true);
    return true;
}

void MarkdownTokenizer::tokenizeVerbatim(QString &text)
{
    int index = verbatimRegex.indexIn(text);

    while (index >= 0)
    {
        QString end = "";
        int count = verbatimRegex.matchedLength();

        // Search for the matching end, which should have the same number
        // of back ticks as the start.
        //
        for (int i = 0; i < count; i++)
        {
            end += '`';
        }

        int endIndex = text.indexOf(end, index + count);

        // If the end was found, add the verbatim token.
        if (endIndex >= 0)
        {
            MarkdownToken token;

            token.setType(MarkdownToken::Verbatim);
            token.setPosition(index);
            token.setLength(endIndex + count - index);
            token.setOpeningLenth(count);
            token.setClosingLength(count);
            addToken(token);

            // Fill out the token match in the string with the dummy
            // character so that searches for other Markdown elements
            // don't find anything within this token's range in the string.
            //
            for (int i = index; i < (index + token.length()); i++)
            {
                text[i] = DUMMY_CHAR;
            }

            index += token.length();
        }
        // Else start searching again at the very next character.
        else
        {
            index++;
        }

        index = verbatimRegex.indexIn(text, index);
    }
}

void MarkdownTokenizer::tokenizeHtmlComments(QString &text)
{
    // Check for the end of a multiline comment so that it doesn't get further
    // tokenized. Don't bother formatting the comment itself, however, because
    // it should have already been tokenized in tokenizeMultilineComment().
    //
    if (m_previous == TokenState::Comment) {
        int commentEnd = text.indexOf("-->");

        for (int i = 0; i < commentEnd + 3; i++) {
            text[i] = DUMMY_CHAR;
        }
    }

    // Now check for inline comments (non-multiline).
    int commentStart = text.indexOf(htmlInlineCommentRegex);

    while (commentStart >= 0)
    {
        int commentLength = htmlInlineCommentRegex.matchedLength();
        MarkdownToken token;

        token.setType(MarkdownToken::HtmlComment);
        token.setPosition(commentStart);
        token.setLength(commentLength);
        addToken(token);

        // Replace comment segment with dummy characters so that it doesn't
        // get tokenized again.
        //
        for (int i = commentStart; i < (commentStart + commentLength); i++) {
            text[i] = DUMMY_CHAR;
        }

        commentStart = text.indexOf(htmlInlineCommentRegex, commentStart + commentLength);
    }

    // Find multiline comment start, if any.
    commentStart = text.indexOf("<!--");

    if (commentStart >= 0)
    {
        MarkdownToken token;

        token.setType(MarkdownToken::HtmlComment);
        token.setPosition(commentStart);
        token.setLength(text.length() - commentStart);
        addToken(token);
        setState(TokenState::Comment);

        // Replace comment segment with dummy characters so that it doesn't
        // get tokenized again.
        //
        for (int i = commentStart; i < text.length(); i++)
        {
            text[i] = DUMMY_CHAR;
        }
    }
}

void MarkdownTokenizer::tokenizeTableHeaderRow(QString &text)
{
    if ((m_previous == TokenState::ParagraphBreak ||
         m_previous == TokenState::ListLineBreak ||
         m_previous == TokenState::SetextHead1Line2 ||
         m_previous == TokenState::SetextHead2Line2 ||
         m_previous == TokenState::AtxHeading1 ||
         m_previous == TokenState::AtxHeading2 ||
         m_previous == TokenState::AtxHeading3 ||
         m_previous == TokenState::AtxHeading4 ||
         m_previous == TokenState::AtxHeading5 ||
         m_previous == TokenState::AtxHeading6 ||
         m_previous == TokenState::HorizontalRule ||
         m_previous == TokenState::CodeFenceEnd ||
         m_previous == TokenState::Unknown)
         &&
        (m_state == TokenState::Paragraph ||
         m_state == TokenState::Unknown)
         &&
        (m_next == TokenState::TableDiv)) {

        setState(TokenState::TableHeader);

        int headerStart = 0;

        for (int i = 0; i < text.length(); i++)
        {
            if (QChar('|') == text[i])
            {
                // Replace pipe with space so that it doesn't get formatted
                // again with, for example, strong or emphasis formatting.
                // Note that we use a space rather than DUMMY_CHAR for this,
                // to prevent formatting such as strong and emphasis from
                // picking it up.
                //
                text[i] = ' ';

                MarkdownToken token;

                if (i > 0)
                {
                    token.setType(MarkdownToken::TableHeader);
                    token.setPosition(headerStart);
                    token.setLength(i - headerStart);
                    addToken(token);
                }

                token.setType(MarkdownToken::TablePipe);
                token.setPosition(i);
                token.setLength(1);
                addToken(token);
                headerStart = i + 1;
            }
        }

        if (headerStart < text.length())
        {
            MarkdownToken token;
            token.setType(MarkdownToken::TableHeader);
            token.setPosition(headerStart);
            token.setLength(text.length() - headerStart);
            addToken(token);
        }
    }
}

bool MarkdownTokenizer::tokenizeTableDivider(const QString &text)
{
    if (m_previous == TokenState::TableHeader)
    {
        if (pipeTableDividerRegex.exactMatch(text))
        {
            setState(TokenState::TableDiv);

            MarkdownToken token;
            token.setType(MarkdownToken::TableDiv);
            token.setLength(text.length());
            token.setPosition(0);
            addToken(token);

            return true;
        }
        else
        {
            // Restart tokenizing on the previous line.
            setStartEarlier(true);
        }
    }
    else if (m_previous == TokenState::Paragraph)
    {
        if (pipeTableDividerRegex.exactMatch(text))
        {
            // Restart tokenizing on the previous line.
            setStartEarlier(true);

            setState(TokenState::TableDiv);

            MarkdownToken token;
            token.setLength(text.length());
            token.setPosition(0);
            token.setType(MarkdownToken::TableDiv);
            addToken(token);
            return true;
        }
    }
    return false;
}

void MarkdownTokenizer::tokenizeTableRow(QString &text)
{
    if (m_previous == TokenState::TableDiv || m_previous == TokenState::TableRow) {

        setState(TokenState::TableRow);

        for (int i = 0; i < text.length(); i++)
        {
            if (QChar('|') == text[i])
            {
                // Replace pipe with space so that it doesn't get formatted
                // again with, for example, strong or emphasis formatting.
                // Note that we use a space rather than DUMMY_CHAR for this,
                // to prevent formatting such as strong and emphasis from
                // picking it up.
                //
                text[i] = ' ';

                MarkdownToken token;
                token.setType(MarkdownToken::TablePipe);
                token.setPosition(i);
                token.setLength(1);
                addToken(token);
            }
        }
    }
}
