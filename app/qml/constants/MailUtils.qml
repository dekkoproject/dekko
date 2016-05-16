pragma Singleton
import QtQuick 2.4
import Dekko.Mail 1.0

QtObject {

    function recipientTypeToString(type) {
        switch (type) {
        case RecipientType.To:
            return qsTr("To")
        case RecipientType.Cc:
            return qsTr("Cc")
        case RecipientType.Bcc:
            return qsTr("Bcc")
        }
    }

}

