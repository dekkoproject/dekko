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
import Dekko.Mail.Settings 1.0
import Dekko.Ubuntu.Components 1.0

PageFlickable {
    id: displaySettings

    SectionHeader {
        textMargin: 0
        text: qsTr("Navigation menu")
    }

    LabeledSwitch {
        text: qsTr("Show smart folders")
        checked: PolicyManager.views.smartFoldersVisible
        onCheckedChanged: {
            if (checked !== PolicyManager.views.smartFoldersVisible) {
                PolicyManager.views.smartFoldersVisible = checked
            }
        }
    }

    LabeledSwitch {
        text: qsTr("Show favourite folders")
        checked: false
        onCheckedChanged: {
            if (checked !== PolicyManager.views.setFavouritesVisible) {
                PolicyManager.views.setFavouritesVisible = checked
            }
        }
    }

    SectionHeader {
        textMargin: 0
        text: qsTr("Messages")
    }

    LabeledSwitch {
        text: qsTr("Show avatars")
        checked: PolicyManager.views.gravatarEnabled
        onCheckedChanged: {
            if (checked !== PolicyManager.views.gravatarEnabled) {
                PolicyManager.views.gravatarEnabled = checked
            }
        }
    }

    LabeledSwitch {
        text: qsTr("Prefer plain text")
        checked: PolicyManager.views.preferPlainText
        onCheckedChanged: {
            if (checked !== PolicyManager.views.preferPlainText) {
                PolicyManager.views.preferPlainText = checked
            }
        }
    }
}
