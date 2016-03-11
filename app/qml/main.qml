import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "untitled.dpniel"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    //automaticOrientation: true


    width: units.gu(100)
    height: units.gu(75)

    Page {
        title: i18n.tr("untitled")
        Accounts{
            id: accounts
        }

        ListView {
            anchors.fill: parent
            model: accounts.model
            delegate: ListItem {
                height: layout.implicitHeight
                ListItemLayout {
                    id: layout
                    title.text: qtObject.name
                }
            }
        }
    }
}

