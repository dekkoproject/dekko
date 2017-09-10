/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
* @ingroup qml_actions
*/
KeyTable {
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
    property string noServerPasswordSet
    property string setNoPasswordAllowed
    property string setUserDetails
    property string setServerDetails
    property string lookForServerDetails
    property string serverDetailsFound
    property string noServerDetailsFound

    property string checkProviderForAccountType
    // Dispatch if config is found and user is setting up a pop3 account
    // we want the user to have the best experience and they may not
    // knwo that they can use imap :-)
    property string askAboutImapInstead
    // use imap config
    property string useImapInstead
    // use pop3 config
    property string stickWithPop
    property string runAccountValidation
    property string validateCredentials
    property string validateIfAccountHasEnoughCredentials
    // walk configs till we find one that validates
    property string validateConfigToUse
    // config validation succeeded
    property string validConfigFound
    // config validation failed :-/ we need to open the manual input view
    property string noValidConfigs
    property string requestManualInput
    property string showUseImapDialog
    property string syncNewAccount
    property string accountSynced
    property string accountSyncFailed
    property string validateServerConfiguration
    property string validateServer
    property string serverConfigValid
    property string invalidServerConfig
    property string applyServerConfig
    property string addAnotherAccount
    property string endSetup
    property string removeNewAccount
    property string setDefaultIdentity
}
