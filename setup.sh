cd monolinux
source setup.sh
cd ..

export ML_LINUX_VERSION=4.14.78-jiffy
export ML_LINUX_CONFIG=$(readlink -f app/linux-4.14.78-jiffy.config)
export ML_SOURCES=$(readlink -f sources)
export ARCH=arm
export CROSS_COMPILE=arm-linux-musleabi-
export ML_AUTOTOOLS_HOST=arm-linux-musleabi
export PATH=$PATH:$(readlink -f arm-linux-musleabi-cross/bin)
