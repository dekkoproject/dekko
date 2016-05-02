

function appendStyle(cssStyle) {
    var head, style;
    head = document.getElementsByTagName('head')[0];
    if (!head) { return; }
    style = document.createElement('style');
    style.type = 'text/css';
    style.innerHTML = cssStyle;
    head.appendChild(style);
}

function addGlobalStyle() {
    var overflowStyle = 'body {overflow: hidden;}';
    appendStyle(overflowStyle);
}

//addFontShim();

addGlobalStyle();
