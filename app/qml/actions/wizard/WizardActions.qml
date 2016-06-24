/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

/*!
*
* WizardActions API of the setup wizard process
*
* WizardActions can only be used while the SetupWizard is visible.
* The AccountSetup AppListener is disabled and will not process disaptched events
* when the setupwizard is closed
*
* @ingroup qml_actions
*/
ActionCreator {

    /*!
      * Sets the new account type
      * @param description The description of the new account. e.g "GMail" "Yahoo!" etc...
      * @param type The type of account to create. options are AccountConfig.IMAP/POP3/SMTP
      */
    signal setNewAccountType(string description, int type)

    /*!
      * Set the basic user details
      * @param details Takes an object in the format { "name": "", "email": "", "password": "", "nickname": ""}
      */
    signal setUserDetails(var details)
    /*!
      * Validate the set user details
      *
      * Ensure the WizardActions::setUserDetails(details) has been called before this
      */
    signal validateUserDetails()

    /*!
      * Validate the configured user details
      *
      * Note: This is dispatched by validateUserDetails so you don't need to call this directly
      */
    signal validateUser(var user)

    /*!
      * Dispatched when user validation succeeded
      */
    signal userDetailsValid()

    /*!
      * Dispatched when user validation failed
      * @param result An array indicating which data fields were invalid.
      *   i.e Array("name", "password") would mean these two fields were invalid
      */
    signal userDetailsInvalid(var result)
    /*!
      * Dispatched when no password has been set in the userDetails.
      *
      * A prompt should be shown to the user to allow proceeding with no password
      */
    signal noPasswordSet()

    /*!
      * Dispatched when no password has been set for a specific server configuration.
      *
      * A prompt should be shown to the user to allow proceeding with no password
      */
    signal noServerPasswordSet()

    /*!
      * Dispatch acceptance of using an empty password
      */
    signal setNoPasswordAllowed()

    /*!
      * Set a servers config details
      * @param details Object of configuration returned by ServerDetails.toMap()
      *
      * Example object:
      * ```
      * {
      *    "servicetype" : "imap",
      *    "hostname" : "imap.example.com",
      *    "port": 993,
      *    "encryption": AccountConfig.SSL,
      *    "username": "test@example.org",
      *    "password": "password",
      *    "authentication": AccountConfig.CRAM_MD5,
      *    "allowUntrusted": false
      * }
      * ```
      */
    signal setServerDetails(var details)

    /*!
     * Navigate to the next view of the setup wizard
     * @param view url/qresource of the view to navigate to
     * @param properties The properties to be applied to view during creation
     *
     * ```qml
     * Button {
     *     onClicked: {
     *         WizardActions.wizardNavigateTo("qrc://qml/views/nextview.qml", {title: "Next view"})
     *     }
     * }
     * ```
     */
    signal wizardNavigateTo(var view, var properties)

    /*!
     * Navigate to the previous step of the setup wizard
     */
    signal wizardStepBack()

    /*!
     * Navigate to the next step of the setup wizard
     */
    signal wizardStepForward()

    /*!
     * Reset's the NewAccount object of the AccountSetup store
     *
     * Call this when you are resetting the setup wizard.
     */
    signal wizardResetAccount()

    /*!
     * Setup the NewAccount to use one of the PresetProviders
     * @param config The configuration provided by the PresetProvider from the PresetProviderModel
     *
     * ```QML
     * import Dekko.Accounts 1.0
     *
     * ListView {
     *     model: PresetProviderModel {}
     *     delegate: ListItem {
     *         property var provider: model.qtObject
     *         onClicked: WizardActions.wizardSetAccountPreset(provider)
     *     }
     * }
     * ```
     */
    signal wizardSetAccountPreset(var config)
    signal lookForServerDetails()
    signal serverDetailsFound()
    signal noServerDetailsFound()
    signal checkProviderForAccountType()
    signal askAboutImapInstead()
    signal validateCredentials()
    signal validateConfigToUse()
    signal validateIfAccountHasEnoughCredentials()
    signal requestManualInput()
    signal showUseImapDialog()
    signal useImapInstead()
    signal stickWithPop()
    signal noValidConfigs()
    signal runAccountValidation()
    signal accountSynced()
    signal accountSyncFailed()
    signal syncNewAccount()
    signal validateServerConfiguration()
    signal validateServer(var config)
    signal serverConfigValid(string serviceType)
    signal invalidServerConfig(string serviceType, var result)
    signal applyServerConfig(var config)
    signal addAnotherAccount()
    signal endSetup()
    signal removeNewAccount()
}
