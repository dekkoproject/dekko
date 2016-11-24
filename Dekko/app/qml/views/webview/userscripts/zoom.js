// ==UserScript==
// @name           Dekko message body zoom level
// @namespace      http://dekkoproject.org
// @description    Oxide UserScript to adjust the zoom level of the message body
// ==/UserScript==

oxide.addMessageHandler("ZOOM_LEVEL", function(zoom) {
    document.body.style.zoom = zoom.args["level"]
});
