#!/usr/bin/env bash

set -e

docker run \
    -it \
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
    monolinux-jiffy bash -c "source setup.sh && bash"
