# Notify

Notify is a simple component for displaying in app notifications on Ubuntu devices.

It depends on the Ubuntu UI Toolkit for it's functionality but it's easily 
modified to work with QtQuick.COntrols or some other toolkit.

## Usage

Usage is pretty simple just add the `Notify` component to your `MainView`
and use the `Notify::send()` function to display notifications

```qml
import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Notify 1.0

MainView {
    Page {
        title: "Notify"
        
        Button {
            anchors.centerIn: parent
            text: "Send"
            onClicked: notify.send("Hello, World", "With a subtitle as well")
        }
    }
    Notify {
        id: notify
    }
}
```

Notify comes with a default styled Notification. but this can easily be replaced
with a custom notification component to suite your style. This can be done by
using a modified `NotifyConfiguration` component and either set the configuration
directly on the `Notify` component or pass the configuration as the final argument
to `Notify::send()`. New notification components should inherit from `NotificationBase`
which you can see an example of in the examples directory.

The examples directory shows a good range of all supported functionality.