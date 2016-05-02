pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import "./"

/* ActionCreator turns signals into dispatched actions
   With a signal like:

    signal signalName(int arg1, string arg2);

  it basically turns into:

    function signalName(arg1, arg2) {
        AppDispatcher.dispatch(ActionTypes.signalName, {"arg1": arg1, "arg2": arg2});
    }

  and in the receiving listener(s) the message attributes match the signal arguments
  e.g

  onDispatched: console.log(message.arg1)
*/
ActionCreator {

    //=========================================================================\\
    // Logging helpers                                                         \\
    // These are handled in components/Logger.qml                              \\
    //=========================================================================\\
    signal logInfo(string location, string message)
    signal logStatus(string location, string message)
    signal logWarning(string location, string message)
    signal logError(string location, string message)

    //=========================================================================\\
    // Dialog queue API                                                        \\
    // All dialogs get pushed onto a queue as we may hit                       \\
    // more than one error at any point and the dialogs                        \\
    // end up being stacked on top of each other                               \\
    // This api queues custom or predefined Dialogs                            \\
    // that get displayed one at a time. See PopupQueue.qml and DialogQueue.qml\\
    // for how this works                                                      \\
    // Note: all dialogs must inherit from DialogBase.qml                      \\
    //=========================================================================\\
    signal showError(string error)
    signal queueDialog(var dlg, var properties)
    signal showConfirmationDialog(string id, string title, string text)
    signal confirmationDialogConfirmed(string id)
    signal confirmationDialogCancelled(string id)

    //==========================================================================\\
    // Setup Wizard API                                                         \\
    // The main logic & state info for this api lives in stores/AccountSetup.qml\\
    //==========================================================================\\
    signal setNewAccountType(string description, int type)
    // Pass object in format of { "name": "", "email": "", "password": ""}
    signal setUserDetails(var details)
    signal validateUserDetails()
    signal validateUser(var user)
    signal userDetailsValid()
    signal userDetailsInvalid(var result)
    signal noPasswordSet()
    signal setNoPasswordAllowed()
    signal setServerDetails(var details)
    signal wizardNavigateTo(var view, var properties)
    signal wizardStepBack()
    signal wizardStepForward()
    signal wizardResetAccount()
    signal wizardSetAccountPreset(var config)

}
