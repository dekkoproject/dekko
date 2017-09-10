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
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0

// Error manager is responsible for listening for and ordering display of error messages
// from the Client and other dispatched error actions.
Item {

    Connections {
        target: Client
        onClientError: {
            Log.logError("ErrorManager::clientError", "Houston we have a problem!!")
            switch(error) {
            case Client.NoError:
                Log.logError("ErrorManager::NoError", "Few! false alarm. Displaying a toast if there is an error string")
                if (errorString != "") {
                    ViewActions.orderSimpleToast(ViewKeys.toastMainWindow, errorString)
                }
                break
            case Client.FrameworkFault:
                Log.logError("ErrorManager::FrameworkFault", "We have a framework fault: " + errorString)
                // shout loudly
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.SystemError:
                Log.logError("ErrorManager::SystemError", "We have a system fault: " + errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.InternalServerError:
                Log.logError("ErrorManager::InternalServerError", "Error in the remote server: " + errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.UnexpectedResponse:
                Log.logError("ErrorManager::UnexpectedResponse", "Unexpected response received: " + errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.LoginFailed:
                Log.logError("ErrorManager::LoginFailed", "Login failed, maybe check credentials??: " + errorString)
                // TODO: show a special login failed dialog with some advice to check credentials.
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.CancelError:
                Log.logError("ErrorManager::CancelError", "Cancelation error " + errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.FileSystemFull:
                Log.logError("ErrorManager::FileSystemFull", "File system is full, time to free up some space me thinks: " + errorString)
                // TODO: once we have the cache pruner should we offer to prune old messages to free up some space?
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.MessageNotExist:
                Log.logError("ErrorManager::MessageNotExist", "Message doesn't exist, maybe it got removed/moved on the server and we didn't find out: " + errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.EnqueueFailed:
                Log.logError("ErrorManager::EnqueueFailed", "Failed to enqueue: " + errorString)
                break
            case Client.NoConnection:
                Log.logError("ErrorManager::NoConnection", errorString)
                ViewActions.orderSimpleToast(ViewKeys.toastMainWindow, qsTr("No connection"))
                break
            case Client.ConnectionInUse:
                Log.logError("ErrorManager::ConnectionInUse", errorString)
                ViewActions.orderSimpleToast(ViewKeys.toastMainWindow, qsTr("No connection"))
                break
            case Client.ConnectionNotReady:
                Log.logError("ErrorManager::ConnectionNotReady",  errorString)
                ViewActions.orderSimpleToast(ViewKeys.toastMainWindow, qsTr("Connection unavailable"))
                break
            case Client.ConfigurationError:
                Log.logError("ErrorManager::ConfigurationError", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.InvalidAddress:
                Log.logError("ErrorManager::InvalidAddress", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.InvalidData:
                Log.logError("ErrorManager::invalidData", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.TimeoutError:
                Log.logError("ErrorManager::TimoutError", errorString);
                ViewActions.orderSimpleToast(ViewKeys.toastMainWindow, qsTr("Action timed out"))
                break
            case Client.InternalStateReset:
                Log.logError("ErrorManager::InternalStateReset", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.SslNotSupported:
                Log.logError("ErrorManager::SslNotSupported", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.UntrustedCertificate:
                Log.logError("ErrorManager::UntrustedCertificate", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            case Client.UnknownError:
                Log.logError("ErrorManager::UnknownError", errorString)
                PopupActions.showError(PopupKeys.popupMainWindow, errorString)
                break
            }
        }
    }
}
