pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

//=========================================================================\\
// Dialog queue API                                                        \\
// All dialogs get pushed onto a queue mainly because we may hit           \\
// more than one error at any point and the dialogs                        \\
// end up being stacked on top of each other                               \\
// This api queues custom or predefined Dialogs                            \\
// that get displayed one at a time. See PopupQueue.qml and DialogQueue.qml\\
// for how this works                                                      \\
// Note: all dialogs must inherit from DialogBase.qml                      \\
//=========================================================================\\
ActionCreator {

    signal showError(string error)
    signal showNotice(string notice)
    signal queueDialog(var dlg, var properties)
    signal showConfirmationDialog(string id, string title, string text)
    signal confirmationDialogConfirmed(string id)
    signal confirmationDialogCancelled(string id)
    signal clearDialogQueue()// TODO
    signal cancelCurrentDialog()// TODO

}
