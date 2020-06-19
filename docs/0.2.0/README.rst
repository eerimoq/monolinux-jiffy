Measurements
============

+-------------------+--------------+---------+
| Measurement point | Elapsed time | Delta   |
+===================+==============+=========+
| Hardware          | 1 ms         | 1 ms    |
+-------------------+--------------+---------+
| ROM code          | 185 ms       | 184 ms  |
+-------------------+--------------+---------+
| Bootloader        | 261 ms       | 76 ms   |
+-------------------+--------------+---------+
| Linux user space  | 342 ms       | 81 ms   |
+-------------------+--------------+---------+
| EXT4 filesystem   | 370 ms       | 28 ms   |
+-------------------+--------------+---------+
| TCP/IP networking | 2.0 s        | 1.6 s   |
+-------------------+--------------+---------+

Sizes
=====

+----------------+---------+
| File           | Size    |
+================+=========+
| Image          | 3.0 MB  |
+----------------+---------+
| initramfs.cpio | 1.33 MB |
+----------------+---------+
| jiffy.bpak     | 4.33 MB |
+----------------+---------+

Linux boot log
==============

.. code-block:: text

   [    0.000000] Booting Linux on physical CPU 0x0
   [    0.000000] Linux version 5.7.4 (erik@erik-GR8) (gcc version 9.1.0 (GCC), GNU ld (GNU Binutils) 2.32) #1 Sat Jun 20 06:43:13 UTC 2020
   [    0.000000] CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=50c53c7d
   [    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
   [    0.000000] OF: fdt: Machine model: Jiffy
   [    0.000000] Memory policy: Data cache writeback
   [    0.000000] On node 0 totalpages: 32768
   [    0.000000] Normal zone: 256 pages used for memmap
   [    0.000000] Normal zone: 0 pages reserved
   [    0.000000] Normal zone: 32768 pages, LIFO batch:7
   [    0.000000] CPU: All CPU(s) started in SVC mode.
   [    0.000000] pcpu-alloc: s0 r0 d32768 u32768 alloc=1*32768
   [    0.000000] pcpu-alloc: [0] 0
   [    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 32512
   [    0.000000] Kernel command line: console=ttymxc1,115200
   [    0.000000] Dentry cache hash table entries: 16384 (order: 4, 65536 bytes, linear)
   [    0.000000] Inode-cache hash table entries: 8192 (order: 3, 32768 bytes, linear)
   [    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
   [    0.000000] Memory: 124556K/131072K available (2053K kernel code, 110K rwdata, 492K rodata, 140K init, 1089K bss, 6516K reserved, 0K cma-reserved, 0K highmem)
   [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
   [    0.000000] NR_IRQS: 16, nr_irqs: 16, preallocated irqs: 16
   [    0.000000] Switching to timer-based delay loop, resolution 41ns
   [    0.000010] sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 89478484971ns
   [    0.000036] clocksource: mxc_timer1: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
   [    0.000197] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=48000)
   [    0.000216] pid_max: default: 32768 minimum: 301
   [    0.000397] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.000411] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.000775] CPU: Testing write buffer coherency: ok
   [    0.001391] Setting up static identity map for 0x801081c0 - 0x80108214
   [    0.001716] random: get_random_bytes called from 0xc03b4609 with crng_init=0
   [    0.001758] VFP support v0.3: implementor 41 architecture 2 part 30 variant 7 rev 5
   [    0.001824] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 3822520892550000 ns
   [    0.001849] futex hash table entries: 256 (order: -1, 3072 bytes, linear)
   [    0.001882] pinctrl core: initialized pinctrl subsystem
   [    0.002660] NET: Registered protocol family 16
   [    0.003054] DMA: preallocated 256 KiB pool for atomic coherent allocations
   [    0.003226] cpuidle: using governor menu
   [    0.003312] imx_soc_device_init: failed to find fsl,imx6ul-ocotp regmap!
   [    0.004866] cpu: supplied by regulator-dummy
   [    0.005297] vddsoc: supplied by regulator-dummy
   [    0.007226] imx6q_suspend_init: failed to find ocram node!
   [    0.007237] imx6_pm_common_init: No DDR LPM support with suspend -19!
   [    0.007637] imx6ul-pinctrl 20e0000.iomuxc: initialized IMX pinctrl driver
   [    0.012033] mxs-dma 1804000.dma-apbh: initialized
   [    0.012506] pps_core: LinuxPPS API ver. 1 registered
   [    0.012517] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
   [    0.012537] PTP clock support registered
   [    0.013368] clocksource: Switched to clocksource mxc_timer1
   [    0.014437] NET: Registered protocol family 2
   [    0.014744] tcp_listen_portaddr_hash hash table entries: 512 (order: 0, 4096 bytes, linear)
   [    0.014776] TCP established hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.014801] TCP bind hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.014824] TCP: Hash tables configured (established 1024 bind 1024)
   [    0.014954] UDP hash table entries: 256 (order: 0, 4096 bytes, linear)
   [    0.014984] UDP-Lite hash table entries: 256 (order: 0, 4096 bytes, linear)
   [    0.016604] workingset: timestamp_bits=30 max_order=15 bucket_order=0
   [    0.018261] 21e8000.serial: ttymxc1 at MMIO 0x21e8000 (irq = 31, base_baud = 5000000) is a IMX
   [    0.018315] printk: console [ttymxc1] enabled
   [    0.018894] libphy: Fixed MDIO Bus: probed
   [    0.019870] mmc0: SDHCI controller on 2190000.usdhc [2190000.usdhc] using ADMA
   [    0.032157] pps pps0: new PPS source ptp0
   [    0.032811] libphy: fec_enet_mii_bus: probed
   [    0.034018] fec 2188000.ethernet eth0: registered PHC device 0
   [    0.034760] sdhci: Secure Digital Host Controller Interface driver
   [    0.034765] sdhci: Copyright(c) Pierre Ossman
   [    0.034769] sdhci-pltfm: SDHCI platform and OF driver helper
   [    0.034971] NET: Registered protocol family 10
   [    0.035786] Segment Routing with IPv6
   [    0.035808] NET: Registered protocol family 17
   [    0.036072] Unpacking initramfs...
   [    0.037832] mmc0: new DDR MMC card at address 0001
   [    0.041980] mmcblk0: mmc0:0001 eMMC   3.64 GiB
   [    0.042368] mmcblk0boot0: mmc0:0001 eMMC   partition 1 4.00 MiB
   [    0.042739] mmcblk0boot1: mmc0:0001 eMMC   partition 2 4.00 MiB
   [    0.042843] mmcblk0rpmb: mmc0:0001 eMMC   partition 3 4.00 MiB, chardev (249:0)
   [    0.046749] Freeing initrd memory: 1304K
   [    0.046881] ThumbEE CPU extension supported.
   [    0.048769] Freeing unused kernel memory: 140K
   [    0.048776] Kernel memory protection not selected by kernel config.
   [    0.048781] Run /init as init process
   [    0.048785] with arguments:
   [    0.048788] /init
   [    0.048792] with environment:
   [    0.048796] HOME=/
   [    0.048799] TERM=linux
   [    0.049513] mmcblk0: p1 p2 p3 p4 p5 p6
   [    0.052395] 1970-01-01 00:00:00 INFO default Inserting /root/mbcache.ko.
   [    0.053162] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/mbcache.ko'.
   [    0.053220] 1970-01-01 00:00:00 INFO default Inserting /root/jbd2.ko.
   [    0.055149] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/jbd2.ko'.
   [    0.055211] 1970-01-01 00:00:00 INFO default Inserting /root/ext4.ko.
   [    0.061509] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/ext4.ko'.
   [    0.061764] 1970-01-01 00:00:00 INFO default Mounting /dev/mmcblk0p3 on /ext4fs.
   [    0.077498] EXT4-fs (mmcblk0p3): mounted filesystem with ordered data mode. Opts: (null)
   [    0.081681] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.081750] 1970-01-01 00:00:00 INFO default /ext4fs/README: | Monolinux Jiffy |
   [    0.081800] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.108830] SMSC LAN8710/LAN8720 2188000.ethernet-1:01: attached PHY driver [SMSC LAN8710/LAN8720] (mii_bus:phy_addr=2188000.ethernet-1:01, irq=POLL)
   [    1.685083] fec 2188000.ethernet eth0: Link is Up - 100Mbps/Full - flow control rx/tx
   [    1.685121] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
   [    1.685556] 1970-01-01 00:00:01 INFO dhcp-client Starting on interface 'eth0'.
   [    1.685720] 1970-01-01 00:00:01 INFO dhcp-client Interface information:
   [    1.685788] 1970-01-01 00:00:01 INFO dhcp-client   MACAddress: 5e:fd:f9:ca:b5:3e
   [    1.685841] 1970-01-01 00:00:01 INFO dhcp-client   Index:      2
   [    1.686891] 1970-01-01 00:00:01 INFO dhcp-client State change from INIT to SELECTING.
   [    1.691974] 1970-01-01 00:00:01 INFO dhcp-client Options:
   [    1.692037] 1970-01-01 00:00:01 INFO dhcp-client   MessageType:     2
   [    1.692102] 1970-01-01 00:00:01 INFO dhcp-client   SubnetMask:      255.255.255.0
   [    1.692160] 1970-01-01 00:00:01 INFO dhcp-client   Gateway:         192.168.0.1
   [    1.692216] 1970-01-01 00:00:01 INFO dhcp-client   DNS:             83.255.255.1
   [    1.692266] 1970-01-01 00:00:01 INFO dhcp-client   LeaseTime:       10800
   [    1.692313] 1970-01-01 00:00:01 INFO dhcp-client   RenewalTime:     5400
   [    1.692359] 1970-01-01 00:00:01 INFO dhcp-client   RebindingTime:   9450
   [    1.692413] 1970-01-01 00:00:01 INFO dhcp-client   ServerIPAddress: 192.168.0.1
   [    1.692475] 1970-01-01 00:00:01 INFO dhcp-client Received OFFER packet.
   [    1.692634] 1970-01-01 00:00:01 INFO dhcp-client State change from SELECTING to REQUESTING.
   [    2.308080] random: fast init done
   [    2.507550] 1970-01-01 00:00:02 INFO dhcp-client Received NONE packet.
   [    2.696726] 1970-01-01 00:00:02 INFO dhcp-client Options:
   [    2.696788] 1970-01-01 00:00:02 INFO dhcp-client   MessageType:     5
   [    2.696850] 1970-01-01 00:00:02 INFO dhcp-client   SubnetMask:      255.255.255.0
   [    2.696905] 1970-01-01 00:00:02 INFO dhcp-client   Gateway:         192.168.0.1
   [    2.696960] 1970-01-01 00:00:02 INFO dhcp-client   DNS:             83.255.255.1
   [    2.697009] 1970-01-01 00:00:02 INFO dhcp-client   LeaseTime:       10800
   [    2.697055] 1970-01-01 00:00:02 INFO dhcp-client   RenewalTime:     5400
   [    2.697101] 1970-01-01 00:00:02 INFO dhcp-client   RebindingTime:   9450
   [    2.697156] 1970-01-01 00:00:02 INFO dhcp-client   ServerIPAddress: 192.168.0.1
   [    2.697217] 1970-01-01 00:00:02 INFO dhcp-client Received ACK packet.
   [    2.697341] 1970-01-01 00:00:02 INFO dhcp-client Configuring interface 'eth0' with ip address 192.168.0.3, subnet mask 255.255.255.0, gateway 192.168.0.1 and mtu 1500.
   [    2.697932] 1970-01-01 00:00:02 INFO dhcp-client State change from REQUESTING to BOUND.
