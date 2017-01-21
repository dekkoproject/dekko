#include "MarkdownDocument.h"
#include <QDebug>
#include <QTextFrame>

MarkdownDocument::MarkdownDocument(QQuickItem *parent) : QQuickItem(parent),
    m_options(Q_NULLPTR),
    m_textDocument(Q_NULLPTR),
    m_enabled(false),
    m_hasSelection(false),
    m_selectionStart(0),
    m_selectionEnd(0),
    m_highlighter(Q_NULLPTR),
    m_cursorPosition(0)
{
    setEnabled(true);
    setVisible(true);
    connect(this, &MarkdownDocument::textDocumentChanged, this, &MarkdownDocument::onDocumentChanged);
    connect(this, &MarkdownDocument::optionsChanged, this, &MarkdownDocument::onDocumentChanged);
    m_blockQuote.setPattern("^ {0,3}(>\\s*)+");
    m_numList.setPattern("^\\s*([0-9]+)[.)]\\s+");
    m_bulletList.setPattern("^\\s*[+*-]\\s+");
    m_taskList.setPattern("^\\s*[-] \\[([x ])\\]\\s+");

    m_pairs.insert('"', '"');
    m_pairs.insert('\'', '\'');
    m_pairs.insert('(', ')');
    m_pairs.insert('[', ']');
    m_pairs.insert('{', '}');
    m_pairs.insert('*', '*');
    m_pairs.insert('_', '_');
    m_pairs.insert('`', '`');
    m_pairs.insert('<', '>');

    m_matches.insert('"', true);
    m_matches.insert('\'', true);
    m_matches.insert('(', true);
    m_matches.insert('[', true);
    m_matches.insert('{', true);
    m_matches.insert('*', true);
    m_matches.insert('_', true);
    m_matches.insert('`', true);
    m_matches.insert('<', true);
}

int MarkdownDocument::cursorPosition() const
{
    return m_cursor.position();
}

void MarkdownDocument::indentText()
{

    const int tabWidth = m_options->get_tabWidth();
    if (m_hasSelection) {
        QTextBlock start = document()->findBlock(m_selectionStart);
        QTextBlock end = document()->findBlock(m_selectionEnd).next();
        m_cursor.beginEditBlock();

        while (start != end) {
            setCursorPosition(start.position());
            if (m_options->get_spacesForTabs()) {
                QString indentText = "";
                for (int i = 0; i < tabWidth; ++i) {
                    indentText += QStringLiteral(" ");
                }
                m_cursor.insertText(indentText);
            } else {
                m_cursor.insertText("\t");
            }
            start = start.next();
        }
        m_cursor.endEditBlock();
    } else {
        int indent = tabWidth;
        QString indentText = "";
        m_cursor.beginEditBlock();

        switch (m_cursor.block().userState()) {
        // TODO:
        default:
            indent = tabWidth - (m_cursor.positionInBlock() % tabWidth);
            break;
        }

        if (m_options->get_spacesForTabs()) {
            for (int i = 0; i < indent; ++i) {
                indentText += QStringLiteral(" ");
            }
        } else {
            indentText = QStringLiteral("\t");
        }
        m_cursor.insertText(indentText);
        m_cursor.endEditBlock();
    }
    setCursorPosition(m_cursor.position());
}

void MarkdownDocument::unindentText()
{
    QTextBlock start;
    QTextBlock end;

    if (m_hasSelection) {
        start = document()->findBlock(m_selectionStart);
        end = document()->findBlock(m_selectionEnd);
    } else {
        start = m_cursor.block();
        end = start.next();
    }

    m_cursor.beginEditBlock();

    while (start != end) {
        setCursorPosition(start.position());
        if (document()->characterAt(m_cursor.position()) == QChar('\t')) {
            m_cursor.deleteChar();
        } else {
            int p = 0;
            while (document()->characterAt(m_cursor.position()) == QChar(' ') && p < m_options->get_tabWidth()) {
                ++p;
                m_cursor.deleteChar();
            }
        }
        start = start.next();
    }
    m_cursor.endEditBlock();
    setCursorPosition(m_cursor.position());
}

void MarkdownDocument::setCursorPosition(int cursorPosition)
{
    if (m_cursorPosition == cursorPosition)
        return;

    m_cursorPosition = cursorPosition;
    m_cursor.setPosition(cursorPosition);
    emit cursorPositionChanged(cursorPosition);
}

