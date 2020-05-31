#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curl/curl.h>
#include "ml/ml.h"

static void http_get(const char *url_p, FILE *fout_p)
{
    CURL *curl_p;
    long response_code;
    int res;

    fprintf(fout_p, "\n>>> HTTP GET %s. >>>\n", url_p);

    curl_p = curl_easy_init();

    if (curl_p == NULL) {
        return;
    }
    
    curl_easy_setopt(curl_p, CURLOPT_URL, url_p);
    curl_easy_setopt(curl_p, CURLOPT_WRITEDATA, fout_p);

    /* WARNING: Makes the connection unsecure! */
    curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYHOST, 0);

    res = curl_easy_perform(curl_p);

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl_p, CURLINFO_RESPONSE_CODE, &response_code);
        fprintf(fout_p, "<<< HTTP GET response code %ld. <<<\n", response_code);
    } else {
        fprintf(fout_p,
                "<<< HTTP GET CURL error code %d: %s. <<<\n",
                res,
                curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_p);
}

static int command_http_get(int argc, const char *argv[], FILE *fout_p)
{
    if (argc != 2) {
        fprintf(fout_p, "Usage: http_get <url>\n");

        return (-EINVAL);
    }

    http_get(argv[1], fout_p);

    return (0);
}

void http_get_module_init()
{
    ml_shell_register_command("http_get", "HTTP GET.", command_http_get);
}
