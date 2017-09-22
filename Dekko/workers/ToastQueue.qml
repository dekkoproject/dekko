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

// Now i'm a lover of toast so I couldn't resist to
// use toast terminology in this component!
// Should probably rename the component to Toaster :-D
QtObject {

    readonly property bool toastPopped: __toastPoppedUp
    readonly property int count: __rounds.length

    function orderToast(consumer, breadType, condiments) {
        __consumer.push(consumer)
        __rounds.push(breadType)
        __condiments.push(condiments)
        toaster.start()
    }

    property bool __toastPoppedUp: false
    // Because i'm british you order toast in "rounds" which is basically
    // a single slice of bread to a round.
    property var __rounds: []
    // Condiments are what you put on your toast. Jam/Marmalade or the daddy of all condiments "marmite"
    // Condiments are the applied properties to the toast.
    property var __condiments: []
    // Consumer is usually the parent aka "me" but sometimes children can nab a round.
    // Much to my detest!
    // Consumer should be the component the round belongs too.
    property var __consumer: []

    // brownness timer. How long do you want to expose the rounds to the
    // effects of the heating bars.
    property Timer __timer: Timer {
        id: toaster
        interval: 20
        onTriggered: {
            // do we have more rounds to toast?
            if (!__toastPoppedUp && __rounds.length) {
                __serveToast(__consumer.shift(), __rounds.shift(), __condiments.shift())
            }/* else {
                toaster.start()
            }*/
        }
    }

    function __serveToast(consumer, toast, condiments) {
        var c = Qt.createComponent(toast)
        var t = c.createObject(consumer, condiments)
        t.closed.connect(__toastCollected)
        __toastPoppedUp = true
    }

    function __toastCollected() {
        __toastPoppedUp = false
        // start the next order if there is one
        toaster.start()
    }
}

