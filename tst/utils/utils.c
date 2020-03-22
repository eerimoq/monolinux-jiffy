#include "nala.h"
#include "curl/curl.h"
#include "ml/ml.h"

const char *curl_easy_strerror(CURLcode nala_0)
{
    (void)nala_0;

    FAIL("Must be mocked.");

    return (NULL);
}

CURL *curl_easy_init(void)
{
    FAIL("Must be mocked.");

    return (NULL);
}

CURLcode curl_easy_perform(CURL *curl)
{
    (void)curl;

    FAIL("Must be mocked.");

    return (CURLE_OK);
}

void curl_easy_cleanup(CURL *curl)
{
    (void)curl;

    FAIL("Must be mocked.");
}

CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
    (void)curl;
    (void)option;

    FAIL("Must be mocked.");

    return (CURLE_OK);
}

CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
    (void)curl;
    (void)info;

    FAIL("Must be mocked.");

    return (CURLE_OK);
}

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
