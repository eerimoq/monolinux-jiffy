#!/usr/bin/env bash

set -e

docker run \
    -it \
    --rm \
    --user $(id -u):$(id -g) \
    --group-add dialout \
    --workdir=$PWD \
    --net host \
    --privileged \
    -v /home/$USER:/home/$USER \
    -v /etc/group:/etc/group:ro \
    -v /etc/passwd:/etc/passwd:ro \
    -v /etc/shadow:/etc/shadow:ro \
    -v /dev/bus/usb:/dev/bus/usb \
    eerimoq/monolinux-jiffy:0.6 bash -c "source setup.sh && bash"
