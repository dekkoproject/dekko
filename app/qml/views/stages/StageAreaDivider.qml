import QtQuick 2.4
import Ubuntu.Components 1.3

Rectangle {
    id: divider

    property Item leftSide: Item {}
    property Item leftSideContainer: Item{}
    property bool canDrag: false

    visible: !dekko.viewState.isSmallFF
    anchors {
        left: leftSide.right
        top: parent.top
        bottom: parent.bottom
    }
    width: visible ? units.dp(1) : 0
    color: UbuntuColors.lightGrey
    MouseArea {
        id: resizerSensing
        enabled: canDrag && divider.width > 0 && !dekko.viewState.isSmallFF
        visible: canDrag
        anchors {
            fill: parent
            leftMargin: 0
            rightMargin: enabled ? -units.gu(1.2) : 0
        }
        cursorShape: Qt.SizeHorCursor
        drag {
            axis: Drag.XAxis
            target: resizer
            smoothed: false
            minimumX: units.gu(30)
            maximumX: units.gu(60)
        }
        onPressed: resizer.x = leftSide.width
        onReleased: {
            leftSide.width = Qt.binding(function() {
                if (leftSideContainer.width > units.gu(90) && !dekko.viewState.isSmallFF) {
                    return leftSideContainer.defaultWidth;
                } else {
                    return leftSideContainer.width
                }
            })
        }
    }
}

