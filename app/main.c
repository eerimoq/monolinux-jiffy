#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <heatshrink_encoder.h>
#include <heatshrink_decoder.h>
#include <lzma.h>
#include <detools.h>
#include "ml/ml.h"

static size_t on_write(void *buf_p, size_t size, size_t nmemb, void *arg_p)
{
    (void)arg_p;

    fwrite(buf_p, size, nmemb, stdout);

    return (size * nmemb);
}

static void http_get(const char *url_p)
{
    CURL *curl_p;
    long response_code;
    int res;

    printf("\n>>> HTTP GET %s. >>>\n", url_p);

    curl_p = curl_easy_init();

    if (curl_p) {
        curl_easy_setopt(curl_p, CURLOPT_URL, url_p);
        curl_easy_setopt(curl_p, CURLOPT_WRITEFUNCTION, on_write);

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
        printf("http_get <url>\n");

        return (-1);
    }

    http_get(argv[1]);

    return (0);
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

static void lzma_test(void)
{
    lzma_ret ret;
    lzma_stream stream;

    memset(&stream, 0, sizeof(stream));

    ret = lzma_alone_decoder(&stream, UINT64_MAX);

    if (ret != LZMA_OK) {
        printf("LZMA decoder init failed.\n");
    } else {
        printf("LZMA decoder init successful.\n");
    }
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

int main()
{
    ml_mount("none", "/proc", "proc", 0, NULL);
    ml_mount("none", "/sys", "sysfs", 0, NULL);

    ml_print_uptime();

    curl_global_init(CURL_GLOBAL_DEFAULT);

    ml_init();
    ml_shell_init();
    ml_shell_register_command("http_get", "HTTP GET.", command_http_get);
    ml_network_init();
    ml_shell_start();
    ml_network_interface_up("eth0");

# if 0
    ml_network_interface_configure("eth0", "192.168.0.100", "255.255.255.0");
    ml_network_interface_add_route("eth0", "192.168.0.1");
#else
    struct ml_dhcp_client_t dhcp_client;

    ml_dhcp_client_init(&dhcp_client, "eth0", ML_LOG_UPTO(DEBUG));
    ml_dhcp_client_start(&dhcp_client);
#endif

    heatshrink_test();
    lzma_test();
    detools_test();

    while (true) {
        sleep(10);
    }

    return (0);
}
