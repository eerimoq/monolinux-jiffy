|buildstatus|_
|codecov|_
|nala|_

Monolinux Jiffy
===============

A Monolinux distro for the Jiffy board!

Monolinux: https://github.com/eerimoq/monolinux

Bootloader: https://github.com/jonasblixt/punchboot

Hardware: https://github.com/jonasblixt/jiffy

Features
========

- Fast development cylce (a matter of seconds from source code change
  to entering user space).

- Everything the Linux kernel provides (networking, filesystems,
  drivers, etc).

- Various libraries provided by `Monolinux`_.

- Efficient and fun unit testing with `Nala`_.

Boot time
=========

The final system enters user space in **0.34 seconds**, which is x
seconds faster than the baseline system. The complete system,
including an EXT4 file system and networking, is functional in just
**2.1 seconds**.

The table below contains measurements for both the baseline and final
systems.

+-------------------+------------------------+---------------------+---------+
| Measurement point | Elapsed time, baseline | Elapsed time, final | Delta   |
+===================+========================+=====================+=========+
| Hardware          | 1 ms                   | 1 ms                | 0 ms    |
+-------------------+------------------------+---------------------+---------+
| ROM code          | 185 ms                 | 185 ms              | 0 ms    |
+-------------------+------------------------+---------------------+---------+
| Bootloader        | x ms                   | 271 ms              | -x ms   |
+-------------------+------------------------+---------------------+---------+
| Linux             | x s                    | 336 ms              | -x s    |
+-------------------+------------------------+---------------------+---------+
| Filesystem        | x s                    | 405 ms              | -x s    |
+-------------------+------------------------+---------------------+---------+
| Network           | x s                    | 2.1 s               | -x s    |
+-------------------+------------------------+---------------------+---------+

A few words about the final system components and its boot sequence:

Hardware

The time from plugging in the USB cable to releasing the reset of the
SoC is neglactable.

ROM code

It's unknown why the ROM code takes so long. It's hard to do anything
about it as this is properitary NXP software.

Bootloader

Punchboot is a slim and fast bootloader. It reads the Linux kernel,
ramdisk and device tree from eMMC and verifies them as quickly as
possible. Then start the Linux kernel.

Linux

The tiny Linux kernel boots in about 60 ms. Both the Linux kernel and
the ramdisk are **not** compressed, as it is faster to read from eMMC
than decompressing them.

Filesystem

It takes some time for Linux to detect the eMMC. Insert the ext4
kernel module while waiting, and mount the file system immediately
when the eMMC is ready.

The eMMC driver could likely be optimized further to save say 20 ms.

Network

Ethernet auto-negotiation takes a significant amount of time, say 1 to
3 seconds. There is nothing to do about it, it's how the protocol is
defined.

Optimizations
-------------

Less functionality in the kernel. No sysfs. No debugfs. Only required
drivers and file systems. No kernel protection for less padding in the
binary (saves 2 MB).

Ramdisk and kernel without compression. LZ4 compression is a good
alternative, only slightly slower.

MMC driver without delays and starting with 52 MHz clock.

Small device tree.

The statically linked init process contains the entire application,
implemented in C. No forks. No scripts. No shared libraries.

Measurements
------------

Test sequence
^^^^^^^^^^^^^

#. Unmount and power off the board.

   .. code-block:: text

      $ umount ext4fs
      $ poweroff

#. Unplug the USB cable.

#. Start a logic analyzer capture.

#. Plug in the USB cable.

#. Wait for the system to start.

#. Stop the logic analyzer capture.

Measurement points
^^^^^^^^^^^^^^^^^^

.. code-block:: text

   Hardware:   RST_N high
   ROM code:   GPIO low
   Bootloader: GPIO high
   Linux:      "main" printed
   Filesystem: dmesg "mounted..." - dmesg "Freeing..." + "main" printed
   Network:    dmesg "Received OFFER..." - dmesg "Freeing..." + "main" printed

Build and run
=============

This requires that punchboot is running and ready to execute commands.

.. code-block:: shell

   $ ./rundocker.sh
   $ make -s -j8 upload

.. |buildstatus| image:: https://travis-ci.org/eerimoq/monolinux-jiffy.svg
.. _buildstatus: https://travis-ci.org/eerimoq/monolinux-jiffy

.. |codecov| image:: https://codecov.io/gh/eerimoq/monolinux-jiffy/branch/master/graph/badge.svg
.. _codecov: https://codecov.io/gh/eerimoq/monolinux-jiffy

.. |nala| image:: https://img.shields.io/badge/nala-test-blue.svg
.. _nala: https://github.com/eerimoq/nala

.. _Monolinux: https://github.com/eerimoq/monolinux

.. _Nala: https://github.com/eerimoq/nala
