#!/bin/bash

set -e

IMAGE=jiffy.bpak
PKG_UUID=8df597ff-2cf5-42ea-b2b6-47c348721b75
PKG_UNIQUE_ID=11111111-2222-3333-4444-555555555555

bpak create $IMAGE -Y
bpak add $IMAGE --meta bpak-package --from-string $PKG_UUID --encoder uuid
bpak add $IMAGE --meta bpak-package-uid --from-string $PKG_UNIQUE_ID --encoder uuid
bpak add $IMAGE \
     --meta pb-load-addr --from-string 0x82820000 --part-ref dt --encoder integer
bpak add $IMAGE --part dt --from-file $1
bpak add $IMAGE \
     --meta pb-load-addr --from-string 0x83000000 --part-ref kernel --encoder integer
bpak add $IMAGE --part kernel --from-file $2
bpak add $IMAGE \
     --meta pb-load-addr --from-string 0x83f00000 --part-ref ramdisk --encoder integer
bpak add $IMAGE --part ramdisk --from-file $3
bpak sign $IMAGE --key ../3pp/punchboot/pki/secp256r1-key-pair.pem \
                    --key-id pb-development \
                    --key-store pb
