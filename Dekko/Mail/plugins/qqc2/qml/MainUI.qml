import QtQuick 2.4
import QtQuick.Controls 2.0
import Dekko.Mail.API 1.0


ApplicationWindow {
    visible: true
    minimumWidth: 350
    minimumHeight: 450

    title: qsTr("Dekko Mail")

    Text {
        anchors.centerIn: parent
        text: "Hello World"
    }

//    Item {
//        anchors.fill: parent

//        Binding {
//            target: window
//            property: "minimumWidth"
//            value: units.gu(30)
//        }

//        Loader {
//            id: workerLoader
//            asynchronous: true
//            source: "qrc:/qml/workers/Workers.qml"
//        }


//        // Workers we need straight away
//        QtObject {
//            id: d
//            property MailWorker mailWorker: MailWorker {}
//            property ComposerWorker composeWorker: ComposerWorker {}
//            property ContentWorker contentWorker: ContentWorker{}
//    //        property Logger logger: Logger {
//    //            devLoggingEnabled: devModeEnabled
//    //        }
//        }
//    }

}
