Monolinux Jiffy
===============

A Monolinux distro for the Jiffy board!

Monolinux: https://github.com/eerimoq/monolinux

Bootloader: https://github.com/jonpe960/punchboot

Hardware: https://github.com/jonpe960/jiffy

Setup
=====

Build and install ``punchboot`` (punchboot/src/tools/punchboot) and
``pbimage`` (punchboot/src/tools/pbimage).

Then do:

.. code-block:: shell

   $ git submodule update --init --recursive
   $ tar xf sources/arm-linux-musleabi-cross.tgz
   $ source setup.sh
   $ make -s -j4 # or make -s -j4 JIFFY_AUTH_COOKIE=<path>
