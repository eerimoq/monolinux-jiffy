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

The final system enters user space in **0.33 seconds**, which is x
seconds faster than the baseline system. The complete system,
including an EXT4 file system and networking, is functional in just
**2.2 seconds**.

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
| Linux             | x s                    | 333 ms              | -x s    |
+-------------------+------------------------+---------------------+---------+
| Filesystem        | x s                    | 373 ms              | -x s    |
+-------------------+------------------------+---------------------+---------+
| Network           | x s                    | 2.2 s               | -x s    |
+-------------------+------------------------+---------------------+---------+

The hardware releases the reset to the i.MX6UL quickly. The ROM code
reads the bootloader from eMMC, verifies its integrity and jumps to
it. This takes 185 milliseconds, which is far more than expected. It's
hard to do anything about it as this is properitary NXP software.

The Punchboot bootloader is rather fast. It reads the Linux kernel,
ramdisk and device tree from eMMC and verifies them in only 86
milliseconds. It then start the Linux kernel.

The final system's tiny Linux kernel boots in about 60 ms, which is
very fast. This is achieved with a minimal kernel configuration, a few
patches, a minimal device tree, and not using compressed kernel and
ramdisk.

The patches are:

- Unpack the ramdisk after drivers are probed.

- Removal of unnecessary delays in the MMC driver.

- Start with MMC clock frequency at 52 MHz instaed of 400 kHz.

- Async MMC and FEC (Ethernet) driver probes.

- 10 Hz Ethernet PHY polling instead of 1 Hz.

See ``3pp/linux`` for details.

The statically linked init process contains the entire application,
implemented in C. No forks. No scripts. No shared libraries. It does
however contain cURL and other libraries, which makes it about 800 kB.

The EXT4 filesystem is mounted within 40 ms after entering user
space. The enabler is to start the customized MMC driver early.

Networking takes by far the longest time to get ready. The main reason
is that Ethernet auto-negotiation takes a significant amount of time,
about 1 to 3 seconds.

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
