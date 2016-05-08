pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

KeyTable {
    // dialog queue api
    property string showError
    property string showNotice
    property string queueDialog
    property string showConfirmationDialog
    property string confirmationDialogConfirmed
    property string confirmationDialogCancelled
    property string clearDialogQueue //TODO
    property string cancelCurrentDialog // TODO
}
