JIFFY_DTB = $(LINUX_SRC)/arch/arm/boot/dts/jiffy.dtb
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

default:
	$(MAKE) all
	$(MAKE) jiffy.bpak
	$(MAKE) upload

include $(ML_ROOT)/make/app.mk

jiffy.bpak: $(JIFFY_DTB) $(BZIMAGE) $(INITRAMFS)
	./jiffy.sh $(JIFFY_DTB) $(BZIMAGE) $(INITRAMFS)

authenticate:
	punchboot dev -a -s secp256r1:sha256 -n 0xa90f9680 -f $(JIFFY_AUTH_COOKIE)

authenticate-token:
	createtoken.sh $(BOARD_UUID) file ../3pp/punchboot/pki/secp256r1-key-pair.pem

upload: authenticate
	punchboot boot -x -f jiffy.bpak
