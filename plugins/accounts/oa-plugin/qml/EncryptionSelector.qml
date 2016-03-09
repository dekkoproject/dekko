/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
import accounts.plugin.Email 1.0

OptionSelector {
    model: encryptionMethodModel
    delegate: OptionSelectorDelegate {
        text: description
    }

    Component.onCompleted: encryptionMethodModel.loadModel()

    ListModel {
        id: encryptionMethodModel

        function loadModel() {
            encryptionMethodModel.append({ "description": i18n.tr("No encryption"), "method": ServerConfig.CLEAR_TEXT, "port": 0 })
            encryptionMethodModel.append({ "description": i18n.tr("Use encryption (STARTTLS)"), "method": ServerConfig.STARTTLS, "port": 0 })
            encryptionMethodModel.append({ "description": i18n.tr("Force encryption (SSL/TLS)"), "method": ServerConfig.SSL_TLS, "port": 0 })
        }
    }

    function getMethod() {
        return model.get(selectedIndex).method
    }

    function setCurrentIndex(networkMethod) {
        switch (networkMethod) {
        case ServerConfig.INVALID:
        case ServerConfig.CLEAR_TEXT:
            selectedIndex = 0
            break
        case ServerConfig.STARTTLS:
            selectedIndex = 1
            break
        case ServerConfig.SSL_TLS:
            selectedIndex = 2
        }
        console.log("SelectedIndex: ", selectedIndex)
    }
}
