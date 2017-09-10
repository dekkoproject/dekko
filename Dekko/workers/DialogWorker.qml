/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
import QuickFlux 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Views 1.0

AppListener {
    id: dialogListener

    waitFor: [ViewStore.listenerId]
    property string name
    property var dlgTarget
    property PopupQueue popupQueue: PopupQueue {}
    property Component noticePopup
    property Component confirmationPopup

    Filter {
        type: PopupKeys.showError

        onDispatched: {
            if (message.key === dialogListener.name) {
                popupQueue.queue(noticePopup, dialogListener.dlgTarget, {title: qsTr("Error"), text: message.error})
            }
        }
    }

    Filter {
        type: PopupKeys.showNotice
        onDispatched: {
            if (message.key === dialogListener.name) {
                popupQueue.queue(noticePopup, dialogListener.dlgTarget, {title: qsTr("Notice"), text: message.notice})
            }
        }
    }

    Filter {
        type: PopupKeys.queueDialog
        onDispatched: {
            console.log(message.key)
            if (message.key === dialogListener.name) {
                popupQueue.queue(message.dlg, dialogListener.dlgTarget, message.properties)
            }
        }
    }

    Filter {
        type: PopupKeys.showConfirmationDialog
        onDispatched: {
            if (message.key === dialogListener.name) {
                popupQueue.queue(confirmationPopup, dialogListener.dlgTarget, {id: message.id, title: message.title, text: message.text})
            }
        }
    }
}

