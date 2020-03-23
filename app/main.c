#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <curl/curl.h>
#include "ml/ml.h"
#include "jiffy/pbconfig.h"
#include "jiffy/http_get.h"

struct folder_t {
    const char *path_p;
    int mode;
};

static void create_folders(void)
{
    static const struct folder_t folders[] = {
        { "/proc", 0644 },
        { "/sys", 0444 },
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
    ml_mount("none", "/sys", "sysfs", 0, NULL);

    ml_mknod("/dev/null", S_IFCHR | 0644, makedev(1, 3));
    ml_mknod("/dev/zero", S_IFCHR | 0644, makedev(1, 5));
    ml_mknod("/dev/urandom", S_IFCHR | 0644, makedev(1, 9));
    ml_mknod("/dev/kmsg", S_IFCHR | 0644, makedev(1, 11));
    ml_mknod("/dev/mmcblk0", S_IFBLK | 0644, makedev(179, 0));
    ml_mknod("/dev/mmcblk0p1", S_IFBLK | 0644, makedev(179, 1));
    ml_mknod("/dev/mmcblk0p2", S_IFBLK | 0644, makedev(179, 2));
    ml_mknod("/dev/mmcblk0p3", S_IFBLK | 0644, makedev(179, 3));
    ml_mknod("/dev/mmcblk0p4", S_IFBLK | 0644, makedev(179, 4));
    ml_mknod("/dev/mmcblk0p5", S_IFBLK | 0644, makedev(179, 5));
    ml_mknod("/dev/mmcblk0p6", S_IFBLK | 0644, makedev(179, 6));
    ml_mknod("/dev/mmcblk0boot0", S_IFBLK | 0644, makedev(179, 16));
    ml_mknod("/dev/mmcblk0boot1", S_IFBLK | 0644, makedev(179, 32));
    ml_mknod("/dev/gpiochip1", S_IFCHR | 0644, makedev(254, 0));
    ml_mknod("/dev/i2c3", S_IFCHR | 0644, makedev(89, 2));

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

int main()
{
    create_folders();
    create_files();
    ml_init();
    set_gpio1_io00_low();
    ml_print_uptime();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    ml_shell_init();
    http_get_module_init();
    pbconfig_module_init();
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

    ml_dhcp_client_init(&dhcp_client, "eth0", ML_LOG_INFO);
    ml_dhcp_client_start(&dhcp_client);
#endif

    while (true) {
        sleep(10);
    }

    return (0);
}
