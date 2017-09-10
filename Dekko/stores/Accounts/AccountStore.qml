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
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.API 1.0

AppListener {
    id: accountStore

    /** type:Accounts Enabled accounts object */
    property alias enabledAccounts: enabledAccounts
    /** type:QAbstractListModel enabled accounts model */
    property alias enabledAccountsModel: enabledAccounts.model
    /** type:Accounts Receive accounts object */
    property alias recieveAccounts: recieveAccounts
    /** type:QAbstractListModel receiving accounts model */
    property alias receiveAccountsModel: recieveAccounts.model
    /** type:Accounts Sending accounts object */
    property alias sendAccounts: sendAccounts
    /** type:QAbstractListModel sending accounts model */
    property alias sendAccountsModel: sendAccounts.model
    /** type:Identities account identities interface */
    property alias identities: identities

    // List of all enabled accounts. (IMAP/POP/SMTP)
    Accounts {
        id: enabledAccounts
        filter: Accounts.Enabled
    }

    // List of enabled accounts that receive mail. (IMAP/POP)
    Accounts{
        id: recieveAccounts
        filter: Accounts.CanReceive
    }

    // List of enabled accounts that send mail. (SMTP)
    Accounts {
        id: sendAccounts
        filter: Accounts.CanSend
    }

    Identities {
        id: identities
        onError: {
            Log.logError("AccountsStore::Identities", errorString)
        }
    }

    Filter {
        type: ViewKeys.reloadAccountBasedModels
        onDispatched: {
            enabledAccounts.reset()
            recieveAccounts.reset()
            sendAccounts.reset()
        }
    }
}
