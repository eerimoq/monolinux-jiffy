#define _GNU_SOURCE
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <asm/byteorder.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "jiffy/one_wire.h"

/* Should be part of the toolchain? */
#define CN_W1_IDX			0x3
#define CN_W1_VAL			0x1

enum w1_netlink_message_types {
    W1_SLAVE_ADD = 0,
    W1_SLAVE_REMOVE,
    W1_MASTER_ADD,
    W1_MASTER_REMOVE,
    W1_MASTER_CMD,
    W1_SLAVE_CMD,
    W1_LIST_MASTERS
};

struct w1_reg_num {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint64_t family : 8;
    uint64_t id : 48;
    uint64_t crc : 8;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint64_t crc : 8;
    uint64_t id : 48;
    uint64_t family : 8;
#else
#error "Please fix <asm/byteorder.h>"
#endif
};

enum w1_commands {
    W1_CMD_READ = 0,
    W1_CMD_WRITE,
    W1_CMD_SEARCH,
    W1_CMD_ALARM_SEARCH,
    W1_CMD_TOUCH,
    W1_CMD_RESET,
    W1_CMD_SLAVE_ADD,
    W1_CMD_SLAVE_REMOVE,
    W1_CMD_LIST_SLAVES,
    W1_CMD_MAX
};

struct cb_id {
    uint32_t idx;
    uint32_t val;
};

struct cn_msg {
    struct cb_id id;
    uint32_t seq;
    uint32_t ack;
    uint16_t len;
    uint16_t flags;
    uint8_t data[0];
};

struct w1_netlink_msg {
    uint8_t type;
    uint8_t status;
    uint16_t len;
    union {
        uint8_t id[8];
        struct w1_mst {
            uint32_t id;
            uint32_t res;
        } mst;
    } id;
    uint8_t data[0];
};

struct w1_netlink_cmd {
    uint8_t cmd;
    uint8_t res;
    uint16_t len;
    uint8_t data[0];
};

/* DS18B20 commands. */
#define DS18B20_CONVERT_T         0x44
#define DS18B20_READ_SCRATCHPAD   0xbe

struct ds18b20_scratchpad_t {
    int16_t temperature;
    int8_t high_trigger;
    int8_t low_trigger;
    uint8_t configuration;
    uint8_t reserved[3];
    uint8_t crc;
} __attribute__ ((packed));

enum ds18b20_state_t {
    ds18b20_state_idle_t = 0,
    ds18b20_state_wait_for_convert_done_t,
    ds18b20_state_wait_for_convert_timeout_t,
    ds18b20_state_wait_for_read_done_t
};

struct module_t {
    int cn_seqno;
    struct ml_queue_t queue;
    struct {
        struct ml_queue_t response_queue;
        pthread_mutex_t mutex;
    } command;
    struct ml_log_object_t log_object;
    pthread_t pthread;
    int fd;
    int put_fd;
    int epoll_fd;
    struct {
        enum ds18b20_state_t state;
        struct w1_reg_num id;
        int seqno;
        struct ml_timer_t timer;
        struct ml_queue_t *response_queue_p;
    } ds18b20;
};

ML_UID(one_wire_read_temperature_req);
ML_UID(one_wire_read_temperature_rsp);
ML_UID(read_temperature_timout);

static struct module_t module;

static const char *ds18b20_state_string(enum ds18b20_state_t state)
{
    const char *res_p;

    switch (state) {

    case ds18b20_state_idle_t:
        res_p = "IDLE";
        break;

    case ds18b20_state_wait_for_convert_done_t:
        res_p = "WAIT-FOR-CONVERT-DONE";
        break;

    case ds18b20_state_wait_for_convert_timeout_t:
        res_p = "WAIT-FOR-CONVERT-TIMEOUT";
        break;

    case ds18b20_state_wait_for_read_done_t:
        res_p = "WAIT-FOR-READ-DONE";
        break;

    default:
        res_p = "INVALID";
        break;
    }

    return (res_p);
}

static void ds18b20_set_state(struct module_t *self_p, enum ds18b20_state_t state)
{
    ML_DEBUG("State change from %s to %s.",
             ds18b20_state_string(self_p->ds18b20.state),
             ds18b20_state_string(state));

    self_p->ds18b20.state = state;
}

