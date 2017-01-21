import QtQuick 2.4
import Dekko.Python 1.0
import Dekko.Mail.API 1.0

Item {

    property string cssString: ""

    signal inlined(string html)
    signal error(string error)

    function inline_css(html) {
        if (html) {
            if (cssString) {
                py.inlineWithCSSString(html, cssString)
            } else {
                py.inlineWithDefaultCSS(html)
            }
        }
    }

    Python {
        id: py

        importPaths: [Qt.resolvedUrl("./")]

        function inlineWithCSSString(html, css) {
            importModule("markdownutils", function() {
                py.call("markdownutils.inline_with_css_string", [html, css], function(result){
                    Log.logInfo("MarkdownRenderer::inline_with_css_string", "Inlined")
                    inlined(result)
                })
            })
        }

        function inlineWithDefaultCSS(html) {
            importModule("markdownutils", function() {
                py.call("markdownutils.inline_css", [html], function(result){
                    Log.logInfo("MarkdownRenderer::inline_css", "Inlined")
                    inlined(result)
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
