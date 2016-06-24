# Coding Style                         {#dev_codestyle}

Note: Alot of the current source doesn't yet follow alot of this style guide. But going forward efforts will be made to bring the source inline. So please try to stick to this guide with new code additions

## Source files

* Indentations should be four spaces
* Source should be encoded in UTF-8
* Try to keep line length below 120 chars (this is not a hard limit if it makes sense to break it)
* All source files should begin with a license header similar to the [one found here](license.md)

## Qt/C++

### Namespaces

All Dekko related modules are located in the `Dekko::` namespace. All submodule namespaces should be part of the include guard.

Example
```cpp
#ifndef ACCOUNTS_UTILS_H
#define ACCOUNTS_UTILS_H

#include <aheader>

namespace Dekko {
namepace Accounts { // first part of incl. guard
class Utils { // final part on incl. guard
public:
	void blah();
};
}
}
#endif // ACCOUNTS_UTILS_H
```

Changes modules should include the top level namespace in the include guard.

### Includes & Foward declarations

Should be sorted alphabetically and includes **above** forward declarations.

```cpp
#include <IncludeA>
#include <IncludeB>

class ForwardA;
class ForwardB;
```
### C++ Header full example

```cpp
/* Copyright (C) 2014-2016 Dan Chapman dpniel@ubuntu.com

This file is part of Dekko email client for Ubuntu devices

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.
*/
#ifndef ACCOUNTS_UTILS_H
#define ACCOUNTS_UTILS_H

#include <aheader>
#include <QObject>

namespace Dekko
{
namepace Accounts
{
class Utils: public QObject
{
	Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_ENUMS(Mode)

public:
	explicit Utils(QObject *parent = 0);
    ~Utils();
	// public enums after constructor
    enum Mode {
    	FIRST,
        SECOND,
        THIRD
    }

    // qproperty getters
	Mode mode() const;

	// QML invokable functions
	Q_INVOKABLE int getAge(const Account *account);

    // c++ public functions last
    bool migrateAccount(const Account *account);

signals:
	void modeChanged();

public slots:
	// qproperty setters
    void setMode(const Mode &mode);

private:
	// private members start with m_*
	Mode m_mode;
};
}
}
#endif // ACCOUNTS_UTILS_H
```

### C++ Code full example

```cpp
/* Copyright (C) 2014-2016 Dan Chapman dpniel@ubuntu.com

This file is part of Dekko email client for Ubuntu devices

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include Utils.h

namespace Dekko
{
namespace Accounts
{

Utils(QObject *parent): QObject(parent), m_mode(Mode::FIRST)
{
}

Mode mode() const {
	return m_mode;
}

int getAge(const Account *account) {
	Q_UNUSED(account);
    return 100; // haha!
}

bool migrateAccount(const Account *account) {
	if (account.isValid()) {
    	return account->migrate();
    } else {
    	qFatal("Use a valid account stupid!");
    }
    Q_UNREACHABLE(true)
    // Trivial blocks can use the compact format & drop the curly braces
    if (account.isValid())
    	return;
}

void setMode(const Mode &mode) {
	if (mode != m_mode) {
    	m_mode = mode;
        emit modeChanged();
    }
}
}
}
```

## QML

Dekko follows [Qt's QML coding conventions](http://doc.qt.io/qt-4.8/qml-coding-conventions.html) with a few additions. I've include it all her for offline viewing.

### QML Objects

Through our documentation and examples, QML objects are always structured in the following order:

* id
* property declarations
* signal declarations
* JavaScript functions
* object properties
* child objects
* states
* transitions

For better readability, we separate these different parts with an empty line.

For example, a hypothetical photo QML object would look like this:

```javascript
Rectangle {
    id: photo // id on the first line

	// ###### property declarations ######
    property bool thumbnail: false
    property alias image: photoImage.source

	// ###### signal declarations ######
    signal clicked() // Dekko includes the brackets

	// ###### javascript functions ######
    function doSomething(x){ // opening brace inline
        return x + photoImage.width
    }

	// ###### object properties ######
    color: "gray"
    x: 20; y: 20; height: 150 // try to group related properties together
    width: {
        if(photoImage.width > 200){
            photoImage.width;
        }else{
            200;
        }
    }

	// ###### child objects ######
    Rectangle {
        id: border
        anchors.centerIn: parent; color: "white"

        Image { id: photoImage; anchors.centerIn: parent }
    }

	// ###### states ######
    states: State {
        name: "selected"
        PropertyChanges { target: border; color: "red" }
    }

	// ###### transitions ######
    transitions: Transition {
        from: ""; to: "selected"
        ColorAnimation { target: border; duration: 200 }
    }
}
```
### Grouped properties

If using multiple properties from a group of properties, we use the group notation rather than the dot notation to improve readability.

```javascript
Rectangle {
    anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: 20 }
}

Text {
    text: "hello"
    font { bold: true; italic: true; pixelSize: 20; capitalization: Font.AllUppercase }
}
```
### Private properties

QML and JavaScript do not enforce private properties like C++. There is a need to hide these private properties, for example, when the properties are part of the implementation. To effectively gain private properties in a QML Item, the convention is to add a QtObject{} child to contain the properties. This shields them from being accessed outside the file in QML and JavaScript. As it involves the creation of another object, it is more expensive than just creating a property. To minimize the performance cost, try to group all private properties in one file into the same QtObject.

```javascript
Item {
    id: component
    width: 40; height: 50
    QtObject {
        id: d
        property real area: width * height * 0.5    //not meant for outside use
    }
}
```
### Lists

If a list contains only one element, we generally omit the square brackets.

```javascript
states: State {
    name: "open"
    PropertyChanges { target: container; width: 200 }
}
```

## Javascript

If the script is a single expression, we recommend writing it inline:

```javascript
Rectangle { color: "blue"; width: parent.width / 3 }
```

If the script is only a couple of lines long, we generally use a block:

```javascript
Rectangle {
    color: "blue"
    width: {
        var w = parent.width / 3
        console.debug(w)
        return w
    }
}
```

If the script is more than a couple of lines long or can be used by different objects, we recommend creating a function and calling it like this:

```javascript
function calculateWidth(object){
    var w = object.width / 3
    // ...
    // more javascript code
    // ...
    console.debug(w)
    return w
}

Rectangle { color: "blue"; width: calculateWidth(parent) }
```
For long scripts, we will put the functions in their own JavaScript file and import it like this:

```javascript
import "myscript.js" as Script

Rectangle { color: "blue"; width: Script.calculateWidth(parent) }
```

In Dekko we try to use as little javascript as possible. And a general rule is if a javscript function is getting too large that it should probably go into a seperate file like above. Then it is better to do it in c++ and expose it via singleton class or a utility class that's already exposed to QML.
