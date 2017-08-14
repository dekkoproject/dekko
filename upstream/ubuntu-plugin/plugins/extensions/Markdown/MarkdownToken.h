#ifndef MARKDOWNTOKEN_H
#define MARKDOWNTOKEN_H

class MarkdownToken
{
public:
    MarkdownToken();
    ~MarkdownToken();

    enum TokenType {
        Unknown = -1,
        AtxHeading1,
        AtxHeading2,
        AtxHeading3,
        AtxHeading4,
        AtxHeading5,
        AtxHeading6,
        SetextHead1Line1,
        SetextHead1Line2,
        SetextHead2Line1,
        SetextHead2Line2,
        Emphasis,
        Strong,
        Strikethrough,
        Verbatim,
        HtmlTag,
        HtmlEntity,
        AutomaticLink,
        InlineLink,
        ReferenceLink,
        ReferenceDefinition,
        Image,
        HtmlComment,
        NumberedList,
        BulletList,
        HorizontalRule,
        Blockquote,
        CodeBlock,
        GFMCodeFence,
        CodeFenceEnd,
        Mention,
        TableHeader,
        TableDiv,
        TablePipe,
        Last
    };

    TokenType type() const;
    void setType(TokenType t);

    int position() const;
    void setPosition(int pos) { m_position = pos; }

    int length() const;
    void setLength(int len);

    int openingLength() const;
    void setOpeningLenth(int len);

    int closingLength() const;
    void setClosingLength(int len);

    const bool operator < (const MarkdownToken &t) const {
        return m_position < t.position();
    }

    const bool operator > (const MarkdownToken &t) const {
        return m_position > t.position();
    }

private:
    TokenType m_type;
    int m_position;
    int m_length;
    int m_opening;
    int m_closing;
};

#endif // MARKDOWNTOKEN_H
