Measurements
============

+-------------------+--------------+---------+
| Measurement point | Elapsed time | Delta   |
+===================+==============+=========+
| Hardware          | 1 ms         | 1 ms    |
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

Linux boot log
==============

.. code-block:: text

   [    0.000000] Booting Linux on physical CPU 0x0
   [    0.000000] Linux version 4.14.78 (erik@erik-GR8) (gcc version 9.1.0 (GCC)) #1 Sat Jun 20 06:20:55 UTC 2020
   [    0.000000] CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=10c53c7d
   [    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
   [    0.000000] OF: fdt: Machine model: Jiffy
   [    0.000000] Memory policy: Data cache writeback
   [    0.000000] On node 0 totalpages: 32768
   [    0.000000] free_area_init_node: node 0, pgdat c0466878, node_mem_map c7efa000
   [    0.000000] Normal zone: 256 pages used for memmap
   [    0.000000] Normal zone: 0 pages reserved
   [    0.000000] Normal zone: 32768 pages, LIFO batch:7
   [    0.000000] CPU: All CPU(s) started in SVC mode.
   [    0.000000] pcpu-alloc: s0 r0 d32768 u32768 alloc=1*32768
   [    0.000000] pcpu-alloc: [0] 0
   [    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 32512
   [    0.000000] Kernel command line: console=ttymxc1,115200
   [    0.000000] PID hash table entries: 512 (order: -1, 2048 bytes)
   [    0.000000] Dentry cache hash table entries: 16384 (order: 4, 65536 bytes)
   [    0.000000] Inode-cache hash table entries: 8192 (order: 3, 32768 bytes)
   [    0.000000] Memory: 123868K/131072K available (2752K kernel code, 100K rwdata, 416K rodata, 176K init, 1089K bss, 7204K reserved, 0K cma-reserved, 0K highmem)
   [    0.000000] Virtual kernel memory layout:\x0a    vector  : 0xffff0000 - 0xffff1000   (   4 kB)\x0a    fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)\x0a    vmalloc : 0xc8800000 - 0xff800000   ( 880 MB)\x0a    lowmem  : 0xc0000000 - 0xc8000000   ( 128 MB)\x0a    pkmap   : 0xbfe00000 - 0xc0000000   (   2 MB)\x0a    modules : 0xbf000000 - 0xbfe00000   (  14 MB)\x0a      .text : 0xc0108000 - 0xc03b85a0   (2754 kB)\x0a      .init : 0xc0422000 - 0xc044e000   ( 176 kB)\x0a      .data : 0xc044e000 - 0xc04671a0   ( 101 kB)\x0a       .bss : 0xc0469000 - 0xc0579494   (1090 kB)
   [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
   [    0.000000] NR_IRQS: 16, nr_irqs: 16, preallocated irqs: 16
   [    0.000000] Switching to timer-based delay loop, resolution 41ns
   [    0.000014] sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 89478484971ns
   [    0.000039] clocksource: mxc_timer1: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
   [    0.000242] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=48000)
   [    0.000261] pid_max: default: 32768 minimum: 301
   [    0.000448] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes)
   [    0.000463] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes)
   [    0.000774] CPU: Testing write buffer coherency: ok
   [    0.001475] Setting up static identity map for 0x80108200 - 0x80108260
   [    0.002333] VFP support v0.3: implementor 41 architecture 2 part 30 variant 7 rev 5
   [    0.002421] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 3822520892550000 ns
   [    0.002444] futex hash table entries: 256 (order: -1, 3072 bytes)
   [    0.002480] pinctrl core: initialized pinctrl subsystem
   [    0.003182] random: get_random_u32 called from 0xc0226880 with crng_init=0
   [    0.003305] NET: Registered protocol family 16
   [    0.003683] DMA: preallocated 256 KiB pool for atomic coherent allocations
   [    0.003892] cpuidle: using governor menu
   [    0.005643] cpu: supplied by regulator-dummy
   [    0.006045] vddsoc: supplied by regulator-dummy
   [    0.008092] imx6q_suspend_init: failed to find ocram node!
   [    0.008102] imx6_pm_common_init: No DDR LPM support with suspend -19!
   [    0.008566] imx6ul-pinctrl 20e0000.iomuxc: initialized IMX pinctrl driver
   [    0.011467] mxs-dma 1804000.dma-apbh: initialized
   [    0.011914] pps_core: LinuxPPS API ver. 1 registered
   [    0.011922] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
   [    0.011944] PTP clock support registered
   [    0.012688] clocksource: Switched to clocksource mxc_timer1
   [    0.013307] NET: Registered protocol family 2
   [    0.013668] TCP established hash table entries: 1024 (order: 0, 4096 bytes)
   [    0.013703] TCP bind hash table entries: 1024 (order: 0, 4096 bytes)
   [    0.013729] TCP: Hash tables configured (established 1024 bind 1024)
   [    0.013874] UDP hash table entries: 256 (order: 0, 4096 bytes)
   [    0.013899] UDP-Lite hash table entries: 256 (order: 0, 4096 bytes)
   [    0.016057] workingset: timestamp_bits=30 max_order=15 bucket_order=0
   [    0.016512] mmc0: SDHCI controller on 2190000.usdhc [2190000.usdhc] using ADMA
   [    0.019666] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 250)
   [    0.019681] io scheduler noop registered (default)
   [    0.021194] 21e8000.serial: ttymxc1 at MMIO 0x21e8000 (irq = 31, base_baud = 5000000) is a IMX
   [    0.021256] console [ttymxc1] enabled
   [    0.021916] libphy: Fixed MDIO Bus: probed
   [    0.022483] imx2-wdt 20bc000.wdog: timeout 60 sec (nowayout=0)
   [    0.022764] sdhci: Secure Digital Host Controller Interface driver
   [    0.022770] sdhci: Copyright(c) Pierre Ossman
   [    0.022774] sdhci-pltfm: SDHCI platform and OF driver helper
   [    0.022946] NET: Registered protocol family 10
   [    0.023825] Segment Routing with IPv6
   [    0.023846] NET: Registered protocol family 17
   [    0.024231] Unpacking initramfs...
   [    0.034617] Freeing initrd memory: 1416K
   [    0.034769] ThumbEE CPU extension supported.
   [    0.035625] mmc0: new DDR MMC card at address 0001
   [    0.036022] mmcblk0: mmc0:0001 eMMC   3.64 GiB
   [    0.037163] Freeing unused kernel memory: 176K
   [    0.037170] This architecture does not have kernel memory protection.
   [    0.039531] 1970-01-01 00:00:00 INFO default Inserting /root/fec.ko.
   [    0.042079] mmcblk0boot0: mmc0:0001 eMMC   partition 1 4.00 MiB
   [    0.042363] mmcblk0boot1: mmc0:0001 eMMC   partition 2 4.00 MiB
   [    0.042575] fec 2188000.ethernet: 2188000.ethernet supply phy not found, using dummy regulator
   [    0.042849] mmcblk0rpmb: mmc0:0001 eMMC   partition 3 4.00 MiB
   [    0.043672] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/fec.ko'.
   [    0.043735] 1970-01-01 00:00:00 INFO default Inserting /root/mbcache.ko.
   [    0.045061] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/mbcache.ko'.
   [    0.045124] 1970-01-01 00:00:00 INFO default Inserting /root/jbd2.ko.
   [    0.046033] mmcblk0: p1 p2 p3 p4 p5 p6
   [    0.048346] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/jbd2.ko'.
   [    0.048413] 1970-01-01 00:00:00 INFO default Inserting /root/ext4.ko.
   [    0.054583] 1970-01-01 00:00:00 INFO default Successfully inserted '/root/ext4.ko'.
   [    0.054944] 1970-01-01 00:00:00 INFO default Mounting /dev/mmcblk0p3 on /ext4fs.
   [    0.056640] pps pps0: new PPS source ptp0
   [    0.057494] libphy: fec_enet_mii_bus: probed
   [    0.059567] fec 2188000.ethernet eth0: registered PHC device 0
   [    0.072903] EXT4-fs (mmcblk0p3): mounted filesystem with ordered data mode. Opts: (null)
   [    0.079491] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.079557] 1970-01-01 00:00:00 INFO default /ext4fs/README: | Monolinux Jiffy |
   [    0.079611] 1970-01-01 00:00:00 INFO default /ext4fs/README: +-----------------+
   [    0.085016] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2188000.ethernet/ptp/ptp0
   [    0.085097] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/virtual/pps/pps0
   [    0.085164] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2188000.ethernet/mdio_bus/2188000.ethernet-1
   [    0.085231] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2188000.ethernet/mdio_bus/2188000.ethernet-1/2188000.ethernet-1:01
   [    0.085297] 1970-01-01 00:00:00 INFO netlink Event: bind@/devices/soc0/soc/2100000.aips-bus/2188000.ethernet/mdio_bus/2188000.ethernet-1/2188000.ethernet-1:01
   [    0.087168] 1970-01-01 00:00:00 INFO netlink Event: add@/devices/soc0/soc/2100000.aips-bus/2188000.ethernet/net/eth0
   [    0.087288] 1970-01-01 00:00:00 INFO netlink Event: bind@/devices/soc0/soc/2100000.aips-bus/2188000.ethernet
   [    0.108247] SMSC LAN8710/LAN8720 2188000.ethernet-1:01: attached PHY driver [SMSC LAN8710/LAN8720] (mii_bus:phy_addr=2188000.ethernet-1:01, irq=POLL)
   [    0.108407] IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
   [    1.668246] fec 2188000.ethernet eth0: Link is Up - 100Mbps/Full - flow control rx/tx
   [    1.668282] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
   [    1.668753] 1970-01-01 00:00:01 INFO dhcp-client Starting on interface 'eth0'.
   [    1.668948] 1970-01-01 00:00:01 INFO dhcp-client Interface information:
   [    1.669016] 1970-01-01 00:00:01 INFO dhcp-client   MACAddress: 5e:fd:f9:ca:b5:3e
   [    1.669068] 1970-01-01 00:00:01 INFO dhcp-client   Index:      2
   [    1.670243] 1970-01-01 00:00:01 INFO dhcp-client State change from INIT to SELECTING.
   [    1.675489] 1970-01-01 00:00:01 INFO dhcp-client Options:
   [    1.675552] 1970-01-01 00:00:01 INFO dhcp-client   MessageType:     2
   [    1.675616] 1970-01-01 00:00:01 INFO dhcp-client   SubnetMask:      255.255.255.0
   [    1.675681] 1970-01-01 00:00:01 INFO dhcp-client   Gateway:         192.168.0.1
   [    1.675738] 1970-01-01 00:00:01 INFO dhcp-client   DNS:             83.255.255.1
   [    1.675789] 1970-01-01 00:00:01 INFO dhcp-client   LeaseTime:       10800
   [    1.675838] 1970-01-01 00:00:01 INFO dhcp-client   RenewalTime:     5400
   [    1.675886] 1970-01-01 00:00:01 INFO dhcp-client   RebindingTime:   9450
   [    1.675939] 1970-01-01 00:00:01 INFO dhcp-client   ServerIPAddress: 192.168.0.1
   [    1.676013] 1970-01-01 00:00:01 INFO dhcp-client Received OFFER packet.
   [    1.676196] 1970-01-01 00:00:01 INFO dhcp-client State change from SELECTING to REQUESTING.
   [    2.321510] 1970-01-01 00:00:02 INFO dhcp-client Received NONE packet.
   [    2.673436] 1970-01-01 00:00:02 INFO dhcp-client Options:
   [    2.673497] 1970-01-01 00:00:02 INFO dhcp-client   MessageType:     5
   [    2.673558] 1970-01-01 00:00:02 INFO dhcp-client   SubnetMask:      255.255.255.0
   [    2.673619] 1970-01-01 00:00:02 INFO dhcp-client   Gateway:         192.168.0.1
   [    2.673678] 1970-01-01 00:00:02 INFO dhcp-client   DNS:             83.255.255.1
   [    2.673729] 1970-01-01 00:00:02 INFO dhcp-client   LeaseTime:       10800
   [    2.673777] 1970-01-01 00:00:02 INFO dhcp-client   RenewalTime:     5400
   [    2.673825] 1970-01-01 00:00:02 INFO dhcp-client   RebindingTime:   9450
   [    2.673880] 1970-01-01 00:00:02 INFO dhcp-client   ServerIPAddress: 192.168.0.1
   [    2.673946] 1970-01-01 00:00:02 INFO dhcp-client Received ACK packet.
   [    2.674072] 1970-01-01 00:00:02 INFO dhcp-client Configuring interface 'eth0' with ip address 192.168.0.3, subnet mask 255.255.255.0, gateway 192.168.0.1 and mtu 1500.
   [    2.674732] random: fast init done
   [    2.674767] 1970-01-01 00:00:02 INFO dhcp-client State change from REQUESTING to BOUND.
