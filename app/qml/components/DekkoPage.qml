import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    id: dekkoPage

    property alias pageHeader: h

    signal searchActivated(string searchString)
    signal searchCanceled()
    signal multiSelectCanceled()

    function startMultiSelect() {
        h.startMultiSelect()
    }

    // We don't want the sdk one. I just don't like it. And it always has quirky behaviour :-/
    header: PageHeader {
        visible: false
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
