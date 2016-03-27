# Setting up your development environment

This section outlines the steps to get a working development environment for Dekko.

## Setup launchpad account

First thing you need to do (if you haven't already) is setup you launchpad profile following the steps outlined here https://help.launchpad.net/YourAccount/NewAccount

Once done you should now be able to assign yourself to bugs and create project branches on launchpad.

## Install latest Ubuntu SDK

You need to add the sdk ppa and install

```bash
$ sudo add-apt-repository ppa:ubuntu-sdk-team/ppa
$ sudo apt update
$ sudo apt install ubuntu-sdk
```


## Clone Dekko project repository

Dekko uses git as it's version control system. There are many great tutorials on the web about using git but I will include the neccesary stuff throughout this guide.

First ensure you have git installed:

```bash
$ sudo apt-get install git
```

Now to save a bit of typing let's add a quick alias to to you local git config. In your home directory create a file called ```.gitconfig``` (if it doesn't already exist) and add the following to it.

```bash
[url "git+ssh://XXXXXX@git.launchpad.net/"]
        insteadof = lp:

[url "git+ssh://XXXXXX@git.launchpad.net/~XXXXXX/+git/"]
        insteadof = lpme:
```

Where ```XXXXXX``` is your launchpad user name. This saves *alot* of typing :-)

Now that's setup you can clone dekko's repo

```bash
$ git clone lp:dekko
```

(See how that handy little alias we created is working here)

Once this is done before making any changes create your own repository on launchpad that you can push your changes to. You need to add a new remote repository. I usually call mine "lp" as that's *my* launchpad repository but you can call it whatever you like. Dekko's main repository is called origin by default. So now you need to add your new remote and push the master branch to launchpad.

```bash
$ git remote add lp lpme:dekko
$ git push lp master
```
You should now see your personal repository listed under the "Other respositories" section here https://code.launchpad.net/dekko .

## Installing dependencies & setting up click chroots

The Ubuntu SDK uses the concept of "Kits" for building applications for different architecture. These kits consist of a click chroot and all related packages/api's to build Ubuntu applications. 

Dekko has some additional dependencies not included in the default kit so we need to install them after creating a chroot. To make life easier there's a script for it! 

From the root of your local Dekko repository run:

```bash
$ ./scripts/bootstrap.sh
```

By default this script will setup an armhf kit. Making it suitable for running on phones & tablets. If you are using the Ubunntu i386 emulator (I would advise not to use the armhf emulator) then you need to set the ```CLICK_SDK_ARCH``` environment variable when running the script.

```bash
$ CLICK_SDK_ARCH=i386 ./scripts/bootstrap.sh
```

If you just want to build & run Dekko on your desktop then you should use the run-desktop script with the ```--setup``` option.

```bash
$ ./scripts/run-desktop --setup
```

This will install all needed dependencies on your computer and not in a click chroot.

## Running on your desktop



Currently there is an issue the the Ubuntu SDK desktop kit that we can't run Dekko using QtCreator so we have to run it from the command line.

> Note: This has only been tested on Ubuntu 15.10 wily werewolf

With all Dekko's dependencies installed from the step before you now need to build and install the latest ubuntu ui toolkit by running the command

```bash
$ ./scripts/run-desktop --with-uitk
```

This will first build and install the ui toolkit then build and finally run dekko. You will be prompted for your password when installing any dependencies. The ```run-desktop``` script creates a build directory under ```dekko/__build```. You should only need to use the ```--with-uitk``` option once for the lifetime of the build directory. Each subsequent run of Dekko requires that you just run ```./scripts/run-desktop``` with no additional options.

If you wish to clean the build directory before a new build then just run.

```bash
$ ./scripts/run-desktop --clean
```

## Running on your phablet or emulator

This is somewhat easier to running on desktop as the Ubuntu Sdk IDE will take care of it all for you You need to make sure you have the armhf or i386 added to the project by going to the "Projects" tab on the left side of the IDE and add the correct kit. Then you need to click on the icon just above the green "Run" button at the bottom left of the IDE and make sure your kit and "dekko" (sometimes comes up as "dekko2") are selected. If all is good you can just hit "Run" :-) It will tell you if something goes wrong. 

## Running the test suite

To run the test suite use the run-desktop script and pass the --test flag

```bash
$ ./scripts/run-desktop --tests
```

