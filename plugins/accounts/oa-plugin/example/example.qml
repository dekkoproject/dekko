/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
import Ubuntu.OnlineAccounts 0.1
import "../qml"

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "mailautodiscover.dpniel"

    width: units.gu(50)
    height: units.gu(60)

    // use our own instance of account
    Account{
        id: account
        objectHandle: Manager.createAccount("e-mail")
//        objectHandle: Manager.loadAccount(8)
    }

    Page {
        title: i18n.tr("Email account")
        flickable: main
        Main {
            id: main
            onFinished: Qt.quit()
        }
    }
}

