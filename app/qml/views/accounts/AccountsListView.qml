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
import QtQuick 2.4
import Ubuntu.Components 1.3
import "../../actions/accounts"
import "../../stores/accounts"
import "../../constants"
import "../components"

ListView {

    anchors.fill: parent
    property bool canRemove: false
    property bool contextActionsEnabled: false

    signal delegateClicked(var delegate)
    signal accountClicked(var account)

    add: DekkoAnimation.listViewAddTransition
    addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
    remove: DekkoAnimation.listViewRemoveTransition
    removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition

    cacheBuffer: units.gu(35)
    model: AccountStore.enabledAccountsModel
    delegate: AccountDelegate {
        id: delegate
        canDelete: canRemove
        showContextActions: contextActionsEnabled
        account: model.qtObject
        showProgression: true
        onClicked: {
            delegateClicked(delegate)
            accountClicked(model.qtObject)
        }
    }
}
