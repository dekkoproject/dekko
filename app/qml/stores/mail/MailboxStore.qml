pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import "../../actions"
import "../../actions/messaging"
import "../../actions/logging"

BaseMessagingStore {
    id: mboxStore

    // We only want to expose the actual models.
    property alias standardFoldersModel: stdFolders.children
    property alias smartFoldersModel: smartFolders.children

    MessageFilterCollection {
        id: stdFolders
        filter: MessageFilterCollection.StandardFolders
    }
    MessageFilterCollection {
        id: smartFolders
        filter: MessageFilterCollection.SmartFolders
    }

    QtObject {
        id: d

    }
}
