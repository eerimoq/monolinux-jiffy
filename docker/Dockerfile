FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

COPY arm-linux-musleabi-cross.tgz .
RUN tar xf arm-linux-musleabi-cross.tgz -C /opt
RUN rm arm-linux-musleabi-cross.tgz

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    rsync \
    flex \
    bison \
    bc \
    python3 \
    git \
    gitk \
    libelf-dev \
    gettext \
    autoconf \
    autogen \
    autopoint \
    libtool \
    cpio \
    bash-completion \
    libncurses5-dev \
    pkg-config \
    libusb-1.0-0-dev \
    uuid-dev \
    libssl-dev \
    libpkcs11-helper1-dev \
    sudo

COPY punchboot punchboot
RUN make -C punchboot/src/tools/punchboot all install GIT_VERSION=monolinux-jiffy
RUN make -C punchboot/src/tools/pbimage all install GIT_VERSION=monolinux-jiffy
RUN rm -rf punchboot

COPY uuu /usr/bin

RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

ENV HOSTNAME=monolinux-jiffy