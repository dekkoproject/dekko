import QtQuick 2.4
import Dekko.Python 1.0
import Dekko.Mail.API 1.0

Item {

    property bool extras: false
    property bool abbreviations: false
    property bool attributeList: false
    property bool definitionLists: false
    property bool fencedCodeBlocks: false
    property bool footnotes: false
    property bool tables: false
    property bool smartStrong: false
    property bool admonition: false
    property bool codehilite: false
    property bool headerId: false
    property bool metadata: false
    property bool nl2br: false
    property bool saneLists: false
    property bool smartyPants: false
    property bool toc: false
    property bool wikilinks: false

    signal error(string error)
    signal rendered(string html)

    function render(text) {
        if (text) {
            py.renderMarkdown(text, enabledExtensions())
        }
    }

    function enabledExtensions() {
        var e = []
        if (extras) {
            e.push('markdown.extensions.extra')
        } else {
            if (abbreviations) {
                e.push('markdown.extensions.abbr')
            }
            if (attributeList) {
                e.push('markdown.extensions.attr_list')
            }
            if (definitionLists) {
                e.push('markdown.extensions.def_list')
            }
            if (fencedCodeBlocks) {
                e.push('markdown.extensions.fenced_code')
            }
            if (footnotes) {
                e.push('markdown.extensions.footnotes')
            }
            if (tables) {
                e.push('markdown.extensions.tables')
            }
            if (smartStrong) {
                e.push('markdown.extensions.smart_strong')
            }
        }
        if (admonition) {
            e.push('markdown.extensions.admonition')
        }
        if (headerId) {
            e.push('markdown.extensions.headerid')
        }
        if (metadata) {
            e.push('markdown.extensions.meta')
        }
        if (nl2br) {
            e.push('markdown.extensions.nl2br')
        }
        if (saneLists) {
            e.push('markdown.extensions.sane_lists')
        }
        if (smartyPants) {
            e.push('markdown.extensions.smarty')
        }
        if (toc) {
            e.push('markdown.extensions.toc')
        }
        if (wikilinks) {
            e.push('markdown.extensions.wikilinks')
        }
        if (codehilite) {
            e.push('markdown.extensions.codehilite')
        }
        return e
    }


    Python {
        id: py

        importPaths: [Qt.resolvedUrl("./")]

        function renderMarkdown(text, extras) {
            importModule("markdownutils", function() {
                py.call("markdownutils.render_markdown", [text, extras], function(html){
                    Log.logInfo("MarkdownRenderer::renderMrkdown", "Rendered")
                    rendered(html)
                })
            })
        }
    }
}
