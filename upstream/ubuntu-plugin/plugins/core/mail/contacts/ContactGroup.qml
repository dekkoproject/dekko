import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Ubuntu.Components 1.0

Item {
    default property alias content: contentCol.data
    property alias title: groupHeader.text
    property alias spacing: contentCol.spacing
    height: groupHeader.height + contentCol.height + units.gu(1.5)
    width: parent.width
    SectionHeader {
        id: groupHeader
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
    }
    Column {
        id: contentCol
        anchors {
            left: parent.left
            top: groupHeader.bottom
            right: parent.right
        }
    }
}
