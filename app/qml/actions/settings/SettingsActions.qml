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

    signal openSettingsGroup(string group)
    signal switchSettingsGroupLocation(string stackKey)
    signal setSelectedAccount(var account)
    signal updateServerDetails(var details)
    signal updateMarkAsReadInterval(int accountId, int interval)
    signal updateMarkAsReadMode(int accountId, var mode)
    signal saveSelectedAccount()
    signal detectStandardFolders()
    signal createStandardFolders(int accountId)

    signal saveCurrentGroup()
    signal saveCurrentGroupNow()
    signal determineIfSettingsChanged()
    signal settingsChanged()
    signal noChange()
    signal currentGroupSaved()

    signal pickFolder(int accountId, string fieldId)
    signal folderPicked(string pickerId, var folder)
    signal folderPathPicked(string fieldId, string path)
    signal pickFolderCancelled(string pickerId)
}
