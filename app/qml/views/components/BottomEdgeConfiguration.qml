import QtQuick 2.4

QtObject {
    property bool canActionTrigger: false
    property bool enabled: true
    property bool hintVisible: false
    property string hintText: ""
    property string iconSource: ""
    // The dispatched key that will open the composer
    property string activationKey: ""
    property Component sourceComponent: null
    property Flickable flickable: null

}

