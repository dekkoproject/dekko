import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import "../../actions/messaging"

AppListener {
    id: baseStore
    property bool hasUndoableActions: Client.service.hasUndoableAction
    property string undoableActionDescription: Client.service.undoableActionDescription
}

