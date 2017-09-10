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
pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

//==========================================================================\\
// Dialog queue API                                                         \\
// All dialogs get pushed onto a queue mainly because we may hit            \\
// more than one error at any point and the dialogs                         \\
// end up being stacked on top of each other                                \\
// This api queues custom or predefined Dialogs                             \\
// that get displayed one at a time. See PopupQueue.qml and DialogQueue.qml \\
// for how this works                                                       \\
// Note: all dialogs must inherit from DialogBase.qml                       \\
//==========================================================================\\

/*!
* @ingroup qml_actions
*/
ActionCreator {

    signal showError(string key, string error)
    signal showNotice(string key, string notice)
    signal queueDialog(string key, var dlg, var properties)
    signal showConfirmationDialog(string key, string id, string title, string text)
    signal confirmationDialogConfirmed(string id)
    signal confirmationDialogCancelled(string id)
    signal clearDialogQueue()// TODO
    signal cancelCurrentDialog()// TODO
    signal showDiscardDialog(string id, string title, string text)
}
