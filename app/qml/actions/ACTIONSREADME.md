This directory contains Dekko's complete QML API.

The API has been broken up into related groups (subdirs) and each of the 
*Actions.qml files are exported as a singleton and contains the functions 
available to use. i.e MessageActions.openMessage(msgId)

There are listeners for these actions throughout Dekko to actually perform the action.
Some actions may require more than one listener if it affects state in more than one place
If you wish to listen on one of these event then use an AppListener and Filter
component and listen on the actions key value. The key values match the function
name you can see these the the *Keys.qml files.

```
AppListener {
    Filter {
        type: MessageKeys.openMessage
        onDispatched: {
            console.log("Message opened: ", message.msgId)
        }
    }
}
```
