#include <string.h>
#include "nala.h"
#include "nala_mocks.h"
#include "ml/ml.h"
#include "jiffy/pbconfig.h"
#include "pb/config.h"

TEST(pbconfig_command_no_args_error)
{
    const char *argv[] = { "pbconfig" };
    struct nala_ml_shell_register_command_params_t *params_p;
    int handle;

    handle = ml_shell_register_command_mock_once("pbconfig",
                                                 "Punchboot config control.");

    pbconfig_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(1, &argv[0]), -EINVAL);
    }

    ASSERT_EQ(output, "Usage: pbconfig {reset,status}\n");
}

TEST(pbconfig_command_bad_subcommand)
{
    const char *argv[] = { "pbconfig", "foobar" };
    struct nala_ml_shell_register_command_params_t *params_p;
    int handle;

    handle = ml_shell_register_command_mock_once("pbconfig",
                                                 "Punchboot config control.");

    pbconfig_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(2, &argv[0]), -EINVAL);
    }

    ASSERT_EQ(output, "Usage: pbconfig {reset,status}\n");
}

TEST(pbconfig_command_reset)
{
    const char *argv[] = { "pbconfig", "reset" };
    struct nala_ml_shell_register_command_params_t *params_p;
    int handle;

    handle = ml_shell_register_command_mock_once("pbconfig",
                                                 "Punchboot config control.");
    ml_dd_mock_once("/dev/zero", "/dev/mmcblk0p5", 512, 512, 0);
    ml_dd_mock_once("/dev/zero", "/dev/mmcblk0p6", 512, 512, 0);

    pbconfig_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(2, &argv[0]), 0);
    }

    ASSERT_EQ(output, "");
}

TEST(pbconfig_command_status)
{
    const char *argv[] = { "pbconfig", "status" };
    struct nala_ml_shell_register_command_params_t *params_p;
    int handle;
    struct config config;

    memset(&config, 0, sizeof(config));
    config.magic = PB_CONFIG_MAGIC;
    config.enable = PB_CONFIG_A_ENABLED;
    config.verified = PB_CONFIG_A_VERIFIED;
    config.crc = 0x72c13861; /* Maybe wrong endianess. */

    handle = ml_shell_register_command_mock_once("pbconfig",
                                                 "Punchboot config control.");
    ml_file_read_mock_once("/dev/mmcblk0p5", sizeof(config), 0);
    ml_file_read_mock_set_buf_p_out(&config, sizeof(config));
    ml_bool_str_mock_once(true, "true");
    ml_bool_str_mock_once(true, "true");
    ml_bool_str_mock_once(false, "false");
    ml_bool_str_mock_once(false, "false");

    pbconfig_module_init();

    CAPTURE_OUTPUT(output, errput) {
        params_p = ml_shell_register_command_mock_get_params_in(handle);
        ASSERT_EQ(params_p->callback(2, &argv[0]), 0);
    }

    ASSERT_EQ(output,
              "System A:\n"
              "  Enabled:  true\n"
              "  Verified: true\n"
              "System B:\n"
              "  Enabled:  false\n"
              "  Verified: false\n");
}
