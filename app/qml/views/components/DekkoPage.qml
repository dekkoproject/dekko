import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: dekkoPage

    property alias pageHeader: h
    property alias selectedIndex: h.selectedIndex
    property alias isSearchMode: h.isSearchMode
    property alias extendHeader: h.extend

    signal searchActivated(string searchString)
    signal searchCanceled()
    signal multiSelectCanceled()

    function startMultiSelect() {
        h.startMultiSelect()
    }

    DekkoHeader {
        id: h
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        onSearchActivated: dekkoPage.searchActivated(searchString)
        onSearchCanceled: dekkoPage.searchCanceled()
        onMultiSelectCanceled: dekkoPage.multiSelectCanceled()
    }
}
