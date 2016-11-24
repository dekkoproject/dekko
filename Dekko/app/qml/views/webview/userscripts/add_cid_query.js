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
var query = "";
var blockRemoteResources = true;

// MessageTypes
var debugType = "DEBUG";
var queryMsg = "QUERY";
var remoteMsg = "REMOTE_ACCESS";

function testScheme(scheme, url) {
    return url.indexOf(scheme) === 0;
}

function isAllowedScheme(url) {
    return testScheme("cid:", url) || testScheme("dekko-part:", url) || testScheme("dekko-msg:", url);
}

function returnMessage(msgType, url, msgVal) {
    oxide.sendMessage({type: msgType, url: url, value: msgVal});
}

oxide.onMessage = function(msg) {
    switch(msg.type) {
    case "QUERY":
        query = msg.value;
        break;
    case "REMOTE_ACCESS":
        blockRemoteResources = msg.value;
        break;
    }
};

exports.onBeforeURLRequest = function(event) {
    // For debugging only.
//    returnMessage(debugType, event.url, "");
    var urlString = event.url.toString();
    if (isAllowedScheme(urlString)) {
        if (testScheme("cid:", urlString)) {
            if (urlString.indexOf(query) === -1) {
                event.redirectUrl = event.url + query;
            }
        }
    } else {
        if (blockRemoteResources) {
            event.cancelRequest();
            returnMessage(remoteMsg, event.url, true);
        }
    }
};
