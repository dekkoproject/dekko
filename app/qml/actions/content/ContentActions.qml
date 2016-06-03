pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

ActionCreator {
    signal importFromContentHub(var transfer)
    signal pickFile()
    signal openFilePicker()
    signal pickerClosed()
    signal filesSelected(var files)
}
