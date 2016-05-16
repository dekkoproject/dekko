import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes.Ambiance 1.3
import "../utils/QtCoreAPI.js" as QtCoreAPI

TextArea {
    id: input
    signal completionTokenSeen();

    maximumLineCount: 1
    wrapMode: TextEdit.NoWrap
    autoSize: false
    height: units.gu(4)
    inputMethodHints: Qt.ImhEmailCharactersOnly
    style:TextAreaStyle {
        overlaySpacing: 0
        frameSpacing: 0
        background: Item {}
    }
    onTextChanged: {
        var specials = [";", ","]
        var currentText = text
        for (var i = 0; i < specials.length; i++) {
            if (currentText.endsWith(specials[i])) {
                text = currentText.substring(0, text.length - 1)
                input.completionTokenSeen()
                break;
            }
        }
    }
}

