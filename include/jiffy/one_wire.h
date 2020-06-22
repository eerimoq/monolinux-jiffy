#ifndef JIFFY_ONE_WIRE_H
#define JIFFY_ONE_WIRE_H

#include <ml/ml.h>

struct one_wire_t {
    int send_seq;
    struct ml_log_object_t log_object;
    pthread_t pthread;
    int fd;
};

/**
 * Initialize the module.
 */
void one_wire_init(struct one_wire_t *self_p);

/**
 * Start the module.
 */
void one_wire_start(struct one_wire_t *self_p);

/**
 * Read the temperature of given slave.
 */
int one_wire_read_temperature(struct one_wire_t *self_p,
                              uint8_t *slave_id_p,
                              float *temperature_p);

#endif
