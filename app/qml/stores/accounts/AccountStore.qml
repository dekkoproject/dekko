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
import Dekko.Accounts 1.0
import "../../actions"
import "../../actions/accounts"
import "../../actions/logging"
import "../../actions/popups"

AppListener {
    id: accountStore

    property alias receiveAccountsModel: recieveAccounts.model
    property alias sendAccountsModel: sendAccounts.model

    Accounts{
        id: recieveAccounts
        filter: Accounts.CanReceive
    }

    Accounts {
        id: sendAccounts
        filter: Accounts.CanSend
    }

    Filter {
        type: AccountKeys.deleteAccount
        onDispatched: {
            d.__accountAboutToRemove = message.accountId
            // It doesn't matter which we use here
            if (message.confirmRemoval) {
                Log.logInfo("AccountStore::deleteAccount", "Confirming account removal")
                AccountActions.confirmRemoveAccount()
            } else {
                Log.logInfo("AccountStore::deleteAccount", "Doesn't need confirmation. Removing now")
                AccountActions._confirmRemoval()
            }
        }
    }

    Filter {
        type: AccountKeys.confirmRemoveAccount
        onDispatched: {
            Log.logWarning("AccountStore::confirmRemoveAccount", "Prompting to confirm account removal")
            PopupActions.showConfirmationDialog(d.removeAccountDlgId, qsTr("Remove account"), qsTr("Are you sure you wish to remove this account?"))
        }
    }

    Filter {
        type: PopupKeys.confirmationDialogConfirmed
        onDispatched: {
            if (message.id !== d.removeAccountDlgId) {
                return;
            }
            Log.logInfo("AccountStore::confirmationDialogConfirmed", "Confirmed account removal")
            AccountActions._confirmRemoval()
        }
    }

    Filter {
        type: PopupKeys.confirmationDialogCancelled
        onDispatched: {
            if (message.id !== d.removeAccountDlgId) {
                return
            }
            Log.logInfo("AccountStore::confirmationFialogCancelled", "Cancelling account removal")
            d.__accountAboutToRemove = -1
        }
    }

    Filter {
        type: AccountKeys._confirmRemoval
        onDispatched: {
            Log.logInfo("AccountStore::_confirmRemoval", "Let's delete it \o/")
            recieveAccounts.deleteAccount(d.__accountAboutToRemove)
            d.__accountAboutToRemove = -1
        }
    }

    QtObject {
        id: d
        property string removeAccountDlgId: "remove-account-dlg"
        property int __accountAboutToRemove: -1
    }
}
