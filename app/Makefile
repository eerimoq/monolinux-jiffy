JIFFY_DTB = $(LINUX_SRC)/arch/arm/boot/dts/jiffy.dtb
JIFFY_DTS = $(LINUX_SRC)/arch/arm/boot/dts/jiffy.dts
BOARD_UUID ?= 1145223f-f951-3d6d-91da-31a89fb59db6
JIFFY_AUTH_COOKIE ?= $(BOARD_UUID).token
BZIMAGE ?= $(LINUX_SRC)/arch/arm/boot/zImage
PACKAGES += curl
PACKAGES += heatshrink
PACKAGES += xz
PACKAGES += detools
PACKAGES += mbedtls
PACKAGES += zlib
PACKAGES += monolinux-c-library
INC += $(ML_SOURCES)/punchboot/src/include
INITRAMFS_FILES += build/linux/drivers/mmc/host/sdhci-esdhc-imx.ko
INITRAMFS_FILES += build/linux/drivers/mmc/host/sdhci-pltfm.ko
INITRAMFS_FILES += build/linux/drivers/mmc/host/sdhci.ko
INITRAMFS_FILES += build/linux/drivers/mmc/core/mmc_core.ko
INITRAMFS_FILES += build/linux/drivers/mmc/core/mmc_block.ko
INITRAMFS_FILES += build/linux/drivers/i2c/busses/i2c-imx.ko
INITRAMFS_FILES += build/linux/drivers/i2c/i2c-dev.ko

default:
	$(MAKE) all
	$(MAKE) jiffy.bpak
	$(MAKE) dtb
	$(MAKE) upload

include $(ML_ROOT)/make/app.mk

jiffy.bpak: $(JIFFY_DTB) $(BZIMAGE) $(INITRAMFS)
	echo "Creating $@"
	./jiffy.sh $(JIFFY_DTB) $(BZIMAGE) $(INITRAMFS)

dtb: $(JIFFY_DTB)

$(JIFFY_DTB): $(JIFFY_DTS) $(INITRAMFS)
	echo "Patching the device tree"
	./patch_device_tree.py $(JIFFY_DTS) $(INITRAMFS)
	$(MAKE) -C $(LINUX_SRC) dtbs

authenticate:
	punchboot dev -a -s secp256r1:sha256 -n 0xa90f9680 -f $(JIFFY_AUTH_COOKIE)

authenticate-token:
	createtoken.sh $(BOARD_UUID) file ../3pp/punchboot/pki/secp256r1-key-pair.pem

upload: authenticate
	punchboot boot -x -f jiffy.bpak

upload-emmc: authenticate
	punchboot part -w -n 0 -f jiffy.bpak