/**
 * The sequence number is always positive.
 */
static int next_seqno(struct module_t *self_p)
{
    if (self_p->cn_seqno == 1000) {
        self_p->cn_seqno = 0;
    }

    self_p->cn_seqno++;

    return (self_p->cn_seqno);
}

static struct w1_netlink_msg *init_nl_message(struct module_t *self_p,
                                              uint8_t *buf_p,
                                              size_t size,
                                              struct w1_reg_num *id_p)
{
    struct nlmsghdr *nl_header_p;
    struct cn_msg *cn_msg_p;
    struct w1_netlink_msg *nl_msg_p;

    memset(buf_p, 0, size);

    nl_header_p = (struct nlmsghdr *)buf_p;
    nl_header_p->nlmsg_seq = next_seqno(self_p);
    nl_header_p->nlmsg_type = NLMSG_DONE;

    cn_msg_p = NLMSG_DATA(nl_header_p);
    cn_msg_p->id.idx = CN_W1_IDX;
    cn_msg_p->id.val = CN_W1_VAL;
    cn_msg_p->seq = nl_header_p->nlmsg_seq;
    cn_msg_p->ack = 0;

    nl_msg_p = (struct w1_netlink_msg *)(&cn_msg_p->data[0]);
    nl_msg_p->type = W1_SLAVE_CMD;
    nl_msg_p->len = 0;
    memcpy(&nl_msg_p->id.id[0], id_p, sizeof(nl_msg_p->id.id));

    return (nl_msg_p);
}

static int send_nl_message(struct module_t *self_p,
                           struct w1_netlink_msg *nl_msg_p)
{
    struct nlmsghdr *nl_header_p;
    struct cn_msg *cn_msg_p;
    int res;

    nl_header_p = (struct nlmsghdr *)(((char *)nl_msg_p)
                                      - sizeof(*cn_msg_p)
                                      - sizeof(*nl_header_p));
    nl_header_p->nlmsg_len = NLMSG_LENGTH(sizeof(*cn_msg_p)
                                          + sizeof(*nl_msg_p)
                                          + nl_msg_p->len);
    cn_msg_p = NLMSG_DATA(nl_header_p);
    cn_msg_p->len = (sizeof(*nl_msg_p) + nl_msg_p->len);

    res = send(self_p->fd,
               nl_header_p,
               sizeof(*nl_header_p) + nl_header_p->nlmsg_len,
               0);

    if (res == -1) {
        ML_ERROR("Failed to send with %s(%d).\n", strerror(errno), errno);
        res = -errno;
    } else {
        res = cn_msg_p->seq;
    }

    return (res);
}

static void ds18b20_finalize(struct module_t *self_p,
                             int res,
                             float temperature)
{
    struct one_wire_read_temperature_rsp_t *response_p;

    response_p = ml_message_alloc(&one_wire_read_temperature_rsp,
                                  sizeof(*response_p));
    response_p->res = res;
    response_p->temperature = temperature;

    ml_queue_put(self_p->ds18b20.response_queue_p, response_p);

    ds18b20_set_state(self_p, ds18b20_state_idle_t);
    self_p->ds18b20.seqno = -1;
    self_p->ds18b20.response_queue_p = NULL;
}

static int ds18b20_send_convert(struct module_t *self_p,
                                struct w1_reg_num *id_p)
{
    uint8_t buf[256];
    struct w1_netlink_msg *nl_msg_p;
    struct w1_netlink_cmd *cmd_p;

    nl_msg_p = init_nl_message(self_p, &buf[0], sizeof(buf), id_p);

    cmd_p = (struct w1_netlink_cmd *)(&nl_msg_p->data[nl_msg_p->len]);
    cmd_p->cmd = W1_CMD_WRITE;
    cmd_p->len = 1;
    cmd_p->data[0] = DS18B20_CONVERT_T;
    nl_msg_p->len += (sizeof(*cmd_p) + cmd_p->len);

    return (send_nl_message(self_p, nl_msg_p));
}

