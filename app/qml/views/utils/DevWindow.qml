import QtQuick 2.4
import QtQuick.Window 2.2
import Dekko.Components 1.0
import Dekko.Mail 1.0
import Ubuntu.Components 1.3
import Ubuntu.PerformanceMetrics 1.0
import "../components"
import "../../constants"

Window {
    title: "Dev Stuff"
    flags: Qt.Window
    modality: Qt.NonModal
    width: pc.width;
    height: pc.height;
    visible: true;
    onClosing: { attach (); }
}
