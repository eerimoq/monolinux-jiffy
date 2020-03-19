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
#include <heatshrink_encoder.h>
#include <heatshrink_decoder.h>
#include <detools.h>
#include "ml/ml.h"

struct folder_t {
    const char *path_p;
    int mode;
};

static void http_get(const char *url_p)
{
    CURL *curl_p;
    long response_code;
    int res;

    printf("\n>>> HTTP GET %s. >>>\n", url_p);

    curl_p = curl_easy_init();

    if (curl_p) {
        curl_easy_setopt(curl_p, CURLOPT_URL, url_p);
        curl_easy_setopt(curl_p, CURLOPT_WRITEDATA, stdout);

        /* WARNING: Makes the connection unsecure! */
        curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYHOST, 0);

        res = curl_easy_perform(curl_p);

        if (res == CURLE_OK) {
            curl_easy_getinfo(curl_p, CURLINFO_RESPONSE_CODE, &response_code);
            printf("<<< HTTP GET response code %ld. <<<\n", response_code);
        } else {
            printf("<<< HTTP GET CURL error code %d: %s. <<<\n",
                   res,
                   curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl_p);
    }
}

static int command_http_get(int argc, const char *argv[])
{
    if (argc != 2) {
        printf("Usage: http_get <url>\n");

        return (-1);
    }

    http_get(argv[1]);

    return (0);
}

static int command_punchboot(int argc, const char *argv[])
{
    int res;

    if (argc < 2) {
        printf("Usage: punchboot config -w\n");

        return (-EINVAL);
    }

    if (strcmp(argv[1], "config") == 0) {
        res = ml_dd("/dev/zero", "/dev/mmcblk0p5", 512, 512);

        if (res == 0) {
            res = ml_dd("/dev/zero", "/dev/mmcblk0p6", 512, 512);
        }
    } else {
        res = -EINVAL;
    }

    return (res);
}

static void heatshrink_test(void)
{
    heatshrink_encoder hse;
    heatshrink_decoder hsd;

    printf("Heatshrink encode and decode start.\n");

    heatshrink_encoder_reset(&hse);
    heatshrink_decoder_reset(&hsd);

    printf("Heatshrink encode and decode done.\n");
}

static void detools_test(void)
{
    int res;

    printf("Applying patch 'patch' to 'from' to create 'to'.\n");

    res = detools_apply_patch_filenames("from", "patch", "to");

    if (res >= 0) {
        printf("detools: OK!\n");
    } else {
        res *= -1;
        printf("error: detools: %s (error code %d)\n",
               detools_error_as_string(res),
               res);
    }
}

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
    FILE *file_p;

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
    ml_mknod("/dev/gpiochip1", S_IFCHR | 0644, makedev(254, 1));

    file_p = fopen("/etc/resolv.conf", "w");

    if (file_p != NULL) {
        fwrite("nameserver 8.8.4.4\n", 19, 1, file_p);
        fclose(file_p);
    }
}

static void set_gpio1_io04_low(void)
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
    request.lineoffsets[0] = 3;
    request.flags = GPIOHANDLE_REQUEST_OUTPUT;
    request.lines = 1;
    res = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &request);

    if (res != 0) {
        perror("failed to get GPIO at offset 3");
        close(fd);

        return;
    }

    values.values[0] = 0;
    res = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &values);

    if (res != 0) {
        perror("failed to set GPIO high");
    }

    close(fd);
}

int main()
{
    create_folders();
    create_files();
    ml_init();
    set_gpio1_io04_low();
    ml_print_uptime();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    ml_shell_init();
    ml_shell_register_command("http_get", "HTTP GET.", command_http_get);
    ml_shell_register_command("punchboot",
                              "Punchboot control.",
                              command_punchboot);
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

    ml_dhcp_client_init(&dhcp_client, "eth0", ML_LOG_UPTO(DEBUG));
    ml_dhcp_client_start(&dhcp_client);
#endif

    heatshrink_test();
    detools_test();

    while (true) {
        sleep(10);
    }

    return (0);
}
