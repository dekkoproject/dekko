import QtQuick 2.4

// Now i'm a lover of toast so I couldn't resist to
// use toast terminology in this component!
// Should probably rename the component to Toaster :-D
QtObject {

    readonly property bool toastPopped: __toastPoppedUp
    readonly property int count: __rounds.length

    function orderToast(consumer, breadType, condiments) {
        __consumer.push(consumer)
        __rounds.push(breadType)
        __condiments.push(condiments)
        toaster.start()
    }

    property bool __toastPoppedUp: false
    // Because i'm british you order toast in "rounds" which is basically
    // a single slice of bread to a round.
    property var __rounds: []
    // Condiments are what you put on your toast. Jam/Marmalade or the daddy of all condiments "marmite"
    // Condiments are the applied properties to the toast.
    property var __condiments: []
    // Consumer is usually the parent aka "me" but sometimes children can nab a round.
    // Much to my detest!
    // Consumer should be the component the round belongs too.
    property var __consumer: []

    // brownness timer. How long do you want to expose the rounds to the
    // effects of the heating bars.
    property Timer __timer: Timer {
        id: toaster
        interval: 20
        onTriggered: {
            // do we have more rounds to toast?
            if (!__toastPoppedUp && __rounds.length) {
                __serveToast(__consumer.shift(), __rounds.shift(), __condiments.shift())
            }/* else {
                toaster.start()
            }*/
        }
    }

    function __serveToast(consumer, toast, condiments) {
//        var c = Qt.createComponent(toast)
        var t = toast.createObject(consumer, condiments)
        t.closed.connect(__toastCollected)
    }

    function __toastCollected() {
        __toastPoppedUp = false
        // start the next order if there is one
        toaster.start()
    }
}

