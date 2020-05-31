#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ml/ml.h"
#include "pb/config.h"
#include "jiffy/pbconfig.h"

static int command_pbconfig_reset(void)
{
    int res;

    res = ml_dd("/dev/zero", "/dev/mmcblk0p5", 512, 512);

    if (res == 0) {
        res = ml_dd("/dev/zero", "/dev/mmcblk0p6", 512, 512);
    }

    return (res);
}

static const char *is_bit_set(uint32_t value, uint32_t bit)
{
    return (ml_bool_str((value & bit) == bit));
}

static void command_pbconfig_print_system(struct config *config_p,
                                          const char *system_p,
                                          uint32_t enabled_bit,
                                          uint32_t verified_bit,
                                          FILE *fout_p)
{
    fprintf(fout_p,
            "System %s:\n"
            "  Enabled:  %s\n"
            "  Verified: %s\n",
            system_p,
            is_bit_set(config_p->enable, enabled_bit),
            is_bit_set(config_p->verified, verified_bit));
}

static int command_pbconfig_status(FILE *fout_p)
{
    struct config config;
    int res;

    res = ml_file_read("/dev/mmcblk0p5", &config, sizeof(config));

    if (res != 0) {
        fprintf(fout_p, "Failed to read config.\n");

        return (res);
    }

    command_pbconfig_print_system(&config,
                                  "A",
                                  PB_CONFIG_A_ENABLED,
                                  PB_CONFIG_A_VERIFIED,
                                  fout_p);
    command_pbconfig_print_system(&config,
                                  "B",
                                  PB_CONFIG_B_ENABLED,
                                  PB_CONFIG_B_VERIFIED,
                                  fout_p);

    return (0);
}

static int command_pbconfig(int argc, const char *argv[], FILE *fout_p)
{
    int res;

    res = -EINVAL;

    if (argc == 2) {
        if (strcmp(argv[1], "reset") == 0) {
            res = command_pbconfig_reset();
        } else if (strcmp(argv[1], "status") == 0) {
            res = command_pbconfig_status(fout_p);
        } else {
            res = -EINVAL;
        }
    }

    if (res != 0) {
        fprintf(fout_p, "Usage: pbconfig {reset,status}\n");
    }

    return (res);
}

void pbconfig_module_init()
{
    ml_shell_register_command("pbconfig",
                              "Punchboot config control.",
                              command_pbconfig);
}
