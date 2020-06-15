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

Measurements:

+------------+-----------------------+--------------------+------------+
| Stage      | Baseline (delta) [ms] | Final (delta) [ms] | Delta [ms] |
+============+=======================+====================+============+
| Hardware   | 1 (1)                 | 1 (1)              | 0          |
| ROM code   | 190 (190)             | 190 (190)          | 0          |
| Bootloader | 490 (300)             | 270 (80)           | -220       |
| Linux      | 4290 (4000)           | 337 (67)           | -3963      |
| Filesystem | 4790 (500)            | 400 (63)           | -4390      |
| Network    | 6790 (2000)           | 1300 (900)         | -5690      |
+------------+-----------------------+--------------------+------------+

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
