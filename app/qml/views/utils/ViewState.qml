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

Item {
    id: viewState
    anchors.fill: parent

    readonly property real gu40: units.gu(40)
    readonly property real gu50: units.gu(50)
    readonly property real gu90: units.gu(90)
    // These 2 aren't the exact values but 5gu less as it looks a little better
    readonly property real gu130: units.gu(125) // gu90 + gu40
    readonly property real gu140: units.gu(135) // gu90 + gu50
    readonly property bool isSmallFF: state === "small"
    readonly property bool isMediumFF: state === "medium"
    readonly property bool isLargeFF: state === "large"
    // This is possibly a bit dumb but it should do the job :-/
    readonly property bool isLandscape: height < width

    states: [
        // Phone/tablet (portrait)
        State {
            name: "small"
            when: width <= gu90
        },
        // tablet (landscape)
        State {
            name: "medium"
            when: width > gu90 && width <= gu140
        },
        // Laptop/Desktop size screen
        State {
            name: "large"
            when: width > gu140
        }
    ]
}
