import QtQuick 2.4
import Ubuntu.Components 1.3

// Modified version of InputHandler from the UITK
MultiPointTouchArea {
    id: overlay

    // PROPERTIES
    property Item root
    property TextEdit textEdit
    property Flickable flickable
    property Item visibleArea: Item {
        parent: flickable
        anchors.fill: parent
    }
    property real lineSpacing: units.dp(3)
    property real lineSize: textEdit.font.pixelSize + lineSpacing
    property point frameDistance: Qt.point(flickable.x, flickable.y)
    property string oldText: ""
    property var flickableList: new Array()
    property bool textChanged: false
    property var popover
    property bool suppressReleaseEvent: false
    property int pressedPosition: -1
    property int moveStarts: -1
    property int moveEnds: -1
    property bool doubleTapInProgress: doubleTap.running
    property Item cursorPositionCursor: null
    property Item selectionStartCursor: null
    property Item selectionEndCursor: null

    // SIGNALS
    signal pressAndHold(int pos, bool fromTouch)
    signal tap(int pos)
    signal textModified()

    //SLOTS

    touchPoints: TouchPoint {
        id: touchPoint
        function touch() {
            longTap.restart();
            if (!doubleTap.running) {
                doubleTap.restart();
            } else if (doubleTap.tapCount > 0) {
                doubleTap.running = false;
                handleDblClick(touchPoint, true);
            }
        }
        function untouch() {
            longTap.running = false;
        }
        function reset() {
            longTap.running = false;
            doubleTap.running = false;
        }
    }

    onPressed: handlePressed(touchPoints[0])
    onReleased: handleReleased(touchPoints[0])

    // Mouse handling
    Mouse.forwardTo: [root]
    Mouse.onPressed: handlePressed(mouse)
    Mouse.onReleased: handleReleased(mouse)
    Mouse.onPositionChanged: handleMove(mouse)
    Mouse.onDoubleClicked: handleDblClick(mouse)
    Keys.onMenuPressed: overlay.pressAndHold(input.cursorPosition, false);
    // PageUp and PageDown handling
    Keys.onPressed: {
        if (event.key === Qt.Key_PageUp && event.modifiers === Qt.NoModifier) {
            movePage(false);
        } else if (event.key === Qt.Key_PageDown && event.modifiers === Qt.NoModifier) {
            movePage(true);
        }
    }

    // FUNCTIONS
    function activateInput() {
        if (!textEdit.activeFocus) {
            textEdit.forceActiveFocus();
        }
        showInputPanel();
    }

    function showInputPanel() {
        if (!UbuntuApplication.inputMethod.visible) {
            UbuntuApplication.inputMethod.show();
        }
        textChanged = false;
    }

    // ensures the text cursorRectangle is always in the internal Flickable's visible area
    function ensureVisible(rect)
    {
        var headHeight = root.headerHeight + root.frameSpacing + units.gu(2)
        if (flickable.moving || flickable.flicking)
            return;
        if (flickable.contentX >= rect.x)
            flickable.contentX = rect.x;
        else if (flickable.contentX + flickable.width <= rect.x + rect.width)
            flickable.contentX = rect.x + rect.width - flickable.width;
        if (flickable.contentY >= rect.y)
            flickable.contentY = rect.y;
        else if (flickable.contentY + (flickable.height - headHeight) <= rect.y + rect.height)
            flickable.contentY = rect.y + rect.height - (flickable.height - headHeight);
    }

    // returns the cursor position from x,y pair
    function unadulteratedCursorPosition(x, y) {
        return textEdit.positionAt(x, y, TextInput.CursorOnCharacter);
    }

    // returns the cursor position taking frame into account
    function cursorPosition(x, y) {
        var frameSpacing = root.frameSpacing;
        var cursorPosition = unadulteratedCursorPosition(x, y);
//        if (cursorPosition === 0)
//            cursorPosition = unadulteratedCursorPosition(x + frameSpacing, y + frameSpacing);
//        if (cursorPosition === textEdit.text.length)
//            cursorPosition = unadulteratedCursorPosition(x - frameSpacing, y - frameSpacing);
        return cursorPosition
    }

    // returns the mouse position
    function mousePosition(mouse) {
        return cursorPosition(mouse.x, mouse.y);
    }
    // checks whether the position is in the selected text
    function positionInSelection(pos) {
        return (textEdit.selectionStart !== textEdit.selectionEnd)
                && (pos >= Math.min(textEdit.selectionStart, textEdit.selectionEnd))
                && (pos <= Math.max(textEdit.selectionStart, textEdit.selectionEnd));
    }

    // check whether the mouse is inside a selected text area
    function mouseInSelection(mouse) {
        var pos = mousePosition(mouse);
        return positionInSelection(pos);
    }

    // selects text
    function selectText(mouse) {
        state = "select";
        moveEnds = mousePosition(mouse);
        if (moveStarts < 0) {
            moveStarts = moveEnds;
        }
        textEdit.select(moveStarts, moveEnds);
    }

    // returns the first Flickable parent of a given item
    function firstFlickableParent(item) {
        var p = item ? item.parent : null;
        while (p && !p.hasOwnProperty("flicking")) {
            p = p.parent;
        }
        return p;
    }

    // focuses the input if not yet focused, and shows the context menu
    function openContextMenu(mouse, noAutoselect) {
        var pos = mousePosition(mouse);
        if (!root.focus || !mouseInSelection(mouse)) {
            activateInput();
            textEdit.cursorPosition = pressedPosition = mousePosition(mouse);
            if (!noAutoselect) {
                textEdit.selectWord();
            }
        }
        // open context menu at the cursor position
        overlay.pressAndHold(textEdit.cursorPosition, mouse.touch);
        // if opened with left press (touch falls into this criteria as well), we need to set state to inactive
        // so the mouse moves won't result in selected text loss/change
        if (mouse.button === Qt.LeftButton) {
            state = "inactive";
        }
    }

    // disables interactive Flickable parents, stops at the first non-interactive flickable.
    function toggleFlickablesInteractive(turnOn) {
        flickable.interactive = turnOn
    }

    // moves the specified position, called by the cursor handler
    // positioner = "currentPosition/selectionStart/selectionEnd"
    function positionCaret(positioner, x, y) {
        if (positioner === "cursorPosition") {
            textEdit[positioner] = cursorPosition(x, y);
        } else {
            var pos = cursorPosition(x, y);
            if (positioner === "selectionStart" && (pos < textEdit.selectionEnd)) {
                textEdit.select(pos, input.selectionEnd);
            } else if (positioner === "selectionEnd" && (pos > textEdit.selectionStart)) {
                textEdit.select(textEdit.selectionStart, pos);
            }
        }
    }

    // moves the cursor one page forward with or without positioning the cursor
    function movePage(forward) {
        var cx = textEdit.cursorRectangle.x;
        var cy = textEdit.cursorRectangle.y;
        if (forward) {
            cy += visibleArea.height;

        } else {
            cy -= visibleArea.height;
        }
        textEdit.cursorPosition = cursorPosition(cx, cy);
    }

    // touch and mous handling
    function handlePressed(event) {
        if (event.touch) {
            // we do not have longTap or double tap, therefore we need to generate those
            event.touch();
        } else {
            // consume event so it does not get forwarded to the input
            event.accepted = true;
        }
        // remember pressed position as we need it when entering into selection state
        pressedPosition = mousePosition(event);
    }
    function handleReleased(event) {
        if (event.touch) {
            event.untouch();
        }
        if ((!root.focus && !root.activeFocusOnPress) || suppressReleaseEvent === true) {
            suppressReleaseEvent = false;
            return;
        }

        activateInput();
        if (state === "" || event.touch) {
            textEdit.cursorPosition = mousePosition(event);
        }
        moveStarts = moveEnds = -1;
        state = "";
        // check if we get right-click from the frame or the area that has no text
        if (event.button === Qt.RightButton) {
            // open the popover
            overlay.pressAndHold(textEdit.cursorPosition, event.touch);
        } else {
            overlay.tap(textEdit.cursorPosition);
        }
    }
    function handleMove(event) {
        // leave if not focus, not the left button or not in select state
        if (!textEdit.activeFocus || (!event.touch && event.button !== Qt.LeftButton) || !textEdit.selectByMouse) {
            return;
        }
        selectText(event);
    }
    function handleDblClick(event) {
        if (root.selectByMouse) {
            openContextMenu(event, false);
            // turn selection state temporarily so the selection is not cleared on release
            state = "selection";
            suppressReleaseEvent = true;
        }
    }


    // COMPONENTS

    // brings the state back to default when the component looses focus
    // and ensures input has active focus when component regains focus
    Connections {
        target: root
        ignoreUnknownSignals: true
        onActiveFocusChanged: {
            textEdit.focus = root.activeFocus;
        }
        onKeyNavigationFocusChanged: {
            if (root.keyNavigationFocus) {
                textEdit.forceActiveFocus();
            }
        }
        onFocusChanged: {
            UbuntuApplication.inputMethod.commit()
            state = (root.focus) ? "" : "inactive";
            if (root.focus) {
                textEdit.forceActiveFocus()
            }
        }
        onVisibleChanged: {
            if (!root.visible)
                root.focus = false;
        }
    }

    // input specific signals
    Connections {
        target: textEdit
        onCursorRectangleChanged: ensureVisible(textEdit.cursorRectangle)
        onTextChanged: {
            textChanged = true;
            if (oldText != textEdit.text) {
                textModified()
                oldText = textEdit.text
            }
        }
        // make sure we show the OSK
        onActiveFocusChanged: {
            if (!textEdit.activeFocus && popover)
                PopupUtils.close(popover);
            showInputPanel();
        }
    }

    // inner or outer Flickable controlling
    Connections {
        target: flickable
        // turn scrolling state on
        onFlickStarted: toggleScrollingState(true)
        onMovementStarted: toggleScrollingState(true)
        // reset to default state
        onMovementEnded: toggleScrollingState(false)

        function toggleScrollingState(turnOn) {
            if (!root.focus) {
                return;
            }
            overlay.state = (turnOn) ? "scrolling" : ""
        }
    }

    // cursors to use when text is selected
    Connections {
        target: textEdit
        onSelectedTextChanged: {
            if (textEdit.selectedText !== "") {
                if (!selectionStartCursor) {
                    selectionStartCursor = textEdit.cursorDelegate.createObject(
                                textEdit, {
                                    "positionProperty": "selectionStart",
                                    "handler": overlay,
                                }
                                );
                    moveSelectionCursor(selectionStartCursor);
                    selectionEndCursor = textEdit.cursorDelegate.createObject(
                                textEdit, {
                                    "positionProperty": "selectionEnd",
                                    "handler": overlay,
                                }
                                );
                    moveSelectionCursor(selectionEndCursor);
                }
            } else {
                if (selectionStartCursor) {
                    selectionStartCursor.destroy();
                    selectionStartCursor = null;
                    selectionEndCursor.destroy();
                    selectionEndCursor = null;
                }
            }
        }
        onSelectionStartChanged: moveSelectionCursor(selectionStartCursor, true);
        onSelectionEndChanged: moveSelectionCursor(selectionEndCursor, true);

        function moveSelectionCursor(cursor, updateProperty) {
            if (!cursor) {
                return;
            }
            // workaround for https://bugreports.qt-project.org/browse/QTBUG-38704
            // selectedTextChanged signal is not emitted for TextEdit when selectByMouse is false
            if (updateProperty && QuickUtils.className(textEdit) === "QQuickTextEdit") {
                textEdit.selectedTextChanged();
            }

            var pos = textEdit.positionToRectangle(textEdit[cursor.positionProperty]);
            cursor.x = pos.x;
            cursor.y = pos.y;
            cursor.height = pos.height;
            ensureVisible(pos);
        }
    }

    // right button handling
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        // trigger pressAndHold
        onReleased: openContextMenu(mouse, true)
        cursorShape: Qt.IBeamCursor
    }

    Timer {
        id: doubleTap
        property int tapCount: 0
        interval: 400
        onRunningChanged: {
            tapCount = running
        }
    }

    Timer {
        id: longTap
        interval: 800
        onTriggered: {
            // do not open context menu if the input is not focus
            if (!root.focus) {
                return;
            }

            // do not open context menu if this is scrolling
            if (touchPoint.startY - touchPoint.y < -units.gu(2))
                return;

            openContextMenu(touchPoint, false);
            suppressReleaseEvent = true;
        }
    }

    // STATES
    states: [
        // override default state to turn on the saved Flickable interactive mode
        State {
            name: ""
            StateChangeScript {
                // restore interactive for all Flickable parents
                script: toggleFlickablesInteractive(true);
            }
        },
        State {
            name: "inactive"
        },
        State {
            name: "scrolling"
            StateChangeScript {
                script: {
                    // stop touch timers
                    touchPoint.reset();
                }
            }
        },
        State {
            name: "select"
            // during select state all the flickables are blocked (interactive = false)
            // we can use scroller here as we need to disable the outer scroller too!
            PropertyChanges {
                target: flickable
                interactive: false
            }
            StateChangeScript {
                script: {
                    // turn off interactive for all parent flickables
                    toggleFlickablesInteractive(false);
                    if (!positionInSelection(pressedPosition)) {
                        textEdit.cursorPosition = pressedPosition;
                    }
                }
            }
        }
    ]
}
