#include <stdlib.h>
#include <errno.h>
#include "nala.h"
#include "nala_mocks.h"
#include "jiffy/http_get.h"
#include "curl/curl.h"
#include "ml/ml.h"

TEST(http_get_command)
{
    CURL *curl_p;
    int handle;
    const char *argv[] = { "http_get", "https://example.com" };
    long response_code;
    struct nala_ml_shell_register_command_params_t *params_p;

    curl_p = malloc(1);
    response_code = 200;

    handle = ml_shell_register_command_mock_ignore_in_once();
    curl_easy_init_mock_once(curl_p);
    curl_easy_setopt_mock_once(CURLOPT_URL, CURLE_OK, "%s", "https://example.com");
    curl_easy_setopt_mock_once(CURLOPT_WRITEDATA, CURLE_OK, "%p");
    curl_easy_setopt_mock_set_va_arg_in_pointer_at(0, stdout);
    curl_easy_setopt_mock_once(CURLOPT_SSL_VERIFYPEER, CURLE_OK, "%d", 0);
    curl_easy_setopt_mock_once(CURLOPT_SSL_VERIFYHOST, CURLE_OK, "%d", 0);
    curl_easy_perform_mock_once(CURLE_OK);
    curl_easy_getinfo_mock_once(CURLINFO_RESPONSE_CODE, CURLE_OK, "%p");
    curl_easy_getinfo_mock_set_va_arg_out_at(0,
                                             &response_code,
                                             sizeof(response_code));
    curl_easy_cleanup_mock_ignore_in_once();

    http_get_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(2, &argv[0]), 0);
    }

    ASSERT_SUBSTRING(output, ">>> HTTP GET https://example.com. >>>");
    ASSERT_SUBSTRING(output, "<<< HTTP GET response code 200. <<<");

    free(curl_p);
}

TEST(http_get_command_get_error)
{
    CURL *curl_p;
    int handle;
    const char *argv[] = { "http_get", "https://example.com" };
    struct nala_ml_shell_register_command_params_t *params_p;

    curl_p = malloc(1);

    handle = ml_shell_register_command_mock_ignore_in_once();
    curl_easy_init_mock_once(curl_p);
    curl_easy_setopt_mock_once(CURLOPT_URL, CURLE_OK, "%s", "https://example.com");
    curl_easy_setopt_mock_once(CURLOPT_WRITEDATA, CURLE_OK, "%p");
    curl_easy_setopt_mock_set_va_arg_in_pointer_at(0, stdout);
    curl_easy_setopt_mock_once(CURLOPT_SSL_VERIFYPEER, CURLE_OK, "%d", 0);
    curl_easy_setopt_mock_once(CURLOPT_SSL_VERIFYHOST, CURLE_OK, "%d", 0);
    curl_easy_perform_mock_once(CURLE_COULDNT_CONNECT);
    curl_easy_strerror_mock_once(CURLE_COULDNT_CONNECT, "Failed!");
    curl_easy_cleanup_mock_ignore_in_once();

    http_get_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(2, &argv[0]), 0);
    }

    ASSERT_SUBSTRING(output, ">>> HTTP GET https://example.com. >>>");
    ASSERT_SUBSTRING(output, "<<< HTTP GET CURL error code 7: Failed!. <<<");

    free(curl_p);
}

TEST(http_get_command_no_args_error)
{
    const char *argv[] = { "http_get" };
    struct nala_ml_shell_register_command_params_t *params_p;
    int handle;

    handle = ml_shell_register_command_mock_ignore_in_once();

    http_get_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(1, &argv[0]), -EINVAL);
    }

    ASSERT_EQ(output, "Usage: http_get <url>\n");
}
