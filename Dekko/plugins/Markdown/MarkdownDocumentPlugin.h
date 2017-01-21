#ifndef MARKDOWNDOCUMENTPLUGIN_H
#define MARKDOWNDOCUMENTPLUGIN_H
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>

class MarkdownDocumentPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // MARKDOWNDOCUMENTPLUGIN_H
