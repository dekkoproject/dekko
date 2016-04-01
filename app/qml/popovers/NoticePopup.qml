import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: dlg

    Button {
        text: "Ok"
        color: UbuntuColors.green
        onClicked: PopupUtils.close(dlg)
    }
}

