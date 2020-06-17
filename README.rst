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

The embedded Linux based system enters user space **0.33 seconds**
after power on. The EXT4 filesystem is ready in **0.37 seconds** and
networking in **2.2 seconds**. Very impressive for an i.MX6UL SoC with
a 528 MHz ARMv7-A CPU, 1 GB DDR3 RAM and 4 GB eMMC! A reboot is even
faster, only **0.26 seconds** from issuing the reboot to entering user
space.

Note that all software except the EXT4 filesystem are part of a secure
boot chain. The system will boot even faster without secure boot, but
unfortunately I've not had the oppertunity to try it.

+-------------------+--------------+---------+
| Measurement point | Elapsed time | Delta   |
+===================+==============+=========+
| Hardware          | 1 ms         | 0 ms    |
+-------------------+--------------+---------+
| ROM code          | 185 ms       | 184 ms  |
+-------------------+--------------+---------+
| Bootloader        | 271 ms       | 86 ms   |
+-------------------+--------------+---------+
| Linux user space  | 333 ms       | 62 ms   |
+-------------------+--------------+---------+
| EXT4 filesystem   | 373 ms       | 40 ms   |
+-------------------+--------------+---------+
| TCP/IP networking | 2.2 s        | 1.8 s   |
+-------------------+--------------+---------+

More information about the system and its boot sequence:

First of all, the power is turned on. The hardware releases the reset
to the i.MX6UL about 1 millisecond later, and soon the ROM code reads
the bootloader from eMMC, verifies its integrity and jumps to it. This
takes 185 milliseconds, which is far more than expected. It's hard to
do anything about it as this is properitary NXP software.

The `Punchboot`_ bootloader is very fast out of the box, and to make
it even faster the device tree patching was removed (now done compile
time) and the eMMC type was changed from DDR52 to HS200. The
bootloader reads the Linux kernel (3.5 MB), ramdisk (1.4 MB) and
device tree (8 kB) from eMMC and verifies them in only 86
milliseconds. It then start the Linux kernel. No other bootloader was
tested, not even U-Boot.

The system's tiny Linux kernel (version 4.14.78) boots in about 62
milliseconds, which is very fast. This is achieved with a minimal
kernel configuration, a few patches, a minimal device tree, and
uncompressed kernel and ramdisk images.

Here is a brief description of the `Linux kernel patches`_.

- Unpack the ramdisk after drivers are probed.

- Removal of unnecessary delays in the MMC driver (I hope). This is
  possible since we know excatly which MMC is mounted on the board,
  and that it is always powered on.

- Start with MMC clock frequency at 52 MHz instaed of 400 kHz. Same
  reasoning as in the previous bullet.

- Async MMC and FEC (Ethernet) driver probes to do other
  initialization in parallel.

- 10 Hz Ethernet PHY polling instead of 1 Hz. Would not be needed if
  the PHY sends an interrupt when its link is up.

The statically linked init process, part of the ramdisk, contains the
entire application. It's implemented in C for low overhead, both in
RAM and CPU time. No forks. No scripts. No shared libraries. It does
however contain cURL and other libraries, which makes it about 800 kB.

The EXT4 filesystem (which is not integrity checked with dmverity) is
mounted within 40 ms after entering user space. The enabler is to
start the customized MMC driver early.

Networking takes by far the longest time to get ready. The main reason
is that Ethernet auto-negotiation takes a significant amount of time,
about 1 to 3 seconds. Users that do not need Ethernet, or can use a
static link configuration, can save a bunch of time.

Below is selected messages from the Linux kernel log to better
understand what is going on. There are a few user space messages in
the log as well.

.. code-block:: text

   [    0.000000] Booting Linux on physical CPU 0x0
   [    0.000000] Linux version 4.14.78 (erik@erik-GR8) (gcc version 9.1.0 (GCC)) #1 Tue Jun 16 19:18:46 UTC 2020

   [    0.022802] Unpacking initramfs...
   [    0.025535] mmc0: SDHCI controller on 2190000.usdhc [2190000.usdhc] using ADMA
   [    0.034262] Freeing initrd memory: 1416K
   [    0.035840] Freeing unused kernel memory: 172K
   [    0.041533] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/fec.ko'.
   [    0.053621] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/ext4.ko'.
   [    0.057696] fec 2188000.ethernet eth0: registered PHC device 0
   [    0.058009] mmcblk0: p1 p2 p3 p4 p5 p6
   [    0.076813] EXT4-fs (mmcblk0p3): mounted filesystem with ordered data mode. Opts: (null)
   [    0.082744] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.082812] 1970-01-01 00:00:00 INFO default /ext4fs/README: | Monolinux Jiffy |
   [    0.082869] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.114466] SMSC LAN8710/LAN8720 2188000.ethernet-1:01: attached PHY driver [SMSC LAN8710/LAN8720] (mii_bus:phy_addr=2188000.ethernet-1:01, irq=POLL)
   [    0.114607] IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
   [    1.892754] fec 2188000.ethernet eth0: Link is Up - 100Mbps/Full - flow control rx/tx
   [    1.892791] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
   [    1.893268] 1970-01-01 00:00:01 INFO dhcp_client Starting on interface 'eth0'.
   [    1.900520] 1970-01-01 00:00:01 INFO dhcp_client Received OFFER packet.

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

.. _Linux kernel patches: https://github.com/eerimoq/linux/compare/e7405910ca5553eae8744af4e5c03e64ee048cb1..a3f1f66ab66b1c03731530e86dcc7262237a437d

.. _Punchboot: https://github.com/jonasblixt/punchboot
