# Setting up your development environment                         {#dev_setup}

This section outlines the steps to get a working development environment for Dekko.

## Setup Dekko project GitLab account

Head over to Dekko's [gitlab instance](http://code.dekkoproject.org) and create an account. Once the account is confirmed you need to add your ssh key so that you can push and pull branches.

Once done you should now be able to assign yourself to bugs and create project branches on gitlab.

## Install latest Ubuntu SDK

You need to add the sdk ppa and install

```bash
$ sudo add-apt-repository ppa:ubuntu-sdk-team/ppa
$ sudo apt update
$ sudo apt install ubuntu-sdk
```

## Clone Dekko project repository

Dekko uses git (gitlab) as it's version control system. There are many great tutorials on the web about using git but I will include the neccesary stuff throughout this guide.

First ensure you have git installed:

```bash
$ sudo apt-get install git
```
Now that's setup you can clone dekko's repo

```bash
$ git clone git@code.dekkoproject.org:dekko-dev/dekko.git
$ cd dekko
```

Dekko is made up of a few projects which are set as git submodules so you also need to initialize them.

```bash
$ git submodule update --init
```

## Installing dependencies & setting up click chroots

The Ubuntu SDK uses the concept of "Kits" for building applications for different architecture. These kits consist of a click chroot and all related packages/api's to build Ubuntu applications.

Dekko has some additional dependencies not included in the default kit so we need to install them after creating a chroot. To make life easier there's a script for it!

From the root of your local Dekko repository run:

```bash
$ ./scripts/bootstrap.sh
```

if you already have an armhf kit setup then you can just use the script to add the extra dependencies

```bash
$ ./scripts/bootstrap.sh update
```

By default this script will setup or update an armhf kit. Making it suitable for running on phones & tablets. If you are using the Ubunntu i386 emulator (I would advise not to use the armhf emulator) then you need to set the ```CLICK_SDK_ARCH``` environment variable when running the script.

```bash
$ CLICK_SDK_ARCH=i386 ./scripts/bootstrap.sh
```

This will install all needed dependencies on your computer and not in a click chroot.

## Running on your desktop

If you just want to build & run Dekko on your desktop then you should use the run-dekko2 script.

```bash
$ ./scripts/run-dekko2
```

Or to run in developer mode, which gives you a panel of application output at the bottom of the window.

```bash
$ ./scripts/run-dekko2 -d
```

To ease development on desktop a bit and work with all formfactors you can set the size of the window you
want when launching dekko. options are ```--small```, ```--medium``` and ```--large```. the default is large

```bash
$ ./scripts/run-dekko2 --small
```

If you wish to clean the build directory before a new build then just run.

```bash
$ ./scripts/run-dekko2 --clean
```

and to launch dekko into a gdb session use

```bash
$ ./scripts/run-dekko2 --gdb
```

## Running on your phablet or emulator

This isn't completely working yet and i will update this doc as soon as I have it running reliably on devices.

