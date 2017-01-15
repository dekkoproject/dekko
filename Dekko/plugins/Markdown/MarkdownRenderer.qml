import QtQuick 2.4
import Dekko.Python 1.0
import Dekko.Mail.API 1.0

Item {

    signal error(string error)
    signal rendered(string html)

    function render(text) {
        if (text) {
            py.renderMarkdown(text, options)
        }
    }

    function renderNoExtensions(text) {
        if (text) {
            py.renderMarkdownBasic(text)
        }
    }

    MarkdownOptions {
        id: options
    }

    Python {
        id: py

        importPaths: [Qt.resolvedUrl("./")]

        function renderMarkdown(text, extras) {
            importModule("markdownutils", function() {
                py.call("markdownutils.render_markdown_with_opts", [text, extras], function(html){
                    Log.logInfo("MarkdownRenderer::render_markdown_with_opts", "Rendered")
                    rendered(html)
                })
            })
        }

        function renderMarkdownBasic(text) {
            importModule("markdownutils", function() {
                py.call("markdownutils.render_markdown", [text], function(html){
                    Log.logInfo("MarkdownRenderer::render_markdown", "Rendered")
                    rendered(html)
                })
            })
        }

        onReady: {
            setHandler('log', function (msg) {
                Log.logInfo("MarkdownRenderer::pyLog", msg)
            })
        }

        onError: {
            error(traceback)
            Log.logError("Python Error", traceback)
        }
    }
}
