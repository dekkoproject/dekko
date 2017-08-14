import QtQuick 2.4
import Dekko.Python 1.0
import Dekko.Mail.API 1.0

Item {

    signal rendered(string html)
    signal error(string error)

    function render(body, signature) {
        if (body) {
            py.render(body, signature)
        }
    }

    Python {
        id: py

        importPaths: [Qt.resolvedUrl("./")]

        function render(html ,sig) {
            importModule("markdownutils", function() {
                py.call("markdownutils.render_template", [html, sig], function(result){
                    Log.logInfo("MarkdownRenderer::render_template", "Rendered")
                    rendered(result)
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
