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
}