static int ds18b20_send_read_scratchpad(struct module_t *self_p,
                                        struct w1_reg_num *id_p)
{
    uint8_t buf[256];
    struct w1_netlink_msg *nl_msg_p;
    struct w1_netlink_cmd *cmd_p;

    nl_msg_p = init_nl_message(self_p, &buf[0], sizeof(buf), id_p);

    cmd_p = (struct w1_netlink_cmd *)(&nl_msg_p->data[nl_msg_p->len]);
    cmd_p->cmd = W1_CMD_WRITE;
    cmd_p->len = 1;
    cmd_p->data[0] = DS18B20_READ_SCRATCHPAD;
    nl_msg_p->len += (sizeof(*cmd_p) + cmd_p->len);

    cmd_p = (struct w1_netlink_cmd *)(&nl_msg_p->data[nl_msg_p->len]);
    cmd_p->cmd = W1_CMD_READ;
    cmd_p->len = 9;
    nl_msg_p->len += (sizeof(*cmd_p) + cmd_p->len);

    return (send_nl_message(self_p, nl_msg_p));
}

static void ds18b20_handle_read_temperature(
    struct module_t *self_p,
    struct one_wire_read_temperature_req_t *request_p)
{
    /* ToDo: Enqueue the request if another request is already active,
       or possibly allow multiple requests in parallel. */
    if (self_p->ds18b20.response_queue_p != NULL) {
        ds18b20_finalize(self_p, -EBUSY, NAN);

        return;
    }

    self_p->ds18b20.response_queue_p = request_p->response_queue_p;
    self_p->ds18b20.id.family = (request_p->slave_id >> 56);
    self_p->ds18b20.id.id = (request_p->slave_id >> 8);
    self_p->ds18b20.id.crc = request_p->slave_id;

    self_p->ds18b20.seqno = ds18b20_send_convert(
        self_p,
        &self_p->ds18b20.id);

    if (self_p->ds18b20.seqno >= 0) {
        ds18b20_set_state(self_p, ds18b20_state_wait_for_convert_done_t);
    } else {
        ds18b20_finalize(self_p, self_p->ds18b20.seqno, NAN);
    }
}

static void ds18b20_handle_read_temperature_convert_done(struct module_t *self_p)
{
    ml_timer_start(&self_p->ds18b20.timer, 750, 0);
    ds18b20_set_state(self_p, ds18b20_state_wait_for_convert_timeout_t);
}

static void ds18b20_handle_read_temperature_timeout(struct module_t *self_p)
{
    self_p->ds18b20.seqno = ds18b20_send_read_scratchpad(
        self_p,
        &self_p->ds18b20.id);

    if (self_p->ds18b20.seqno >= 0) {
        ds18b20_set_state(self_p, ds18b20_state_wait_for_read_done_t);
    } else {
        ds18b20_finalize(self_p, self_p->ds18b20.seqno, NAN);
    }
}

static void ds18b20_handle_read_temperature_read_scratchpad_done(
    struct module_t *self_p,
    struct w1_netlink_cmd *cmd_p)
{
    struct ds18b20_scratchpad_t *scratchpad_p;

    if (cmd_p->len != sizeof(*scratchpad_p)) {
        return;
    }

    scratchpad_p = (struct ds18b20_scratchpad_t *)&cmd_p->data[0];

    ds18b20_finalize(self_p, 0, scratchpad_p->temperature * 0.0625f);
}

static struct w1_netlink_cmd *next_cmd(struct w1_netlink_msg *nl_msg_p,
                                       int offset)
{
    struct w1_netlink_cmd *cmd_p;

    if ((offset + sizeof(*cmd_p)) > nl_msg_p->len) {
        return (NULL);
    }

    cmd_p = (struct w1_netlink_cmd *)(&nl_msg_p->data[offset]);

    if ((offset + sizeof(*cmd_p) + cmd_p->len) > nl_msg_p->len) {
        return (NULL);
    }

    return (cmd_p);
}

static void ds18b20_handle_read_temperature_slave_response(
    struct module_t *self_p,
    struct w1_netlink_msg *nl_msg_p)
{
    struct w1_netlink_cmd *cmd_p;
    int offset;

