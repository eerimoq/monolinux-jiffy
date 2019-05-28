JIFFY_DTB = $(LINUX_SRC)/arch/arm/boot/dts/jiffy.dtb
AUTH_COOKIE ?=

default:
	$(MAKE) all
	$(MAKE) jiffy.pbi
	$(MAKE) upload

include $(ML_ROOT)/make/app.mk
include $(ML_ROOT)/make/packages/curl-7.65.0.mk
include $(ML_ROOT)/make/packages/heatshrink-0.4.1.mk
include $(ML_ROOT)/make/packages/xz-5.2.4.mk
include $(ML_ROOT)/make/packages/detools-0.29.0.mk
include $(ML_ROOT)/make/packages/openssl-1.1.1b.mk
include $(ML_ROOT)/make/packages/zlib-1.2.11.mk

jiffy.pbi: $(JIFFY_DTB) $(LINUX_SRC)/arch/arm/boot/zImage $(INITRAMFS)
	pbimage jiffy.pbc

upload:
ifneq ($(JIFFY_AUTH_COOKIE),)
	punchboot dev -a -s secp256r1:sha256 -n 0 -f $(JIFFY_AUTH_COOKIE)
endif
	punchboot boot -x -f jiffy.pbi
