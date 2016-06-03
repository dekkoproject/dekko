import QtQuick 2.4
import QtQuick.Window 2.1
import Qt.labs.folderlistmodel 2.1;
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../components"

Window {
    id: addAttachmentsDialog
    title: qsTr("Select file...")
    modality: Qt.WindowModal
    minimumHeight: units.gu(50)
    minimumWidth: units.gu(70)
    maximumHeight: units.gu(120)
    maximumWidth: units.gu(150)
    width: units.gu(80)

    signal rejected
    signal accepted
    FolderListModel {
        id: modelFS;
        showDirs: true;
        showFiles: true;
        showHidden: false;
        showDirsFirst: true;
        showDotAndDotDot: false;
        folder: "file://%1".arg(Paths.standardHomeLocation())
    }
    Item {
        anchors.fill: parent

        StretchRow {
            spacing: 0
            anchors.fill: parent

            PanelContainer {
                resizable: true
                size: units.gu(20)
                maxSize: units.gu(25)
                height: parent.height
                activeEdge: Item.Right

                Rectangle {
                    anchors.fill: parent
                    color: UbuntuColors.porcelain
                    Label {
                        anchors.centerIn: parent
                        text: "Places links go here"
                    }
                }
            }

            Stretcher {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                Item {
                    anchors.fill: parent
                    StretchColumn {
                        anchors.fill: parent
                        spacing: 0
                        PanelContainer {
                            id: actionBar1
                            resizable: true
                            minSize: units.gu(5)
                            maxSize: units.gu(7)
                            size: units.gu(6)
                            width: parent.width
                            activeEdge: Item.Bottom
                            Rectangle {
                                anchors.fill: parent
                                color: UbuntuColors.porcelain
                                StretchRow {
                                    anchors {
                                        leftMargin: units.gu(2)
                                        rightMargin: units.gu(2)
                                        fill: parent
                                    }
                                    Button {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "Go up"
                                    }

                                    Stretcher{}

                                    Label {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: qsTr("Show hidden")
                                    }
                                    CheckBox {
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                            }
                        }
                        Stretcher {

                            ScrollView {
                                anchors.fill: parent
                                ListView {
                                    anchors.fill: parent
                                    model: modelFS
                                    delegate: ListItem {
                                        height: layout.height
                                        ListItemLayout {
                                            id: layout
                                            title.text: model.fileName + (model.fileIsDir ? "/" : "");
                                        }
                                    }
                                }
                            }

                        }
                        PanelContainer {
                            resizable: true
                            minSize: units.gu(5)
                            maxSize: units.gu(7)
                            size: units.gu(6)
                            width: parent.width
                            activeEdge: Item.Top

                            Rectangle {
                                anchors.fill: parent
                                color: UbuntuColors.porcelain
                                StretchRow {
                                    anchors {
                                        leftMargin: units.gu(2)
                                        rightMargin: units.gu(2)
                                        fill: parent
                                    }
                                    Stretcher{}
                                    Button {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "Click me"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
