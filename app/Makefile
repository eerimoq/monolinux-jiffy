JIFFY_DTB = $(LINUX_SRC)/arch/arm/boot/dts/jiffy.dtb
AUTH_COOKIE ?=
PACKAGES += curl
PACKAGES += heatshrink
PACKAGES += xz
PACKAGES += detools
PACKAGES += mbedtls
PACKAGES += zlib
PACKAGES += monolinux-c-library

default:
	$(MAKE) all
	$(MAKE) jiffy.pbi
	$(MAKE) upload

include $(ML_ROOT)/make/app.mk

jiffy.pbi: $(JIFFY_DTB) $(LINUX_SRC)/arch/arm/boot/zImage $(INITRAMFS)
	pbimage jiffy.pbc

upload:
ifneq ($(JIFFY_AUTH_COOKIE),)
	punchboot dev -a -s secp256r1:sha256 -n 0 -f $(JIFFY_AUTH_COOKIE)
endif
	punchboot boot -x -f jiffy.pbi
