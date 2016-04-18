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
