.pragma library

var avatarColors = ["#6b9362", "#4d8fac", "#bb7796", "#b95754", "#ffa565", "#f7bb7d", "#bda928"]

function getIconColor(name) {
    var tmp = 0;
    for (var i = 0; i < name.length; i++) {
        tmp += name.charCodeAt(i);
    }
    return avatarColors[tmp % avatarColors.length];
}