    if (nl_msg_p->status != 0) {
        ds18b20_finalize(self_p, -nl_msg_p->status, NAN);

        return;
    }

    offset = 0;

    while ((cmd_p = next_cmd(nl_msg_p, offset)) != NULL) {
        switch (cmd_p->cmd) {

        case W1_CMD_WRITE:
            switch (self_p->ds18b20.state) {

            case ds18b20_state_wait_for_convert_done_t:
                ds18b20_handle_read_temperature_convert_done(self_p);
                break;

            default:
                break;
            }

            break;

        case W1_CMD_READ:
            switch (self_p->ds18b20.state) {

            case ds18b20_state_wait_for_read_done_t:
                ds18b20_handle_read_temperature_read_scratchpad_done(self_p, cmd_p);
                break;

            default:
                break;
            }

            break;

        default:
            break;
        }

        offset += (sizeof(*cmd_p) + cmd_p->len);
    }
}

static struct w1_netlink_msg *next_nl_msg(struct cn_msg *cn_msg_p,
                                          int offset)
{
    struct w1_netlink_msg *nl_msg_p;

    if (offset == cn_msg_p->len) {
        return (NULL);
    }

    nl_msg_p = (struct w1_netlink_msg *)(&cn_msg_p->data[offset]);

    return (nl_msg_p);
}

static void handle_message_done(struct module_t *self_p,
                                struct cn_msg *cn_msg_p)
{
    struct w1_netlink_msg *nl_msg_p;
    uint16_t offset;

    offset = 0;

    while ((nl_msg_p = next_nl_msg(cn_msg_p, offset)) != NULL) {
        switch (nl_msg_p->type) {

        case W1_SLAVE_CMD:
            if (cn_msg_p->seq == self_p->ds18b20.seqno) {
                ds18b20_handle_read_temperature_slave_response(self_p, nl_msg_p);
            }

            break;

        default:
            break;
        }

        offset += (sizeof(*nl_msg_p) + nl_msg_p->len);
    }
}

static int handle_socket(struct module_t *self_p)
{
    unsigned char buf[512];
    ssize_t size;
    struct nlmsghdr *message_p;

    size = read(self_p->fd, &buf[0], sizeof(buf));

    if (size <= 0) {
        if ((size == -1) && (errno == EINTR)) {
            return (0);
        } else {
            return (-1);
        }
    }

    message_p = (struct nlmsghdr *)&buf[0];

    if (size < sizeof(*message_p)) {
        ML_ERROR("Short data.");

        return (0);
    }

    switch (message_p->nlmsg_type) {

    case NLMSG_DONE:
        handle_message_done(self_p, NLMSG_DATA(message_p));
        break;

    case NLMSG_ERROR:
        ML_WARNING("Error message received.\n");
        break;

    default:
        break;
    }

    return (0);
}

