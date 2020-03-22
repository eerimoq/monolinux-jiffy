#include "nala.h"
#include "curl/curl.h"
#include "ml/ml.h"

CURLcode curl_easy_setopt_mock_va_arg_real(CURL *curl,
                                           CURLoption option,
                                           va_list __nala_va_list)
{
    (void)curl;
    (void)option;
    (void)__nala_va_list;

    FAIL("Must be mocked.");

    return (CURLE_OK);
}

CURLcode curl_easy_getinfo_mock_va_arg_real(CURL *curl,
                                            CURLINFO info,
                                            va_list __nala_va_list)
{
    (void)curl;
    (void)info;
    (void)__nala_va_list;

    FAIL("Must be mocked.");

    return (CURLE_OK);
}

void ml_shell_register_command(const char *name_p,
                               const char *description_p,
                               ml_shell_command_callback_t callback)
{
    (void)name_p;
    (void)description_p;
    (void)callback;

    FAIL("Must be mocked.");
}
