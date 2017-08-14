import QtQuick 2.4
import Dekko.Python 1.0
import Dekko.Mail.API 1.0

Item {

    signal handled(string text)
    signal error(string error)

    function handle(html) {
        if (html) {
            py.handleHtml(html)
        }
    }

    Python {
        id: py

        importPaths: [Qt.resolvedUrl("./")]

        function handleHtml(html) {
            importModule("markdownutils", function() {
                py.call("markdownutils.html_to_plaintext", [html], function(text){
                    Log.logInfo("MarkdownRenderer::html_to_plaintext", "Rendered")
                    handled(text)
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
