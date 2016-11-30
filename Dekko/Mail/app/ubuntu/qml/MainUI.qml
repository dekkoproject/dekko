import QtQuick 2.4
import QtQuick.Window 2.1
import Ubuntu.Components 1.3
import "./workers"
import Dekko.Mail.Workers 1.0

Window {

    visible: true
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(30)

    Binding {
        target: window
        property: "minimumWidth"
        value: units.gu(30)
    }

    Loader {
        id: workerLoader
        asynchronous: true
        source: "qrc:/qml/workers/Workers.qml"
    }

    // Workers we need straight away
    QtObject {
        id: d
        property MailWorker mailWorker: MailWorker {}
        property ComposerWorker composeWorker: ComposerWorker {}
        property ContentWorker contentWorker: ContentWorker{}
//        property Logger logger: Logger {
//            devLoggingEnabled: devModeEnabled
//        }
    }
}
