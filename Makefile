default:
	$(MAKE) -C app

clean:
	$(MAKE) -C app clean

punchboot-build:
	$(MAKE) -C 3pp/punchboot/src clean BOARD=jiffy
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy LOGLEVEL=3

punchboot-build-timing-report:
	$(MAKE) -C 3pp/punchboot/src clean BOARD=jiffy
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy TIMING_REPORT=1

punchboot-build-quiet:
	$(MAKE) -C 3pp/punchboot/src clean BOARD=jiffy
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy LOGLEVEL=0

punchboot-upload:
	$(MAKE) -C app authenticate
	punchboot boot -w -f 3pp/punchboot/src/build-jiffy/pb_signed.imx

punchboot-upload-to-ram:
	sudo uuu 3pp/punchboot/src/build-jiffy/pb_signed_uuu.imx

docker-image:
	rm -rf docker/punchboot
	cp -r 3pp/punchboot docker
	rm -rf docker/bpak
	cp -r 3pp/bpak docker
	cd docker && docker build -t monolinux-jiffy .

docker-image-tag-and-push:
	docker tag monolinux-jiffy:latest eerimoq/monolinux-jiffy:$(TAG)
	docker push eerimoq/monolinux-jiffy:$(TAG)
