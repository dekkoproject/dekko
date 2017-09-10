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
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0

AppListener {

    waitFor: [AccountStore.listenerId]

    Filter {
        type: AccountKeys.deleteAccount
        onDispatched: {
            d.__accountAboutToRemove = message.accountId
            if (!AccountStore.enabledAccounts.hasAccount(d.__accountAboutToRemove)) {
                Log.logError("AccountWorker::deleteAccount", "Invalid account id. Cancelling removal")
                if (message.confirmRemoval) {
                    ViewActions.orderSimpleToast(qsTr("Account removal failed"));
                }
            }
            // It doesn't matter which we use here
            if (message.confirmRemoval) {
                Log.logInfo("AccountWorker::deleteAccount", "Confirming account removal")
                AccountActions.confirmRemoveAccount()
            } else {
                Log.logInfo("AccountWorker::deleteAccount", "Doesn't need confirmation. Removing now")
                AccountActions._confirmRemoval()
            }
        }
    }

    Filter {
        type: AccountKeys.confirmRemoveAccount
        onDispatched: {
            Log.logWarning("AccountWorker::confirmRemoveAccount", "Prompting to confirm account removal")
            PopupActions.showConfirmationDialog(PopupKeys.popupSettings, d.removeAccountDlgId, qsTr("Remove account"), qsTr("Are you sure you wish to remove this account?"))
        }
    }

    Filter {
        type: PopupKeys.confirmationDialogConfirmed
        onDispatched: {
            if (message.id !== d.removeAccountDlgId) {
                return;
            }
            Log.logInfo("AccountWorker::confirmationDialogConfirmed", "Confirmed account removal")
            AccountActions._confirmRemoval()
        }
    }

    Filter {
        type: PopupKeys.confirmationDialogCancelled
        onDispatched: {
            if (message.id !== d.removeAccountDlgId) {
                return
            }
            Log.logInfo("AccountWorker::confirmationFialogCancelled", "Cancelling account removal")
            d.__accountAboutToRemove = -1
        }
    }

    Filter {
        type: AccountKeys._confirmRemoval
        onDispatched: {
            Log.logInfo("AccountWorker::_confirmRemoval", "Let's delete it \o/")
            if (AccountStore.enabledAccounts.hasAccount(d.__accountAboutToRemove)) {
                AccountActions.removeIdentitiesForAccount(d.__accountAboutToRemove)
                AccountStore.enabledAccounts.deleteAccount(d.__accountAboutToRemove)
                ViewActions.orderSimpleToast(qsTr("Account removed"));
            } else {
                Log.logError("AccountWorker::_confirmRemoval", "Invalid account id. Cancelling removal")
                ViewActions.orderSimpleToast(qsTr("Account removal failed"));
            }
            d.__accountAboutToRemove = -1
        }
    }

    Filter {
        type: AccountKeys.addIdentity
        onDispatched: {
            Log.logInfo("AccountWorker::addIdentity", "Adding new identity")
            var identity = message.identity
            if (identity.id) {
                Log.logInfo("AccountWorker::addIdentity", "Identity has valid id. Updating instead")
                AccountActions.updateIdentity(identity)
            } else {
                if (AccountStore.identities.add(identity)) {
                    Log.logInfo("AccountWorker::addIdentity", "Identity created")
                } else {
                    Log.logError("AccountWorker::addIdentity", "Failed creating identity")
                }
            }
        }
    }

    Filter {
        type: AccountKeys.updateIdentity
        onDispatched: {
            Log.logInfo("AccountWorker::updateIdentity", "Adding new identity")
            var identity = message.identity
            if (!identity.id) {
                Log.logInfo("AccountWorker::updateIdentity", "Identity has no valid id. Adding new identity instead")
                AccountActions.addIdentity(identity)
            } else {
                if (AccountStore.identities.update(identity)) {
                    Log.logInfo("AccountWorker::updateIdentity", "Identity updated")
                } else {
                    Log.logError("AccountWorker::updateIdentity", "Identity update failed")
                }
            }
        }
    }

    Filter {
        type: AccountKeys.removeIdentity
        onDispatched: {
            if (AccountStore.identities.remove(message.id)) {
                Log.logInfo("AccountWorker::removeIdentity", "Identity removed")
            } else {
                Log.logError("AccountWorker::removeIdentity", "Identity removal failed")
            }
        }
    }

    Filter {
        type: AccountKeys.removeIdentitiesForAccount
        onDispatched: {
            Log.logInfo("AccountWorker::removeIdentitiesForAccount", "Removing identities for account")
            AccountStore.identities.removeAccount(message.accountId)
        }
    }

    QtObject {
        id: d
        property string removeAccountDlgId: "remove-account-dlg"
        property int __accountAboutToRemove: -1
    }
}
