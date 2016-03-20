#!/bin/bash

if [ "$1" == "clean" ]; then
    adb shell "rm -f /home/phablet/.cache/upstart/application-click-dekko.dekkoproject*.log"
    shift
fi

version="*"
if [ "$#" -gt 0 ]; then
    version=$1
    shift
fi

log="/home/phablet/.cache/upstart/application-click-dekko.dekkoproject_dekko_"$version".log"

adb shell "tail -n 200 --follow $log|grep -v lttng"
