pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

KeyTable {

    //logging helpers
    property string logInfo
    property string logStatus
    property string logWarning
    property string logError

    // dialog queue api
    property string showError
    property string queueDialog
    property string showConfirmationDialog
    property string confirmationDialogConfirmed
    property string confirmationDialogCancelled

    // SetupWizard api
    property string wizardStepBack
    property string wizardStepForward
    property string wizardResetAccount
    property string wizardNavigateTo
    property string wizardSetAccountPreset
    property string setNewAccountType
    property string validateUserDetails
    property string validateUser
    property string userDetailsValid
    property string userDetailsInvalid
    property string noPasswordSet
    property string setNoPasswordAllowed
    property string setUserDetails
    property string setServerDetails

}
