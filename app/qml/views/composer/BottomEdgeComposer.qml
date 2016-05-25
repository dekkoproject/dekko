import QtQuick 2.4
import Ubuntu.Components 1.3
import QuickFlux 1.0
import "../../actions/composer"
import "../../actions/views"
import "../../stores/composer"
import "../components"

BottomEdgeConfiguration {
    sourceComponent: DekkoPage {
        width: dekkoPage.width
        height: dekkoPage.height
        Rectangle {
            anchors.fill: parent
            color: "#FFFFFF"
        }
        pageHeader.title: "New message"
        pageHeader.backAction: Action {
            iconName: "down"
            onTriggered: ComposerActions.discardMessage()
        }
        pageHeader.primaryAction: ComposerStore.actions.sendAction

        MessageComposer {
            anchors.fill: parent
        }

        AppScript {
            runWhen: ViewKeys.closeComposer
            enabled: listenerEnabled
            script: {
                if (!dekko.viewState.isLargeFF) {
                    bottomEdge.collapse()
                } else {
                    exit.bind(this, 0)
                }
            }
        }
    }
}

