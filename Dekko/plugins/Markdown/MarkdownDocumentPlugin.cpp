#include "MarkdownDocumentPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "MarkdownDocument.h"

void MarkdownDocumentPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Markdown"));
    // @uri Dekko.Markdown
    qmlRegisterType<MarkdownDocument>(uri, 1, 0, "MarkdownDocument");
    qmlRegisterType<FormattingOptions>(uri, 1, 0, "FormattingOptions");
}

void MarkdownDocumentPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
