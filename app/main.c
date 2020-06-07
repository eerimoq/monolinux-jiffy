#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <linux/gpio.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <curl/curl.h>
#include "ml/ml.h"
#include "jiffy/pbconfig.h"
#include "jiffy/http_get.h"
#include "bunga_server_linux.h"

struct folder_t {
    const char *path_p;
    int mode;
};

static void insert_modules(void)
{
    int res;
    int i;
    static const char *modules[] = {
        "/root/cn.ko",
        "/root/wire.ko",
        "/root/w1-gpio.ko",
        "/root/w1_therm.ko"
    };

    for (i = 0; i < membersof(modules); i++) {
        res = ml_insert_module(modules[i], "");

        if (res == 0) {
            printf("Successfully inserted '%s'.\n", modules[i]);
        } else {
            printf("Failed to insert '%s'.\n", modules[i]);
        }
    }
}

static void create_folders(void)
{
    static const struct folder_t folders[] = {
        { "/proc", 0644 },
        /* { "/sys", 0444 }, */
        { "/etc", 0644 }
    };
    int i;
    int res;

    for (i = 0; i < membersof(folders); i++) {
        res = mkdir(folders[i].path_p, folders[i].mode);

        if (res != 0) {
            fprintf(stderr, "Failed to create '%s'", folders[i].path_p);
        }
    }
}

static void create_files(void)
{
    ml_mount("none", "/proc", "proc", 0, NULL);
    /* ml_mount("none", "/sys", "sysfs", 0, NULL); */

    mknod("/dev/null", S_IFCHR | 0644, makedev(1, 3));
    mknod("/dev/zero", S_IFCHR | 0644, makedev(1, 5));
    mknod("/dev/urandom", S_IFCHR | 0644, makedev(1, 9));
    mknod("/dev/kmsg", S_IFCHR | 0644, makedev(1, 11));
    mknod("/dev/mmcblk0", S_IFBLK | 0644, makedev(179, 0));
    mknod("/dev/mmcblk0p1", S_IFBLK | 0644, makedev(179, 1));
    mknod("/dev/mmcblk0p2", S_IFBLK | 0644, makedev(179, 2));
    mknod("/dev/mmcblk0p3", S_IFBLK | 0644, makedev(179, 3));
    mknod("/dev/mmcblk0p4", S_IFBLK | 0644, makedev(179, 4));
    mknod("/dev/mmcblk0p5", S_IFBLK | 0644, makedev(179, 5));
    mknod("/dev/mmcblk0p6", S_IFBLK | 0644, makedev(179, 6));
    mknod("/dev/mmcblk0boot0", S_IFBLK | 0644, makedev(179, 16));
    mknod("/dev/mmcblk0boot1", S_IFBLK | 0644, makedev(179, 32));
    mknod("/dev/gpiochip1", S_IFCHR | 0644, makedev(254, 0));
    mknod("/dev/i2c3", S_IFCHR | 0644, makedev(89, 2));

    ml_file_write_string("/etc/resolv.conf", "nameserver 8.8.4.4\n");
}

static void set_gpio1_io00_low(void)
{
    int fd;
    int res;
    struct gpiohandle_request request;
    struct gpiohandle_data values;

    fd = open("/dev/gpiochip1", O_RDONLY);

    if (fd == -1) {
        perror("failed to open /dev/gpiochip1");

        return;
    }

    memset(&request, 0, sizeof(request));
    request.lineoffsets[0] = 0;
    request.flags = GPIOHANDLE_REQUEST_OUTPUT;
    request.lines = 1;
    res = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &request);

    if (res != 0) {
        perror("failed to get GPIO at offset 0");
        close(fd);

        return;
    }

    values.values[0] = 0;
    res = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &values);

    if (res != 0) {
        perror("failed to set GPIO low");
    }

    close(fd);
}

/**
 * This should probably be moved to the DHCP client (or Monlinux C
 * Library network module).
 *
 * Right now this function waits for any new network link, not only
 * eth0.
 */
static void wait_for_eth0_up(void)
{
    struct sockaddr_nl addr;
    int sockfd;
    char buf[4096];
    struct nlmsghdr *nlh_p;
    int res;

    sockfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    if (sockfd == -1) {
        perror("couldn't open NETLINK_ROUTE socket");

        return;
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK;

    res = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    if (res == -1) {
        perror("couldn't bind");

        return;
    }

    nlh_p = (struct nlmsghdr *)buf;

    while (true) {
        res = read(sockfd, nlh_p, 4096);

        if (res <= 0) {
            break;
        }

        while ((NLMSG_OK(nlh_p, res)) && (nlh_p->nlmsg_type != NLMSG_DONE)) {
            if (nlh_p->nlmsg_type == RTM_NEWLINK) {
                close(sockfd);
            }

            nlh_p = NLMSG_NEXT(nlh_p, res);
        }
    }
}

static void ds18b20_usage(FILE *fout_p)
{
    fprintf(fout_p,
            "Usage: ds18b20 read <id>\n"
            "         where\n"
            "           <id> is a device id on the form 28-0000055d3295.\n"
            "                Run \"ls sys/bus/w1/devices\" to list devices.\n");
}

static int ds18b20_read(int argc, const char *argv[], FILE *fout_p)
{
    int res;
    char path[64];
    char output[75];
    int temperature;

    if (argc != 3) {
        ds18b20_usage(fout_p);

        return (-EINVAL);
    }

    snprintf(&path[0], sizeof(path), "/sys/bus/w1/devices/%s/w1_slave", argv[2]);

    res = ml_file_read(&path[0], &output[0], sizeof(output));

    if (res != 0) {
        fprintf(fout_p, "Failed to read from %s.\n", &path[0]);

        return (-EGENERAL);
    }

    if (strncmp(&output[36], "YES", 3) != 0) {
        fprintf(fout_p, "Wrong CRC.\n");

        return (-EGENERAL);
    }

    temperature = atoi(&output[69]);

    fprintf(fout_p,
            "Temperature: %d.%u C\n",
            temperature / 1000,
            temperature % 1000);

    return (0);
}

static int ds18b20(int argc, const char *argv[], FILE *fout_p)
{
    int res;

    if (argc < 2) {
        ds18b20_usage(fout_p);

        return (-EINVAL);
    }

    if (strcmp(argv[1], "read") == 0) {
        res = ds18b20_read(argc, &argv[0], fout_p);
    } else {
        res = -EINVAL;
    }

    return (res);
}

int main()
{
    pthread_setname_np(pthread_self(), "main");

    insert_modules();
    create_folders();
    create_files();
    ml_init();
    set_gpio1_io00_low();
    ml_print_uptime();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    ml_shell_init();
    http_get_module_init();
    pbconfig_module_init();
    ml_shell_register_command("ds18b20",
                              "Dallas DS18B20 sensor.",
                              ds18b20);
    ml_network_init();
    ml_shell_start();
    ml_network_interface_up("eth0");

# if 0
    ml_network_interface_configure("eth0",
                                   "192.168.0.100",
                                   "255.255.255.0",
                                   1500);
    ml_network_interface_add_route("eth0", "192.168.0.1");
#else
    struct ml_dhcp_client_t dhcp_client;

    wait_for_eth0_up();
    ml_dhcp_client_init(&dhcp_client, "eth0", ML_LOG_INFO);
    ml_dhcp_client_start(&dhcp_client);
#endif

    bunga_server_linux_create();

    while (true) {
        sleep(10);
    }

    return (0);
}
