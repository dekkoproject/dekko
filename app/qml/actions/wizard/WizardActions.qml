pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

ActionCreator {
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
