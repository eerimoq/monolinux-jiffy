#ifndef JIFFY_ONE_WIRE_H
#define JIFFY_ONE_WIRE_H

#include <ml/ml.h>

/*
 * Messages to read the temperature. It takes up to 750 ms for the
 * operation to complete.
 */
struct one_wire_read_temperature_req_t {
    /* Slave to read from. 1 byte family, 6 bytes id and 1 byte
       crc. */
    uint64_t slave_id;

    /* A response message is put on this queue when the operation is
       complete. */
    struct ml_queue_t *response_queue_p;
};

struct one_wire_read_temperature_rsp_t {
    /* Zero if successful, otherwise negative error code. */
    int res;

    /* The temperature in degrees Celsius. */
    float temperature;
};

/* Read temperature message identifiers. */
extern struct ml_uid_t one_wire_read_temperature_req;
extern struct ml_uid_t one_wire_read_temperature_rsp;

/**
 * Initialize the module.
 */
void one_wire_init(void);

/**
 * Start the module.
 */
void one_wire_start(void);

#endif
