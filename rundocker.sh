#!/usr/bin/env bash

set -e

docker run \
    -it \
    --rm \
    --user $(id -u):$(id -g) \
    --workdir=$PWD \
    --net host \
    --volume="/home/$USER:/home/$USER" \
    --volume="/etc/group:/etc/group:ro" \
    --volume="/etc/passwd:/etc/passwd:ro" \
    --volume="/etc/shadow:/etc/shadow:ro" \
    eerimoq/monolinux-jiffy:0.1 bash -c "source setup.sh && bash"