static int init(struct module_t *self_p)
{
    struct sockaddr_nl addr;
    struct epoll_event event;
    int res;

    pthread_setname_np(self_p->pthread, "one_wire");

    self_p->fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

    if (self_p->fd == -1) {
        ML_ERROR("socket");

        return (-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = 23; /* What is this? */

    if (bind(self_p->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        ML_ERROR("bind");
        goto out;
    }

    self_p->epoll_fd = epoll_create1(0);

    if (self_p->epoll_fd == -1) {
        ML_ERROR("epoll_create1");
        goto out;
    }

    event.events = EPOLLIN;
    event.data.fd = self_p->fd;

    res = epoll_ctl(self_p->epoll_fd, EPOLL_CTL_ADD, self_p->fd, &event);

    if (res == -1) {
        ML_ERROR("epoll_ctl_add");
        goto out;
    }

    event.events = EPOLLIN;
    event.data.fd = self_p->put_fd;

    res = epoll_ctl(self_p->epoll_fd, EPOLL_CTL_ADD, self_p->put_fd, &event);

    if (res == -1) {
        ml_error("epoll_ctl_add");
    }

    return (0);

 out:
    close(self_p->fd);

    return (-1);
}

static void *one_wire_main(struct module_t *self_p)
{
    int res;
    struct epoll_event event;
    uint64_t value;
    void *message_p;
    struct ml_uid_t *uid_p;

    res = init(self_p);

    if (res != 0) {
        return (NULL);
    }

    while (res == 0) {
        res = epoll_wait(self_p->epoll_fd, &event, 1, -1);

        if (res != 1) {
            break;
        }

        if (event.data.fd == self_p->fd) {
            res = handle_socket(self_p);
        } else if (event.data.fd == self_p->put_fd) {
            res = read(self_p->put_fd, &value, sizeof(value));

            if (res != sizeof(value)) {
                break;
            }

            uid_p = ml_queue_get(&self_p->queue, &message_p);

            if (uid_p == &one_wire_read_temperature_req) {
                ds18b20_handle_read_temperature(self_p, message_p);
            } else if (uid_p == &read_temperature_timout) {
                ds18b20_handle_read_temperature_timeout(self_p);
            }

            ml_message_free(message_p);
            res = 0;
        }
    }

    ML_ERROR("Stopping the thread.");

    close(self_p->fd);

    return (NULL);
}

static int ds18b20_command_read(int argc, const char *argv[], FILE *fout_p)
{
    void *message_p;
    struct one_wire_read_temperature_req_t *request_p;
    struct one_wire_read_temperature_rsp_t *response_p;

    if (argc != 3) {
        return (-EINVAL);
    }

    request_p = ml_message_alloc(&one_wire_read_temperature_req, sizeof(*request_p));
    request_p->slave_id = strtoull(argv[2], NULL, 16);
    request_p->response_queue_p = &module.command.response_queue;

    pthread_mutex_lock(&module.command.mutex);

    ml_queue_put(&module.queue, request_p);
    ml_queue_get(&module.command.response_queue, &message_p);

    pthread_mutex_unlock(&module.command.mutex);

    response_p = (struct one_wire_read_temperature_rsp_t *)message_p;

    if (response_p->res == 0) {
        fprintf(fout_p, "Temperature: %.2f C\n", response_p->temperature);
    }

    ml_message_free(message_p);

    return (response_p->res);
}

static int ds18b20_command(int argc, const char *argv[], FILE *fout_p)
{
    int res;

    res = -EINVAL;

    if (argc >= 2) {
        if (strcmp(argv[1], "read") == 0) {
            res = ds18b20_command_read(argc, argv, fout_p);
        }
    }

    if (res != 0) {
        fprintf(fout_p, "Usage: ds18b20 read <sensor-id>\n");
    }

    return (res);
}

static void on_put_signal_event(int *fd_p)
{
    uint64_t value;
    ssize_t size;

    value = 1;
    size = write(*fd_p, &value, sizeof(value));
    (void)size;
}

void one_wire_init(void)
{
    module.cn_seqno = 0;
    module.ds18b20.seqno = -1;
    module.ds18b20.state = ds18b20_state_idle_t;
    module.ds18b20.response_queue_p = NULL;
    ml_queue_init(&module.queue, 32);
    ml_queue_set_on_put(&module.queue,
                        (ml_queue_put_t)on_put_signal_event,
                        &module.put_fd);
    ml_queue_init(&module.command.response_queue, 2);
    pthread_mutex_init(&module.command.mutex, NULL);
    ml_timer_init(&module.ds18b20.timer,
                  &read_temperature_timout,
                  &module.queue);
    ml_log_object_init(&module.log_object, "one-wire", ML_LOG_INFO);
    ml_log_object_register(&module.log_object);
    ml_shell_register_command("ds18b20",
                              "DS18B20 sensor.",
                              ds18b20_command);

    /* Epoll fd. */
    module.epoll_fd = epoll_create1(0);

    if (module.epoll_fd == -1) {
        ml_error("epoll_create1");

        return;
    }

    /* Put fd. */
    module.put_fd = eventfd(0, EFD_SEMAPHORE);

    if (module.put_fd == -1) {
        ml_error("eventfd");

        return;
    }
}

void one_wire_start(void)
{
    pthread_create(&module.pthread,
                   NULL,
                   (void *(*)(void *))one_wire_main,
                   &module);
}
