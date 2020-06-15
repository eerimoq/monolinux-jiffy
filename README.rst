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

WARNING: This section is under construction and contains a ton of
errors.

The final system enters user space in **0.34 seconds**, which is
3.9 seconds faster than the baseline system. The complete system,
including an EXT4 file system and networking, is functional in just
**1.3 seconds**.

The table below contains measurements for both the baseline and final
systems.

+-------------------+------------------------+---------------------+---------+
| Measurement point | Elapsed time, baseline | Elapsed time, final | Delta   |
+===================+========================+=====================+=========+
| Hardware          | 1 ms                   | 1 ms                | 0 ms    |
+-------------------+------------------------+---------------------+---------+
| ROM code          | 190 ms                 | 190 ms              | 0 ms    |
+-------------------+------------------------+---------------------+---------+
| Bootloader        | 490 ms                 | 270 ms              | -220 ms |
+-------------------+------------------------+---------------------+---------+
| Linux             | 4.3 s                  | 337 ms              | -3.9 s  |
+-------------------+------------------------+---------------------+---------+
| Filesystem        | 4.8 s                  | 400 ms              | -4.4 s  |
+-------------------+------------------------+---------------------+---------+
| Network           | 6.8 s                  | 1.3 s               | -5.7 s  |
+-------------------+------------------------+---------------------+---------+

Test sequence:

#. Unmount and power off the board.

   .. code-block:: text

   $ umount ext4fs
   $ poweroff

#. Unplug the USB cable.

#. Start logic analyzer capture.

#. Plug in the USB cable.

#. Wait for the system to start.

#. Stop logic analyzer capture.

Measurement points:

.. code-block:: text

   Hardware:   RST_N high
   ROM code:   GPIO low
   Bootloader: GPIO high
   Linux:      "main" printed
   Filesystem: "filesystem" printed
   Network:    "ping" printed

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