QTextDocument *MarkdownDocument::document() const
{
    if (m_textDocument != Q_NULLPTR) {
        return m_textDocument->textDocument();
    }
    return Q_NULLPTR;
}

void MarkdownDocument::onDocumentChanged()
{
    if (document() != Q_NULLPTR && m_options != Q_NULLPTR) {
        connect(document(), &QTextDocument::contentsChange, this, &MarkdownDocument::onContentsChange);
        m_cursor = textCursor();
        m_highlighter = new MarkdownHighlighter(document(), m_options);
    }
}

void MarkdownDocument::onContentsChange(const int &pos, const int &rm, const int &add)
{
    Q_UNUSED(pos);
    Q_UNUSED(rm);
    Q_UNUSED(add);
    //    qDebug() << "CONTENTS CHANGED";
}

QTextCursor MarkdownDocument::textCursor()
{
    return document()->rootFrame()->firstCursorPosition();
}

bool MarkdownDocument::insertPair(const QChar &c)
{
    if (m_pairs.contains(c)) {
        QChar p = m_pairs.value(c);
        QTextBlock block;
        QTextBlock end;
        if (m_hasSelection) {
            block = document()->findBlock(m_selectionStart);
            end = document()->findBlock(m_selectionEnd);
            if (block == end) {
                auto s = m_selectionStart;
                auto e = m_selectionEnd;
                m_cursor.beginEditBlock();
                setCursorPosition(s);
                m_cursor.insertText(c);
                setCursorPosition(e + 1);
                m_cursor.insertText(p);
                setCursorPosition(s);
                setCursorPosition(e);
                m_cursor.endEditBlock();
                return true;
            }
        } else if (m_options->get_autoMatchEnabled() && m_matches.value(c)) {
            m_cursor.insertText(c);
            m_cursor.insertText(p);
            m_cursor.movePosition(QTextCursor::PreviousCharacter);
            setCursorPosition(m_cursor.position());
            return true;
        }
    }
    return false;
}

bool MarkdownDocument::endPairHandled(const QChar &c)
{
    bool lookAhead = false;
    if (m_options->get_autoMatchEnabled() && !m_cursor.hasSelection()) {
        if (m_pairs.values().contains(c)) {
            auto key = m_pairs.key(c);
            if (m_matches.value(key)) {
                lookAhead = true;
            }
        }
    }

    if (lookAhead) {
        auto text = m_cursor.block().text();
        auto pos = m_cursor.positionInBlock();
        if (pos < text.length()) {
            if (text[pos] == c) {
                m_cursor.movePosition(QTextCursor::NextCharacter);
                return true;
            }
        }
    }
    return false;
}

void MarkdownDocument::keyPressEvent(QKeyEvent *event)
{
    //    qDebug() << "KEY PRESS EVENT: " << event;
    int key = event->key();
    bool accepted = false;
    switch (key) {
    case Qt::Key_Return:
        if (!m_hasSelection) {
            if (event->modifiers() & Qt::ShiftModifier) {
                m_cursor.insertText("  ");
            }

            if (event->modifiers() & Qt::ControlModifier) {
                m_cursor.insertText("\n");
            } else {
                handleCRLF();
            }
            accepted = true;
        }
        break;
    case Qt::Key_Tab:
        indentText();
        accepted = true;
        break;
    case Qt::Key_Backtab:
        unindentText();
        accepted = true;
        break;
    case Qt::Key_Backspace:
        accepted = handleBackspace();
        break;
    default:
        if (event->text().size() == 1) {
            auto c = event->text().at(0);
            if (!endPairHandled(c) && !insertPair(c)) {
                accepted = false;
            } else {
                accepted = true;
            }
        }
        break;
    }
    event->setAccepted(accepted);
}

