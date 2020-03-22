JIFFY_DTB = build/jiffy.dtb
JIFFY_DTS = $(LINUX_SRC)/arch/arm/boot/dts/jiffy.dts
JIFFY_BPAK = build/jiffy.bpak
BOARD_UUID ?= 1145223f-f951-3d6d-91da-31a89fb59db6
JIFFY_AUTH_COOKIE ?= $(BOARD_UUID).token
BZIMAGE ?= $(LINUX_SRC)/arch/arm/boot/zImage
PACKAGES += curl
PACKAGES += mbedtls
PACKAGES += zlib
PACKAGES += monolinux-c-library
INC += $(ML_SOURCES)/punchboot/src/include

default:
	$(MAKE) all
	$(MAKE) $(JIFFY_DTB)
	$(MAKE) $(JIFFY_BPAK)

authenticate:
	punchboot dev -a -s secp256r1:sha256 -n 0xa90f9680 -f $(JIFFY_AUTH_COOKIE)

authenticate-token:
	createtoken.sh $(BOARD_UUID) file ../3pp/punchboot/pki/secp256r1-key-pair.pem

upload: default authenticate
	echo "Uploading $(JIFFY_BPAK) to eMMC."
	punchboot part -w -n 0 -f $(JIFFY_BPAK)
	punchboot boot -a -s A
	punchboot boot -r

upload-ram: default authenticate
	echo "Uploading $(JIFFY_BPAK) to RAM."
	punchboot boot -x -f $(JIFFY_BPAK)

include $(ML_ROOT)/make/app.mk

$(JIFFY_DTB): $(JIFFY_DTS) $(INITRAMFS)
	echo "Patching the device tree."
	./patch_device_tree.py $(JIFFY_DTS) $(INITRAMFS)
	$(MAKE) -C $(LINUX_SRC) dtbs
	cp $(JIFFY_DTS:%.dts=%.dtb) $(JIFFY_DTB)

$(JIFFY_BPAK): $(JIFFY_DTB) $(BZIMAGE) $(INITRAMFS)
	echo "Creating $@."
	./jiffy.sh $(JIFFY_DTB) $(BZIMAGE) $(INITRAMFS)
