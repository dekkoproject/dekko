import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: viewState
    anchors.fill: parent

    readonly property real gu40: units.gu(40)
    readonly property real gu50: units.gu(50)
    readonly property real gu90: units.gu(90)
    readonly property real gu130: units.gu(130) // gu90 + gu40
    readonly property real gu140: units.gu(140) // gu90 + gu50
    readonly property bool isSmallFF: state === "small"
    readonly property bool isMediumFF: state === "medium"
    readonly property bool isLargeFF: state === "large"
    // This is possibly a bit dumb but it should do the job :-/
    readonly property bool isLandscape: height < width

    states: [
        // Phone/tablet (portrait)
        State {
            name: "small"
            when: width <= gu90
        },
        // tablet (landscape)
        State {
            name: "medium"
            when: width > gu90 && width <= gu140
        },
        // Laptop/Desktop size screen
        State {
            name: "large"
            when: width > gu140
        }
    ]
}
