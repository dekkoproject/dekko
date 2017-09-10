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
import QuickFlux 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Views 1.0

AppListener {
    id: tw
    waitFor: [ViewStore.listenerId]
    property string name
    property var target
    property url toastSource

    property ToastQueue toaster: ToastQueue{}

    Filter {
        type: ViewKeys.orderSimpleToast
        onDispatched: {
            if (message.target === tw.name) {
                toaster.orderToast(target, toastSource, { text: message.message })
            }
        }
    }
}

