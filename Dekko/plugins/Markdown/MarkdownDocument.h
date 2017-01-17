#ifndef MARKDOWNDOCUMENT_H
#define MARKDOWNDOCUMENT_H

#include <QObject>
#include <QQuickTextDocument>
#include <QQmlAutoPropertyHelpers.h>

class MarkdownDocument : public QObject
{
    Q_OBJECT
    QML_WRITABLE_AUTO_PROPERTY(bool, enabled)
    QML_WRITABLE_AUTO_PROPERTY(QQuickTextDocument*, document)
public:
    explicit MarkdownDocument(QObject *parent = 0);

signals:

public slots:
};

#endif // MARKDOWNDOCUMENT_H