void MarkdownDocument::handleCRLF()
{
    QString autoInsertText = "";
    bool endList = false;
    if (m_cursor.positionInBlock() < (m_cursor.block().length() - 1)) {
        autoInsertText = getPreviousIndentation();
        if (m_cursor.positionInBlock() < autoInsertText.length()) {
            autoInsertText.truncate(m_cursor.positionInBlock());
        }
    } else {
        switch (m_cursor.block().userState()) {
        case MarkdownTokenizer::TokenState::NumList:
        {
            autoInsertText = getBlockStart(m_numList);
            QStringList c = m_numList.capturedTexts();
            if (!autoInsertText.isEmpty() && c.size() == 2) {
                if (m_cursor.block().text().length() == autoInsertText.length()) {
                    endList = true;
                } else {
                    QRegExp num("\\d+");
                    int liNum = c.at(1).toInt();
                    liNum++;
                    autoInsertText = autoInsertText.replace(num, QString("%1").arg(liNum));
                }
            } else {
                autoInsertText = getPreviousIndentation();
            }
            break;
        }
        case MarkdownTokenizer::TokenState::BulletList:
        {
            autoInsertText = getBlockStart(m_taskList);
            if (autoInsertText.isEmpty()) {
                autoInsertText = getBlockStart(m_bulletList);
                if (autoInsertText.isEmpty()) {
                    autoInsertText = getPreviousIndentation();
                } else if (m_cursor.block().text().length() == autoInsertText.length()) {
                    endList = true;
                }
            } else {
                if (m_cursor.block().text().length() == autoInsertText.length()) {
                    endList = true;
                } else {
                    autoInsertText = autoInsertText.replace('x', ' ');
                }
            }
            break;
        }
        case MarkdownTokenizer::TokenState::Blockquote:
        {
            autoInsertText = getBlockStart(m_blockQuote);
            break;
        }
        default:
            autoInsertText = getPreviousIndentation();
            break;
        }
    }

    if (endList)
    {
        autoInsertText = getPreviousIndentation();
        m_cursor.movePosition(QTextCursor::StartOfBlock);
        m_cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        setCursorPosition(m_cursor.position());
        m_cursor.insertText(autoInsertText);
        autoInsertText = "";
    }

    m_cursor.insertText(QStringLiteral("\n") + autoInsertText);
}

bool MarkdownDocument::handleBackspace()
{
    if (m_hasSelection) {
        return false;
    }

    int backtrackIndex = -1;

    switch (m_cursor.block().userState())
    {
    case MarkdownTokenizer::TokenState::NumList:
    {
        if (m_numList.exactMatch(m_cursor.block().text()))
        {
            backtrackIndex = m_cursor.block().text().indexOf(QRegExp("\\d"));
        }
        break;
    }
    case MarkdownTokenizer::TokenState::BulletList:
    {
        if( m_bulletList.exactMatch(m_cursor.block().text())
            || m_taskList.exactMatch(m_cursor.block().text())) {

            backtrackIndex = m_cursor.block().text().indexOf(QRegExp("[+*-]"));
        }
        break;
    }
    case MarkdownTokenizer::TokenState::Blockquote:
    {
        if (m_blockQuote.exactMatch(m_cursor.block().text()))
        {
            backtrackIndex = m_cursor.block().text().lastIndexOf('>');
        }
        break;
    }
    default:
        // If the first character in an automatched set is being
        // deleted, then delete the second matching one along with it.
        //
        if (m_options->get_autoMatchEnabled() && (m_cursor.positionInBlock() > 0))
        {
            QString blockText = m_cursor.block().text();

            if (m_cursor.positionInBlock() < blockText.length())
            {
                QChar currentChar = blockText[m_cursor.positionInBlock()];
                QChar previousChar = blockText[m_cursor.positionInBlock() - 1];

                if (m_pairs.value(previousChar) == currentChar)
                {
                    m_cursor.movePosition(QTextCursor::Left);
                    m_cursor.movePosition
                            (
                                QTextCursor::Right,
                                QTextCursor::KeepAnchor,
                                2
                                );
                    setCursorPosition(m_cursor.position());
                    m_cursor.removeSelectedText();
                    return true;
                }
            }
        }
        break;
    }

    if (backtrackIndex >= 0)
    {
        m_cursor.movePosition(QTextCursor::StartOfBlock);
        m_cursor.movePosition
                (
                    QTextCursor::Right,
                    QTextCursor::MoveAnchor,
                    backtrackIndex
                    );

        m_cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        setCursorPosition(m_cursor.position());
        m_cursor.removeSelectedText();
        return true;
    }

    return false;
}

QString MarkdownDocument::getBlockStart(QRegExp &regexp)
{
    QTextBlock block = m_cursor.block();

    QString text = block.text();

    if (regexp.indexIn(text, 0) >= 0)
    {
        return text.left(regexp.matchedLength());
    }

    return QString("");
}

QString MarkdownDocument::getPreviousIndentation()
{
    QString indent = "";
    QTextBlock block = m_cursor.block();

    QString text = block.text();

    for (int i = 0; i < text.length(); i++)
    {
        if (text[i].isSpace())
        {
            indent += text[i];
        }
        else
        {
            return indent;
        }
    }

    return indent;
}
