/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../constants"

PixelPerfectItem {
    id: dekkoHeader
    property string title
    property Action backAction
    property Action primaryAction
    // List of actions to be used. If more than 1 action, the actions will be placed
    // in the action drawer.
    property list<Action> secondaryActions
    // List of actions to use for the multi-select toolbar
    property list<Action> multiSelectActionList
    // List of filter sections to display
    property alias filterSections: filters.filterSections
    // The selected filter index. i.e ListView.currentIndex
    property alias selectedIndex: filters.selectedIndex
    // Property to add additional content inside the header
    // This get's loaded when state === "custom",
    // NOTE: this is loaded into an empty header so any back actions etc need implementing
    // for each custom component
    property Component customComponent
    // Property to hide the bottom margin, will
    // be useful when we push the mailbox filter up against the bottom
    property bool showDivider: true
    // If enableSearching is enabled the search action overrides the primaryAction
    // And the primaryAction if also set get's place either in secondaryAction or the action drawer if there
    // are already multiple secondaryAction set.
    property bool enableSearching: false
    property bool searchInProgress: false
    // Setting the pageFlickable will make the child flickable take all available space below
    // the header. For our usecase we aren't going to have a flickable that uses on a partial area
    // of the screen so there is no need to configure it to do so here.
    property Item pageFlickable: null
    property bool composeButtonEnabled: false
    property alias backgroundVisible: backgroundColor.visible
    // Useful readonly properties
    readonly property bool isDefault: state === "default"
    readonly property bool isSearchMode: state === "search"
    readonly property bool isMultiSelect: state === "multi-select"
    readonly property int dividerHeight: units.dp(1)
    property bool extend: false
    readonly property int extendHeight: extend ? units.gu(3) : 0
    // Get's emitted when a search is activated from the enter key being pressed in the search field.
    signal searchActivated(string searchString)
    // emitted on close of search state.
    signal searchCanceled()
    // emitted on close of multi-select state
    signal multiSelectCanceled()

    signal composeNewMessage()

    // function to start multi-select mode
    function startMultiSelect() {
        state = "multi-select"
    }
    onPageFlickableChanged: internal.checkPageFlickableAnchors();
    onHeightChanged: internal.checkPageFlickableAnchors()

    anchors {
        left: parent.left
        top: parent.top
        right: parent.right
    }
    visible: true
    height: visible ? units.gu(6) + filterColumn.height + btmMargin.height + extendHeight : 0


    Action {
        id: drawerAction
        iconName: "contextual-menu"
        onTriggered: actionDrawer.opened = true
    }

    Action {
        id: searchAction
        iconName: "find"
        onTriggered: dekkoHeader.state = "search"
    }

    QtObject {
        id: internal

        function checkPageFlickableAnchors() {
            if (dekkoHeader.pageFlickable) {
                pageFlickable.anchors.left = pageFlickable.parent.left
                pageFlickable.anchors.right = pageFlickable.parent.right
                pageFlickable.anchors.bottom = pageFlickable.parent.bottom
                pageFlickable.anchors.top = dekkoHeader.bottom;
            }
        }
    }

    Rectangle {
        id: backgroundColor
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        height: units.gu(6)
        color: "#ffffff"
    }

    Component {
        id: multiSelectComponent
        Row {
            property alias button: inner_button
            anchors.fill: parent
//            anchors.leftMargin: backAction ? 0 : units.gu(2)
            anchors.rightMargin: Style.defaultSpacing
            spacing: Style.defaultSpacing
            HeaderButton {
                id: inner_button
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                visible: backAction
                action: Action {
                    iconName: "back"
                    onTriggered: {
                        dekkoHeader.state = "default"
                        multiSelectCanceled()
                    }
                }
            }
            Repeater {
                model: multiSelectActionList
                delegate: HeaderButton {
//                    verticalCenterOffset: -units.gu(0.5)
                    iconSize: units.gu(2.2)
                    action: modelData
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                }
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: searchComponent
        RowLayout {
            property alias button: inner_button

            anchors.fill: parent
            anchors.rightMargin: Style.defaultSpacing
            spacing: 0

            HeaderButton {
                id: inner_button
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                visible: action
                action: Action {
                    iconName: "back"
                    onTriggered: {
                        dekkoHeader.state = "default"
                        searchCanceled()
                    }
                }
            }
            TextField {
                id: sf
                onAccepted: {searchActivated(text); focus = false}
                onTextChanged: {
                    if (text) {
                        if (searchTimer.running) {
                            searchTimer.restart()
                        } else {
                            searchTimer.start()
                        }
                    } else {
                        searchTimer.stop()
                    }
                }

                placeholderText: qsTr("Enter search...")
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()

                Timer {
                    id: searchTimer
                    interval: 500
                    repeat: false
                    onTriggered: searchActivated(sf.text)
                }
            }
            Item {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                width: units.gu(4)
                visible: dekkoHeader.searchInProgress
                ActivityIndicator {
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    running: dekkoHeader.searchInProgress
                    visible: running
                }
            }
        }
    }

    Component {
        id: defaultHeadState
        StretchRow {
            property alias button: inner_button
            height: parent ? parent.height : undefined
            anchors {
                fill: parent
                leftMargin: backAction ? 0 : units.gu(1)
            }
            spacing: 0

            HeaderButton {
                id: inner_button
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                implicitWidth: units.gu(4)
                visible: backAction
                action: backAction ? backAction : null
            }

            Stretcher {
                height: parent.height
                Label {
                    text: dekkoHeader.title
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    fontSize: "large"
                    elide: Text.ElideRight
                    clip: true
                    textFormat: Qt.RichText
                    Layout.fillWidth: true
                }
            }


            HeaderButton {
                id: compose_button
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                width: units.gu(4)
                implicitWidth: width
                implicitHeight: height
                visible: !dekko.viewState.isSmallFF && composeButtonEnabled
                action: Action {
                    iconName: "list-add"
                    onTriggered: composeNewMessage()
                }
            }
            HeaderButton {
                id: inner2_button
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                width: units.gu(4)
                implicitWidth: width
                implicitHeight: height
                visible: action ? action.visible : false
                action: enableSearching ? searchAction : primaryAction
            }
            HeaderButton {
                id: inner3_button
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                width: units.gu(4)
                implicitWidth: width
                implicitHeight: height
                visible: secondaryActions[0] ? secondaryActions[0].visible : false
                action: secondaryActions.length > 1 ? drawerAction : secondaryActions.length === 1 ? secondaryActions[0] : null
            }
        }
    }
    Column {
        id: filterColumn
        anchors {
            left: parent.left
            top: backgroundColor.bottom
            right: parent.right
        }
        height: filters.height
        FilterBar {
            id: filters
            anchors {
                left: parent.left
                right: parent.right
            }
            height: filterSections.length && !isMultiSelect ? units.gu(3) : 0
        }

    }


    Line {
        id: btmMargin
        anchors {
            left: parent.left
            right: parent.right
            top: filterColumn.bottom
            topMargin: extend ? extendHeight : 0
        }
        visible: showDivider
    }

    Loader {
        id: loader
        anchors{
            left: backgroundColor.left
            right: backgroundColor.right
            top: backgroundColor.top
            bottom: backgroundColor.bottom
            leftMargin: units.gu(1)
            rightMargin: units.gu(1)
        }
        Binding {
            target: loader
            property: "sourceComponent"
            value: defaultHeadState
            when: loader && dekkoHeader.state === "default"
        }
        Binding {
            target: loader
            property: "sourceComponent"
            value: searchComponent
            when: loader && dekkoHeader.state === "search"
        }
        Binding {
            target: loader
            property: "sourceComponent"
            value: multiSelectComponent
            when: loader && dekkoHeader.state === "multi-select"
        }
        Binding {
            target: loader
            property: "sourceComponent"
            value: customComponent
            when: loader && dekkoHeader.state === "custom"
        }
    }

    ActionDrawer {
        id: actionDrawer
        anchors {
            top: parent.bottom
            right: parent.right
        }
        actions: secondaryActions.length > 1 ? secondaryActions : []
    }

    state: "default"
    states: [
        State {
            name: "default"
        },
        State {
            name: "search"
        },
        State {
            name: "multi-select"
            PropertyChanges {
                target: dekkoHeader
                height: units.gu(6) + filterColumn.height
            }
        },
        State {
            name: "custom"
        }
    ]
}
