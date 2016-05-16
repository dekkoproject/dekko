import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "./components"
import "./delegates"
import "../actions/views"
import "../actions/messaging"

DekkoPage {

    property string pageTitle: "Folders"
    property alias accountId: folderList.accountId

    pageHeader.title: pageTitle
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: ViewActions.popStageArea(dekko.viewState.isLargeFF ? ViewKeys.navigationStack : ViewKeys.messageListStack)
    }
    extendHeader: !dekko.viewState.isSmallFF

    FolderList {
        id: folderList
    }

    PageContent {

        ScrollView {
            anchors {
                fill: parent
            }
            ListView {
                anchors.fill: parent
                clip: true
                model: folderList.model
                delegate: FolderListDelegate {
                    folder: model.qtObject
                    onItemClicked: MessageActions.openFolder(folder.name, folder.messageKey)
                }
            }
        }
    }
}

