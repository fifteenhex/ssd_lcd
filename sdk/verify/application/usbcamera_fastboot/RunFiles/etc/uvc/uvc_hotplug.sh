#!/bin/sh
if [ $ACTION = "add" ]; then
    echo $DEVNAME plug in > /dev/ttyS0
else
    echo $DEVNAME plug out > /dev/ttyS0
fi
