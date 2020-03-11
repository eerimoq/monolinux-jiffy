default:
	$(MAKE) -C app

clean:
	$(MAKE) -C app clean

docker-image:
	rm -rf docker/punchboot
	cp -r 3pp/punchboot docker
	cd docker && docker build -t monolinux-jiffy .

docker-image-tag-and-push:
	docker tag monolinux-jiffy:latest eerimoq/monolinux-jiffy:$(TAG)
	docker push eerimoq/monolinux-jiffy:$(TAG)
