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

/*!
* @ingroup qml_actions
*/
ActionCreator {

    /* Displays a message given it's message id

       This will display the messageview in the correct column based on formfactor
       so you don't need to care about where or what you should load when calling
       this action
    */
    signal openMessage(int msgId)

    signal setCurrentMessage(int msgId)

    /* Opens the folder in the messagelist

       folderName: Readable description of the folder which will be used as the page title
       folderKey: is the key used to generate the messagelist
    */
    signal openFolder(string folderName, var folderKey)

    /* Displays folderlist view for accountId.

       accountName: Readable description of the account. used in the page title
       accountId: QMailAccountId

       This will display the folder list view either as a new page when in small/medium form factors
       or in the left column on large form factors.
    */
    signal openAccountFolder(string accountName, var accountId)

    /* Change the sort order of the message list view

       order: Can be either Qt.DescendingOrder or Qt.AscendingOrder

       Errors: will log an invalid sort order error and default to DescendingOrder
    */
    signal changeMessageListSortOrder(int order)

    /* Reset the message list as if it's being loaded for the first time.

       It does some clean up like reset selected indexes, repositions the listview etc
    */
    signal resetMessageList()// TODO

    /* Deletes a message with msgId

       Delete actions are not sent to the server immediately and are undoable.
       The delete action is queued and a timer is started for 5 seconds. Which during that time you
       can call undoRecentActions to restore the deleted message to previous state.

       Once the timer finishes the delete action is committed and sent to the server
    */
    signal deleteMessage(int msgId)

    /* Deletes the selected messages set using setMessageCheck when in multiselection state

       No messages will be deleted unless the msg list model is in selection mode
    */
    signal deleteSelectedMessages()

    /* Undo any recent message actions that are marked as undoable.

       You can check for undoable actions via MailStore.hasUndoableActions and MailboxStore.hasUndoableActions
    */
    signal undoRecentActions()

    /* Mark message with ID as read/unread */
    signal markMessageRead(int msgId, bool read)

    /* Marks any selected messages set using setMessageCheck when in multiselection state

       No message will be marked as read/unread unless the msg list model in in selection mode
    */
    signal markSelectedMessagesRead()

    /* Mark message with ID as as important/not-important */
    signal markMessageImportant(int msgId, bool important)

    /* Marks any selected messages set using setMessageCheck when in multiselection state

       No message will be marked as important/not-important unless the msg list model in in selection mode
    */
    signal markSelectedMessagesImportant()

    signal markMessageAsTodo(int msgId, bool todo)
    signal markMessageAsDone(int msgId, bool done)
    signal restoreMessage(int msgId)

    /* Select a ll messages when in multiselect */
    signal selectAllMessages()

    /* Unselect all messages when in multiselect */
    signal unselectAllMessages()

    // checked options are Qt.Checked/Unchecked
    signal setMessageCheck(int msgIndex, var checked)

    /* Add more messages to the MessageListModel */
    signal showMoreMessages()

    signal setMessageListFilter(int filter)

    signal rewindMessageListStack()

    signal startMultiSelection()

    signal endMultiSelection()

    signal resetFolderLists()

    signal stackRewound()

    signal sendPendingMessages()
}
