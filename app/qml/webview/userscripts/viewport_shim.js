// ==UserScript==
// @name           Dekko message body style
// @namespace      http://dekkoproject.org
// @description    Oxide UserScript to add local font style to html message bodies
// ==/UserScript==

function addViewPortShim() {

    var x = document.querySelector('meta[name="viewport"]');
    if (x) {
      console.log("VIEW PORT EALREADY EXISTS");
    } else {
        var meta = document.createElement('meta');
        meta.name = "viewport";
        meta.content = "width=device-width, initial-scale=1";
        document.getElementsByTagName('head')[0].appendChild(meta);
    }
}

addViewPortShim();

