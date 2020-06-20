Measurements
============

+-------------------+--------------+---------+
| Measurement point | Elapsed time | Delta   |
+===================+==============+=========+
| Hardware          | 1 ms         | 1 ms    |
+-------------------+--------------+---------+
| ROM code          | 185 ms       | 184 ms  |
+-------------------+--------------+---------+
| Bootloader        | 260 ms       | 75 ms   |
+-------------------+--------------+---------+
| Linux user space  | 336 ms       | 76 ms   |
+-------------------+--------------+---------+
| EXT4 filesystem   | 360 ms       | 24 ms   |
+-------------------+--------------+---------+
| TCP/IP networking | 2.0 s        | 1.6 s   |
+-------------------+--------------+---------+

Sizes
=====

+----------------+---------+
| File           | Size    |
+================+=========+
| Image          | 3.27 MB |
+----------------+---------+
| initramfs.cpio | 0.87 MB |
+----------------+---------+
| jiffy.bpak     | 4.14 MB |
+----------------+---------+

Linux boot log
==============

.. code-block:: text

   [    0.000000] Booting Linux on physical CPU 0x0
   [    0.000000] Linux version 5.7.4 (erik@erik-GR8) (gcc version 9.1.0 (GCC), GNU ld (GNU Binutils) 2.32) #2 Sat Jun 20 12:30:36 UTC 2020
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
   [    0.000000] Memory: 124736K/131072K available (2282K kernel code, 111K rwdata, 536K rodata, 136K init, 1090K bss, 6336K reserved, 0K cma-reserved, 0K highmem)
   [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
   [    0.000000] NR_IRQS: 16, nr_irqs: 16, preallocated irqs: 16
   [    0.000000] Switching to timer-based delay loop, resolution 41ns
   [    0.000011] sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 89478484971ns
   [    0.000036] clocksource: mxc_timer1: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
   [    0.000192] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=48000)
   [    0.000208] pid_max: default: 32768 minimum: 301
   [    0.000394] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.000409] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.000759] CPU: Testing write buffer coherency: ok
   [    0.001362] Setting up static identity map for 0x801081c0 - 0x80108214
   [    0.001696] random: get_random_bytes called from 0xc03f9b95 with crng_init=0
   [    0.001738] VFP support v0.3: implementor 41 architecture 2 part 30 variant 7 rev 5
   [    0.001806] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 3822520892550000 ns
   [    0.001830] futex hash table entries: 256 (order: -1, 3072 bytes, linear)
   [    0.001863] pinctrl core: initialized pinctrl subsystem
   [    0.002635] NET: Registered protocol family 16
   [    0.003030] DMA: preallocated 256 KiB pool for atomic coherent allocations
   [    0.003206] cpuidle: using governor menu
   [    0.003288] imx_soc_device_init: failed to find fsl,imx6ul-ocotp regmap!
   [    0.004858] cpu: supplied by regulator-dummy
   [    0.005281] vddsoc: supplied by regulator-dummy
   [    0.007181] imx6q_suspend_init: failed to find ocram node!
   [    0.007191] imx6_pm_common_init: No DDR LPM support with suspend -19!
   [    0.007596] imx6ul-pinctrl 20e0000.iomuxc: initialized IMX pinctrl driver
   [    0.011952] mxs-dma 1804000.dma-apbh: initialized
   [    0.012422] pps_core: LinuxPPS API ver. 1 registered
   [    0.012429] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
   [    0.012451] PTP clock support registered
   [    0.013273] clocksource: Switched to clocksource mxc_timer1
   [    0.014290] NET: Registered protocol family 2
   [    0.014657] tcp_listen_portaddr_hash hash table entries: 512 (order: 0, 4096 bytes, linear)
   [    0.014687] TCP established hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.014714] TCP bind hash table entries: 1024 (order: 0, 4096 bytes, linear)
   [    0.014738] TCP: Hash tables configured (established 1024 bind 1024)
   [    0.014867] UDP hash table entries: 256 (order: 0, 4096 bytes, linear)
   [    0.014897] UDP-Lite hash table entries: 256 (order: 0, 4096 bytes, linear)
   [    0.016510] workingset: timestamp_bits=30 max_order=15 bucket_order=0
   [    0.018377] 21e8000.serial: ttymxc1 at MMIO 0x21e8000 (irq = 31, base_baud = 5000000) is a IMX
   [    0.018432] printk: console [ttymxc1] enabled
   [    0.018996] libphy: Fixed MDIO Bus: probed
   [    0.019969] mmc0: SDHCI controller on 2190000.usdhc [2190000.usdhc] using ADMA
   [    0.032239] pps pps0: new PPS source ptp0
   [    0.032892] libphy: fec_enet_mii_bus: probed
   [    0.034154] fec 2188000.ethernet eth0: registered PHC device 0
   [    0.034884] sdhci: Secure Digital Host Controller Interface driver
   [    0.034890] sdhci: Copyright(c) Pierre Ossman
   [    0.034894] sdhci-pltfm: SDHCI platform and OF driver helper
   [    0.035089] NET: Registered protocol family 10
   [    0.035898] Segment Routing with IPv6
   [    0.035920] NET: Registered protocol family 17
   [    0.036178] Unpacking initramfs...
   [    0.037730] mmc0: new DDR MMC card at address 0001
   [    0.041913] Freeing initrd memory: 848K
   [    0.042048] ThumbEE CPU extension supported.
   [    0.042782] mmcblk0: mmc0:0001 eMMC   3.64 GiB
   [    0.043147] mmcblk0boot0: mmc0:0001 eMMC   partition 1 4.00 MiB
   [    0.043614] mmcblk0boot1: mmc0:0001 eMMC   partition 2 4.00 MiB
   [    0.043718] mmcblk0rpmb: mmc0:0001 eMMC   partition 3 4.00 MiB, chardev (249:0)
   [    0.045038] Freeing unused kernel memory: 136K
   [    0.045046] Kernel memory protection not selected by kernel config.
   [    0.045051] Run /init as init process
   [    0.045056] with arguments:
   [    0.045059] /init
   [    0.045063] with environment:
   [    0.045068] HOME=/
   [    0.045071] TERM=linux
   [    0.047698] 1970-01-01 00:00:00 INFO default Mounting /dev/mmcblk0p3 on /ext4fs.
   [    0.049437] mmcblk0: p1 p2 p3 p4 p5 p6
   [    0.068758] EXT4-fs (mmcblk0p3): mounted filesystem with ordered data mode. Opts: (null)
   [    0.075144] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.075209] 1970-01-01 00:00:00 INFO default /ext4fs/README: | Monolinux Jiffy |
   [    0.075303] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.082747] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0
   [    0.082829] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0p1
   [    0.082896] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0p2
   [    0.082963] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0p3
   [    0.083026] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0p4
   [    0.084934] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0p5
   [    0.085048] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0p6
   [    0.085113] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/virtual/bdi/179:32
   [    0.085175] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0boot1
   [    0.085234] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/virtual/bdi/179:16
   [    0.085360] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001/block/mmcblk0/mmcblk0boot0
   [    0.085433] 1970-01-01 00:00:00 INFO netlink Event: bind@/devices/soc0/soc/2100000.aips-bus/2190000.usdhc/mmc_host/mmc0/mmc0:0001
   [    0.103023] SMSC LAN8710/LAN8720 2188000.ethernet-1:01: attached PHY driver [SMSC LAN8710/LAN8720] (mii_bus:phy_addr=2188000.ethernet-1:01, irq=POLL)
   [    1.668994] fec 2188000.ethernet eth0: Link is Up - 100Mbps/Full - flow control rx/tx
   [    1.669031] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
   [    1.669468] 1970-01-01 00:00:01 INFO dhcp-client Starting on interface 'eth0'.
   [    1.669638] 1970-01-01 00:00:01 INFO dhcp-client Interface information:
   [    1.669704] 1970-01-01 00:00:01 INFO dhcp-client   MACAddress: 5e:fd:f9:ca:b5:3e
   [    1.669756] 1970-01-01 00:00:01 INFO dhcp-client   Index:      2
   [    1.670783] 1970-01-01 00:00:01 INFO dhcp-client State change from INIT to SELECTING.
   [    1.675903] 1970-01-01 00:00:01 INFO dhcp-client Options:
   [    1.675966] 1970-01-01 00:00:01 INFO dhcp-client   MessageType:     2
   [    1.676028] 1970-01-01 00:00:01 INFO dhcp-client   SubnetMask:      255.255.255.0
   [    1.676085] 1970-01-01 00:00:01 INFO dhcp-client   Gateway:         192.168.0.1
   [    1.676142] 1970-01-01 00:00:01 INFO dhcp-client   DNS:             83.255.255.1
   [    1.676190] 1970-01-01 00:00:01 INFO dhcp-client   LeaseTime:       10800
   [    1.676239] 1970-01-01 00:00:01 INFO dhcp-client   RenewalTime:     5400
   [    1.676285] 1970-01-01 00:00:01 INFO dhcp-client   RebindingTime:   9450
   [    1.676338] 1970-01-01 00:00:01 INFO dhcp-client   ServerIPAddress: 192.168.0.1
   [    1.676404] 1970-01-01 00:00:01 INFO dhcp-client Received OFFER packet.
   [    1.676568] 1970-01-01 00:00:01 INFO dhcp-client State change from SELECTING to REQUESTING.
   [    2.239648] 1970-01-01 00:00:02 INFO dhcp-client Received NONE packet.
   [    2.350576] random: fast init done
   [    2.676052] 1970-01-01 00:00:02 INFO dhcp-client Options:
   [    2.676109] 1970-01-01 00:00:02 INFO dhcp-client   MessageType:     5
   [    2.676171] 1970-01-01 00:00:02 INFO dhcp-client   SubnetMask:      255.255.255.0
   [    2.676226] 1970-01-01 00:00:02 INFO dhcp-client   Gateway:         192.168.0.1
   [    2.676283] 1970-01-01 00:00:02 INFO dhcp-client   DNS:             83.255.255.1
   [    2.676333] 1970-01-01 00:00:02 INFO dhcp-client   LeaseTime:       10800
   [    2.676380] 1970-01-01 00:00:02 INFO dhcp-client   RenewalTime:     5400
   [    2.676427] 1970-01-01 00:00:02 INFO dhcp-client   RebindingTime:   9450
   [    2.676482] 1970-01-01 00:00:02 INFO dhcp-client   ServerIPAddress: 192.168.0.1
   [    2.676549] 1970-01-01 00:00:02 INFO dhcp-client Received ACK packet.
   [    2.676673] 1970-01-01 00:00:02 INFO dhcp-client Configuring interface 'eth0' with ip address 192.168.0.3, subnet mask 255.255.255.0, gateway 192.168.0.1 and mtu 1500.
   [    2.677203] 1970-01-01 00:00:02 INFO dhcp-client State change from REQUESTING to BOUND.
