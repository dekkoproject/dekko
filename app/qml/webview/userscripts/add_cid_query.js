var query = "";

oxide.onMessage = function(msg) {
    oxide.sendMessage({query: msg.query })
    query = msg.query;
};

exports.onBeforeURLRequest = function(event) {
    var urlString = event.url.toString();
    if (urlString.indexOf("cid:") === 0) {
        if (urlString.indexOf(query) === -1) {
            event.redirectUrl = event.url + query;
        }
    }
    // For debugging only.
    oxide.sendMessage({ url: event.url});
};
