pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

KeyTable {

    // StageStack api
    property string stageStackReady
    property string pushStage
    property string popStage
    property string clearStageStack
    property string runSetupWizard

    // messagelistview
    property string openNavDrawer
    property string closeNavDrawer
    property string toggleNavDrawer
    property string expandNavPanel
    property string collapseNavPanel

    property string positionMessageListAtStart
    property string openMessageComposer
    property string closeMessageComposer
    property string replyToOpenMessage
    property string detachComposer
    property string attachComposer

    property string resetPanelWidths

    // StageArea api
    property string pushToStageArea
    property string replaceTopStageAreaItem
    property string popStageArea
    property string clearStageArea

    // Compose panel API
    property string openComposer
    property string setComposePanelSource
    property string closeComposer
    // These are not actions as such but keys for the stagearea's
    // They are used to determine which StackView we are pushing a page to
    // via pushToStage(stackID, ....)
    // NOTE: When creating a new StageArea add a key for it here
    property string navigationStack
    property string messageListStack
    property string messageViewStack
}
