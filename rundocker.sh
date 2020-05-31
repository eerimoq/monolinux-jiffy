#!/usr/bin/env bash

set -e

if [ "$#" -eq 0 ] ; then
    COMMAND="bash"
else
    COMMAND="bash -c \"$*\""
fi

docker run \
       --interactive \
       --tty \
       --rm \
       --user $(id -u):$(id -g) \
       --workdir=$PWD \
       --net host \
       --privileged \
       -v /home/$USER:/home/$USER \
       -v /etc/group:/etc/group:ro \
       -v /etc/passwd:/etc/passwd:ro \
       -v /etc/shadow:/etc/shadow:ro \
       -v /dev/bus/usb:/dev/bus/usb \
       eerimoq/monolinux-jiffy:0.13 bash -c "source setup.sh && $COMMAND"
