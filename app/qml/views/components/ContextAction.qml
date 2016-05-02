import QtQuick 2.4
import Ubuntu.Components 1.3

Action {
    property string description: ""
    property int actionIcon: 0
    property string actionIconColor: UbuntuColors.ash
    property int selectedIcon: 0 // Visible if "selected"
    property bool selected: false
    property string selectedIconColor: UbuntuColors.green
}

