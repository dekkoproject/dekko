pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

ActionCreator {

    //=========================================================================\\
    // StageStack api                                                          \\
    //=========================================================================\\
    signal stageStackReady() // Call when the StageStage onCompleted
    signal pushStage(var stage, var properties)
    signal popStage()
    signal clearStageStack()
    signal runSetupWizard()

    // messagelistview
    signal openNavDrawer()
    signal closeNavDrawer()
    signal expandNavPanel()
    signal collapseNavPanel()
    signal toggleNavDrawer()

    signal positionMessageListAtStart()
    signal openMessageComposer()
    signal closeMessageComposer()
    signal replyToOpenMessage()
    signal detachComposer()
    signal attachComposer()

    signal resetPanelWidths()

    signal pushToStageArea(string stageID, var page, var properties)
    signal replaceTopStageAreaItem(string stageID, var page, var properties)
    signal popStageArea(string stageID)
    signal clearStageArea(string stageID)


    signal openComposer()
    signal setComposePanelSource(var source, var properties)

}
