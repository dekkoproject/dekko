### Setup ###
You need to have a few packages installed for this plugin to function correctly

First you need signon-plugin-sasl. This hasn't yet been packaged up for Ubuntu so you will
need to to build and install this manually.

```
#!bash
sudo apt-get build-dep signon-plugin-oauth2
sudo apt-get install libsasl2-dev
bzr branch lp:signon-plugin-sasl
cd signon-plugin-sasl
qmake
make -j4
sudo make install
```

The plugin uses the ubuntu connectivity api on the unity8 and falls back to querying Qnetworkaccessmanager on non
unity8 desktops.

```
#!bash
sudo apt-get install libconnectivity-qt1-dev
```

### To run the example app ###

The example app will run in the same way as if the OA had invoked the plugin and an
account will be created in accounts & credentials db's. This helps development/testing
on desktop as unity8 USSOA UI doesn't work all that great on unity7 and tends to hang loading
the plugin. Not entirely sure why.... :-/

```
#!bash
bzr branch lp:accounts-plugin-email && cd accounts-plugin-email
mkdir __build && cd __build
cmake ..
make -j4
make run
```

To run the unit tests it's the same as above apart from the make command should be

```
#!bash

make check
```

### Still TODO ###
* agree with others on default template settings available to all apps.
