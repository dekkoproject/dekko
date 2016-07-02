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

/*!
* @ingroup qml_actions
*/
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
    signal closeComposer()

    signal orderSimpleToast(string message)
    signal orderSubtitledToast(string title, string subtitle)
    signal orderComplexToast(string title, string subtitle, string image)

    signal setCurrentNavFolder(string folderName);

}
