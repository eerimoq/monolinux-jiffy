#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
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

struct netlink_t {
    pthread_t pthread;
    struct ml_log_object_t log_object;
    char buf[2048];
};

static struct netlink_t netlink;

static void setup_ext4fs(void)
{
    int res;
    int i;
    static const char *modules[] = {
        "/root/mbcache.ko",
        "/root/jbd2.ko",
        "/root/ext4.ko"
    };

    for (i = 0; i < membersof(modules); i++) {
        ml_info("Inserting %s.", modules[i]);

        res = ml_insert_module(modules[i], "");

        if (res == 0) {
            ml_info("Successfully inserted '%s'.", modules[i]);
        } else {
            ml_error("Failed to insert '%s'.", modules[i]);
        }
    }
}

static void on_ext4fs(void)
{
    char line[256];
    FILE *file_p;
    int res;

    ml_info("Mounting /dev/mmcblk0p3 on /ext4fs.");

    res = ml_mount("/dev/mmcblk0p3", "/ext4fs", "ext4", 0, NULL);

    if (res != 0) {
        ml_error("Mount of /dev/mmcblk0p3 on /ext4fs failed.");

        return;
    }

    file_p = fopen("/ext4fs/README", "r");

    if (file_p == NULL) {
        ml_warning("Failed to open /ext4fs/README.");

        return;
    }

    while (fgets(&line[0], sizeof(line), file_p) != NULL) {
        ml_info("/ext4fs/README: %s", ml_strip(&line[0], NULL));
    }

    fclose(file_p);
}

static void create_folders(void)
{
    static const struct folder_t folders[] = {
        { "/proc", 0644 },
        { "/ext4fs", 0777 },
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

static void *netlink_main(struct netlink_t *self_p)
{
    int netlink_fd;
    int res;
    ssize_t size;
    int offset;
    struct sockaddr_nl nladdr;
    bool is_add;

    netlink_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);

    if (netlink_fd == -1) {
        ML_ERROR("socket()");

        return (NULL);
    }

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 1;

    res = bind(netlink_fd, (struct sockaddr *)&nladdr, sizeof(nladdr));

    if (res == -1) {
        ML_ERROR("bind()");

        return (NULL);
    }

    self_p->buf[sizeof(self_p->buf) - 1] = '\0';

    while (true) {
        size = recv(netlink_fd, &self_p->buf[0], sizeof(self_p->buf) - 1, 0);

        if (size <= 0) {
            return (NULL);
        }

        ML_INFO("Event: %s", &self_p->buf[0]);

        is_add = (strncmp(&self_p->buf[0], "add@", 4) == 0);
        offset = 0;

        while (offset < size) {
            ML_DEBUG("%s", &self_p->buf[offset]);

            if (is_add) {
                if (strcmp(&self_p->buf[offset], "DEVNAME=mmcblk0p3") == 0) {
                    ML_INFO("Found mmcblk0p3.");
                    on_ext4fs();
                }
            }

            offset += (strlen(&self_p->buf[offset]) + 1);
        }
    }

    close(netlink_fd);

    return (NULL);
}

static void netlink_start(void)
{
    pthread_create(&netlink.pthread,
                   NULL,
                   (void *(*)(void *))netlink_main,
                   &netlink);
}

int main()
{
    fprintf(stderr, "main()\n");

    pthread_setname_np(pthread_self(), "main");

    create_folders();
    create_files();
    ml_init();
    ml_log_object_init(&netlink.log_object,
                       "netlink",
                       ML_LOG_INFO);
    ml_log_object_register(&netlink.log_object);
    setup_ext4fs();
    set_gpio1_io00_low();
    ml_print_uptime();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    ml_shell_init();
    http_get_module_init();
    pbconfig_module_init();
    ml_network_init();
    ml_shell_start();
    netlink_start();
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
