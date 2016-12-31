import QtQuick 2.4
import MazDB 1.0
import Dekko.Mail.Stores.Accounts 1.0

MazDBListModel {
    source: AccountStore.identities.sourcePath
    excludeKeys: ["/meta"]
    customRoles: [
        "id",
        "name",
        "email",
        "signature",
        "accountName"
    ]
    rolesCallBack: function (role, key, value) {
        switch(role) {
        case "id":
            return value.id
        case "name":
            return value.name
        case "email":
            return value.email
        case "signature":
            return value.signature
        case "accountName":
            return AccountStore.enabledAccounts.get(value.parentId).name
        default:
            return value
        }
    }
}
