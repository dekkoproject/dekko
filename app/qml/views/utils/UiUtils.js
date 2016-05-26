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

