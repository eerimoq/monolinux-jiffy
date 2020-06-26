#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <unistd.h>
#include "ml/ml.h"

static int http_get(const char *url_p, FILE *fout_p)
{
    CURL *curl_p;
    long response_code;
    int ret;
    int res;

    res = -EGENERAL;

    fprintf(fout_p, "\n>>> HTTP GET %s. >>>\n", url_p);

    curl_p = curl_easy_init();

    if (curl_p == NULL) {
        return (res);
    }

    curl_easy_setopt(curl_p, CURLOPT_URL, url_p);
    curl_easy_setopt(curl_p, CURLOPT_WRITEDATA, fout_p);

    /* WARNING: Makes the connection unsecure! */
    curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYHOST, 0);

    ret = curl_easy_perform(curl_p);

    if (ret == CURLE_OK) {
        ret = curl_easy_getinfo(curl_p, CURLINFO_RESPONSE_CODE, &response_code);

        if (ret == CURLE_OK) {
            fprintf(fout_p, "<<< HTTP GET response code %ld. <<<\n", response_code);

            if (response_code == 200) {
                res = 0;
            }
        } else {
            fprintf(fout_p, "<<< HTTP GET without response code. <<<\n");
        }
    } else {
        fprintf(fout_p,
                "<<< HTTP GET CURL error code %d: %s. <<<\n",
                ret,
                curl_easy_strerror(ret));
    }

    curl_easy_cleanup(curl_p);

    return (res);
}

static int command_http_get(int argc, const char *argv[], FILE *fout_p)
{
    if (argc != 2) {
        fprintf(fout_p, "Usage: http_get <url>\n");

        return (-EINVAL);
    }

    return (http_get(argv[1], fout_p));
}

void http_get_module_init()
{
    ml_shell_register_command("http_get", "HTTP GET.", command_http_get);
}
