.PHONY: test

default:
	$(MAKE) -C app default

clean:
	$(MAKE) -C app clean
	$(MAKE) -C tst clean

test:
	$(MAKE) -C tst

system-test:
	python3 test/test.py $(ARGS)

upload:
	$(MAKE) -C app upload

upload-ram:
	$(MAKE) -C app upload-ram

punchboot-build:
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy LOGLEVEL=3

punchboot-build-timing-report:
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy TIMING_REPORT=1

punchboot-build-quiet:
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy LOGLEVEL=0

punchboot-build-force-run-recovery:
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy FORCE_RUN_RECOVERY=1

punchboot-clean:
	$(MAKE) -C 3pp/punchboot/src clean BOARD=jiffy

punchboot-upload:
	$(MAKE) -C app authenticate
	punchboot boot -w -f 3pp/punchboot/src/build-jiffy/pb_signed.imx

punchboot-upload-ram:
	sudo uuu 3pp/punchboot/src/build-jiffy/pb_signed_uuu.imx

punchboot-reset:
	$(MAKE) -C app authenticate
	punchboot boot -r

docker-image:
	rm -rf docker/punchboot
	cp -r 3pp/punchboot docker
	rm -rf docker/bpak
	cp -r 3pp/bpak docker
	cd docker && docker build -t monolinux-jiffy .

docker-image-tag-and-push:
	docker tag monolinux-jiffy:latest eerimoq/monolinux-jiffy:$(TAG)
	docker push eerimoq/monolinux-jiffy:$(TAG)

gdb-coredump:
	gdb-multiarch app/build/app.debug core
