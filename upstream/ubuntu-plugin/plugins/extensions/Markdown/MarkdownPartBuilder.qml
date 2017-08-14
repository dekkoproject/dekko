import QtQuick 2.4
import Dekko.Python 1.0
import Dekko.Mail.API 1.0

Item {
    visible: false
    signal error(string error)
    signal partsReady(var parts)

    function build(text, signature) {
        if (text) {
            py.buildPartsFromMarkdown(text, signature, options)
        }
    }

    MarkdownOptions {
        id: options
    }

    Python {
        id: py

        importPaths: [Qt.resolvedUrl("./")]

        function buildPartsFromMarkdown(text, sig, opts) {
            importModule("markdownutils", function() {
                py.call("markdownutils.build_message_part_map", [text, sig, opts], function(html){
                    Log.logInfo("MarkdownRenderer::build_message_part_map", "Parts ready")
                    partsReady(html)
                })
            })
        }

        onReady: {
            importModule("markdownutils", function(){})
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
