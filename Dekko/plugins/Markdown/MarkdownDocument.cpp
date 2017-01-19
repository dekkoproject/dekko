#include "MarkdownDocument.h"
#include <QDebug>
#include <QTextFrame>

MarkdownDocument::MarkdownDocument(QQuickItem *parent) : QQuickItem(parent),
    m_textDocument(Q_NULLPTR),
    m_enabled(false),
    m_autoMatchEnabled(false),
    m_cycleBulletMarker(true),
    m_enableLargeHeadingSizes(false),
    m_useUnderlineForEmp(false),
    m_spacesForTabs(false),
    m_tabWidth(0),
    m_paperMargins(0),
    m_hasSelection(false),
    m_selectionStart(0),
    m_selectionEnd(0),
    m_highlighter(Q_NULLPTR),
    m_cursorPosition(0)
{
    setEnabled(true);
    setVisible(true);
    connect(this, &MarkdownDocument::textDocumentChanged, this, &MarkdownDocument::onDocumentChanged);

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
    if (m_hasSelection) {
        QTextBlock start = document()->findBlock(m_selectionStart);
        QTextBlock end = document()->findBlock(m_selectionEnd).next();
        m_cursor.beginEditBlock();

        while (start != end) {
            setCursorPosition(start.position());
            if (m_spacesForTabs) {
                QString indentText = "";
                for (int i = 0; i < m_tabWidth; ++i) {
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
        int indent = m_tabWidth;
        QString indentText = "";
        m_cursor.beginEditBlock();

        switch (m_cursor.block().userState()) {
        // TODO:
        default:
            indent = m_tabWidth - (m_cursor.positionInBlock() % m_tabWidth);
            break;
        }

        if (m_spacesForTabs) {
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
            while (document()->characterAt(m_cursor.position()) == QChar(' ') && p < m_tabWidth) {
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
    if (document() != Q_NULLPTR) {
        connect(document(), &QTextDocument::contentsChange, this, &MarkdownDocument::onContentsChange);
        m_cursor = textCursor();
        m_highlighter = new MarkdownHighlighter(document());
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
        } else if (m_autoMatchEnabled && m_matches.value(c)) {
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
    if (m_autoMatchEnabled && !m_cursor.hasSelection()) {
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
                handleCLRF();
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

void MarkdownDocument::handleCLRF()
{
    QString autoInsertText = "";
    bool endList = false;
    if (m_cursor.positionInBlock() < (m_cursor.block().length() - 1)) {

    }

    m_cursor.insertText(QStringLiteral("\n") + autoInsertText);
}
