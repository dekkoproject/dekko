#ifndef FORMATTINGOPTIONS_H
#define FORMATTINGOPTIONS_H

#include <QObject>
#include <QColor>
#include <QFont>
#include <QQmlAutoPropertyHelpers.h>

class FormattingOptions : public QObject
{
    Q_OBJECT
    QML_WRITABLE_AUTO_PROPERTY(int, fontSize)
    QML_WRITABLE_AUTO_PROPERTY(QColor, textColor)
    QML_WRITABLE_AUTO_PROPERTY(QColor, backgroundColor)
    QML_WRITABLE_AUTO_PROPERTY(QColor, markupColor)
    QML_WRITABLE_AUTO_PROPERTY(QColor, linkColor)
    QML_WRITABLE_AUTO_PROPERTY(QString, fontFamily)
    QML_WRITABLE_AUTO_PROPERTY(QFont::Weight, fontWeight)
    QML_WRITABLE_AUTO_PROPERTY(bool, autoMatchEnabled)
    QML_WRITABLE_AUTO_PROPERTY(bool, cycleBulletMarker)
    QML_WRITABLE_AUTO_PROPERTY(bool, enableLargeHeadingSizes)
    QML_WRITABLE_AUTO_PROPERTY(bool, useUnderlineForEmp)
    QML_WRITABLE_AUTO_PROPERTY(bool, spacesForTabs)
    QML_WRITABLE_AUTO_PROPERTY(int, tabWidth)
    QML_WRITABLE_AUTO_PROPERTY(int, paperMargins)

public:

    explicit FormattingOptions(QObject *parent = Q_NULLPTR) :
        QObject(parent),
        m_fontSize(12),
        m_textColor(Qt::darkGray),
        m_backgroundColor(Qt::white),
        m_markupColor(Qt::lightGray),
        m_linkColor(Qt::blue),
        m_fontFamily(QStringLiteral("Ubuntu")),
        m_fontWeight(QFont::Light),
        m_autoMatchEnabled(false),
        m_cycleBulletMarker(true),
        m_enableLargeHeadingSizes(false),
        m_useUnderlineForEmp(false),
        m_spacesForTabs(false),
        m_tabWidth(0),
        m_paperMargins(0)
    {
    }

};

#endif // FORMATTINGOPTIONS_H
