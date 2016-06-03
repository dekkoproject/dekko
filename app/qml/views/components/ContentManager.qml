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
import QuickFlux 1.0
import Ubuntu.Content 1.1
import "../../actions/composer"
import "../../actions/content"
import "../../actions/logging"
import "../../actions/views"
import "../dialogs"

AppListener {


    property string pickerUrl: ""

    AppScript {
        runWhen: ContentKeys.pickFile
        script: {
            // First open the file picker for the platform
            // This will either be the ContentHub picker
            // or Qt's own FileDialog for non unity8 platforms
            ContentActions.openFilePicker()
            // Listen on selected files and add them to the composer
            once(ContentKeys.filesSelected, function(files) {
                console.log("FILES: ", files)
                for (var i in files) {
                    if (isRunningOnMir) {
                        ComposerActions.addFileAttachment(files[i].url)
                    } else {
                        console.log("File: ", files[i])
                        var file = files[i]
                        console.log("SubFile: ", file[i])
                        ComposerActions.addFileAttachment(file.toString().replace("file://", ""))
                    }
                }
            })
            // Drop all callbacks if the picker is closed with no selections
            once(ContentKeys.pickerClosed, exit.bind(this, 0))
        }
    }

    Filter {
        type: ContentKeys.openFilePicker
        onDispatched: {
            // TODO: open picker
            if (isRunningOnMir) {
                //TODO: popup content hub
            } else {
//                var c = Qt.createComponent("qrc:/qml/views/dialogs/FileDialog.qml")
                var filePicker = pick.createObject(dekko)
                filePicker.show()
                filePicker.accepted.connect(function(){
                    ContentActions.filesSelected(filePicker.fileUrls)
                    filePicker.close()
                    filePicker.destroy()
                })
                filePicker.rejected.connect(function() {
                    ContentActions.pickerClosed()
                })
            }
        }
    }

    Component {
        id: pick
        FileDialog {
            visible: true
//            id: addAttachmentsDialog
//            title: qsTr("Select files")
//            folder: shortcuts.home
//            onRejected: {
//                console.log("Canceled adding attachments")
//                close()
//                destroy()
//            }
//            selectMultiple: true
//            Component.onCompleted: open()
        }
    }

    Connections {
        target: ContentHub
        onImportRequested: {
            Log.logStatus("[ContentHub::importRequested]", "import requested, running import...")
            ContentActions.importFromContentHub(transfer)
        }
        onShareRequested: {
            Log.logStatus("[ContentHub::shareRequested]", "share requested. importing share")
            ContentActions.importFromContentHub(transfer)
        }
        onExportRequested: {
            Log.logStatus("ContentHub::exportRequested",  "NotImplementedYet")
        }
    }

    Filter {
        type: ContentKeys.importFromContentHub
        onDispatched: {
            if (message.transfer.length === 0) {
                Log.logWarning("ContentManager::importFromContentHub", "Not items to transfer :-/")
                return
            }
            Log.logInfo("ContentManager::importFromContentHub", "Number of transferred items: " + message.transfer.items.length)
            // open the composer
            ViewActions.openComposer()
            for (var item in message.transfer.items) {
                var url = message.transfer.items[item].url
                Log.logInfo("ContentManager::importFromContentHub", "Item url: " + url)
                var text = message.transfer.items[item].text
                Log.logInfo("ContentManager::importFromContentHub", "Item text: " + text)
                switch (message.transfer.contentType) {
                case ContentType.Links:
                    Log.logInfo("ContentManager::importFromContentHub", "ContentType is Link: " + url.toString())
                    ComposerActions.appendTextToBody(url.toString())
                    break;
                case ContentType.Text:
                    Log.logInfo("ContentManager::importFromContentHub", "ContentType is Text: " + text)
                    ComposerActions.appendTextToBody(text)
                    break;
                default:
                    Log.logInfo("ContentManager::importFromContentHub", "ContentType is Other, most likely a file: " + url.toString())
                    ComposerActions.addFileAttachment(url.toString())
                    break;
                }
            }
        }
    }
}

