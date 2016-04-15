// ==UserScript==
// @name           Dekko message body style
// @namespace      http://dekkoproject.org
// @description    Oxide UserScript to add local font style to html message bodies
// ==/UserScript==

var plainTextStyle = 'body { font: 1.1rem "Ubuntu"; color: #5D5D5D; line-height: 1.33; margin.left: 16px; margin.right: 16px; }a:link { color: #19b6ee } a:visited { color: #19b6ee } a:hover { color: #19b6ee }';
var htmlStyle = 'body { font: 0.9rem "Ubuntu"; font-weight: 100; color: #5D5D5D; line-height: 1.33; margin.left: 16px; margin.right: 16px; }a:link { color: #19b6ee } a:visited { color: #19b6ee } a:hover { color: #19b6ee }';
var ubuntuFontShim = '@font-face { font-family: "Ubuntu"; font-style: normal; font-weight: 300; src: local("Ubuntu Light"), local("Ubuntu-Light"), url(../fonts/ubuntu-light.woff) format("woff"); }'

function appendStyle(cssStyle) {
    var head, style;
    head = document.getElementsByTagName('head')[0];
    if (!head) { return; }
    style = document.createElement('style');
    style.type = 'text/css';
    style.innerHTML = cssStyle;
    head.appendChild(style);
}

function addFontShim() {
    /* Workarround to load the Ubuntu Light font pad.lv/1186372 */
    appendStyle(ubuntuFontShim);
}

function addGlobalStyle() {
    var globalStyle
    var desc = document.querySelector('meta[name="description"]');
    if (desc && desc.content === "plaintext") {
        globalStyle = plainTextStyle;
    } else {
        globalStyle = htmlStyle;
    }
    appendStyle(globalStyle);
}

//addFontShim();

addGlobalStyle();

