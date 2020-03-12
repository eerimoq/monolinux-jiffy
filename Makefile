default:
	$(MAKE) -C app

clean:
	$(MAKE) -C app clean

upload-bootloader:
	uuu 3pp/punchboot/src/build-jiffy/pb_signed_uuu.imx
	punchboot boot -w -f 3pp/punchboot/src/build-jiffy/pb_signed.imx

docker-image:
	rm -rf docker/punchboot
	cp -r 3pp/punchboot docker
	cd docker && docker build -t monolinux-jiffy .

docker-image-tag-and-push:
	docker tag monolinux-jiffy:latest eerimoq/monolinux-jiffy:$(TAG)
	docker push eerimoq/monolinux-jiffy:$(TAG)
