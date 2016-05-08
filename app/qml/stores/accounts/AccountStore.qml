pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Accounts 1.0
import "../../actions"
import "../../actions/accounts"
import "../../actions/logging"

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
}
