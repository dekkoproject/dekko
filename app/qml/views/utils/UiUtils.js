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
.pragma library
// original avatar colors
//var avatarColors = ["#6b9362", "#4d8fac", "#bb7796", "#b95754", "#ffa565", "#f7bb7d", "#bda928"]
// Possibly good for dark theme
//var avatarColors = ["#bc97ff", "#e8abb7", "#7dabff", "#88cfbc", "#b1f9a5"]
// Nice new ones :-D no more pukey greeny/brown
var avatarColors = ["#797DC3","#88a825", "#3A8891", "#cf4a30", "#ed8c2b", "#5E82BD"]

function getIconColor(name) {
    var tmp = 0;
    for (var i = 0; i < name.length; i++) {
        tmp += name.charCodeAt(i);
    }
    return avatarColors[tmp % avatarColors.length];
}

