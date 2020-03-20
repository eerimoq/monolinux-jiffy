#!/usr/bin/env python3

import os
import re
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('dts')
parser.add_argument('initramfs')
args = parser.parse_args()

address = 0x83f00000
initramfs_size = os.stat(args.initramfs).st_size

with open(args.dts, 'r') as fin:
    dts = fin.read()

dts = re.sub(r'linux,initrd-start = <[^>]+>',
             f'linux,initrd-start = <{hex(address)}>',
             dts)
dts = re.sub(r'linux,initrd-end = <[^>]+>',
             f'linux,initrd-end = <{hex(address + initramfs_size)}>',
             dts)

with open(args.dts, 'w') as fout:
    fout.write(dts)
