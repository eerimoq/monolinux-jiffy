default:
	$(MAKE) -C app

clean:
	$(MAKE) -C app clean

punchboot-build:
	$(MAKE) -C 3pp/punchboot/src clean BOARD=jiffy
	$(MAKE) -C 3pp/punchboot/src BOARD=jiffy LOGLEVEL=0

punchboot-upload:
	$(MAKE) -C app authenticate
	punchboot boot -w -f 3pp/punchboot/src/build-jiffy/pb_signed.imx

punchboot-upload-uuu:
	uuu 3pp/punchboot/src/build-jiffy/pb_signed_uuu.imx

docker-image:
	rm -rf docker/punchboot
	cp -r 3pp/punchboot docker
	cd docker && docker build -t monolinux-jiffy .

docker-image-tag-and-push:
	docker tag monolinux-jiffy:latest eerimoq/monolinux-jiffy:$(TAG)
	docker push eerimoq/monolinux-jiffy:$(TAG)
