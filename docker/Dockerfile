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
    python3-pip \
    bpython3 \
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
    sudo \
    u-boot-tools \
    locales \
    xxd \
    tio \
    liblz4-tool

COPY bpak bpak
RUN cd bpak && autoreconf -fi && ./configure && make && make install
RUN ldconfig
RUN rm -rf bpak

COPY punchboot punchboot
RUN make -C punchboot/src/tools/punchboot/src GIT_VERSION=monolinux-jiffy
RUN make -C punchboot/src/tools/punchboot/src install GIT_VERSION=monolinux-jiffy
RUN make -C punchboot/src/tools/imxcst/src GIT_VERSION=monolinux-jiffy
RUN install -m 644 punchboot/src/tools/imxcst/src/build-x86_64-linux-gnu/cst /usr/bin
RUN chmod +x /usr/bin/cst
RUN install -m 644 punchboot/src/tools/createtoken.sh /usr/bin
RUN chmod +x /usr/bin/createtoken.sh
RUN rm -rf punchboot

COPY uuu /usr/bin

RUN pip3 install \
    ecdsa \
    imx \
    nala==0.105.0

ENV LC_ALL C.UTF-8
ENV LANG C.UTF-8

RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

ENV HOSTNAME=monolinux-jiffy