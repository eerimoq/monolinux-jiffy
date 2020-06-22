#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <asm/byteorder.h>
#include <asm/types.h>
#include <sys/types.h>
#include "jiffy/one_wire.h"

/* DS18B20 commands. */
#define DS18B20_CONVERT_T         0x44
#define DS18B20_READ_SCRATCHPAD   0xbe

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

static struct w1_netlink_msg *init_nl_message(struct one_wire_t *self_p,
                                              uint8_t *buf_p,
                                              size_t size,
                                              struct w1_reg_num *id_p)
{
    struct nlmsghdr *nl_header_p;
    struct cn_msg *cn_msg_p;
    struct w1_netlink_msg *nl_msg_p;

    memset(buf_p, 0, size);

    nl_header_p = (struct nlmsghdr *)buf_p;
    nl_header_p->nlmsg_seq = self_p->send_seq++;
    nl_header_p->nlmsg_type = NLMSG_DONE;

    cn_msg_p = NLMSG_DATA(nl_header_p);
    cn_msg_p->id.idx = CN_W1_IDX;
    cn_msg_p->id.val = CN_W1_VAL;
    cn_msg_p->seq = nl_header_p->nlmsg_seq;
    cn_msg_p->ack = 0;

    nl_msg_p = (struct w1_netlink_msg *)(&cn_msg_p->data[0]);
    nl_msg_p->type = W1_SLAVE_CMD;
    nl_msg_p->len = 0;
    memcpy(nl_msg_p->id.id, id_p, sizeof(nl_msg_p->id.id));

    return (nl_msg_p);
}

static int send_nl_message(struct one_wire_t *self_p,
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
        ML_ERROR("Failed to send: %s [%d].\n", strerror(errno), errno);
        res = -errno;
    } else {
        res = 0;
    }

    return (res);
}

static int start_convertion(struct one_wire_t *self_p,
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

static int read_scratchpad(struct one_wire_t *self_p,
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

static int handle_nl_message_slave_add(struct one_wire_t *self_p,
                                       struct w1_netlink_msg *nl_msg_p)
{
    int res;
    struct w1_reg_num id;

    memcpy(&id, nl_msg_p->id.id, sizeof(id));

    res = start_convertion(self_p, &id);

    if (res != 0) {
        return (res);
    }

    sleep(1);

    return (read_scratchpad(self_p, &id));
}

static void handle_nl_message_slave_command(struct one_wire_t *self_p,
                                            struct w1_netlink_msg *nl_msg_p)
{
    char prefix[32];
    struct w1_reg_num id;
    struct w1_netlink_cmd *cmd_p;
    int offset;
    float temperature;

    if (nl_msg_p->status != 0) {
        ML_ERROR("Netlink message status: %u", nl_msg_p->status);
    }

    memcpy(&id, nl_msg_p->id.id, sizeof(id));

    snprintf(&prefix[0],
             sizeof(prefix),
             "Slave %02x-%012llx:",
             id.family,
             (unsigned long long)id.id);

    offset = 0;

    while ((offset + sizeof(*cmd_p)) <= nl_msg_p->len) {
        cmd_p = (struct w1_netlink_cmd *)(&nl_msg_p->data[offset]);

        if ((sizeof(*cmd_p) + cmd_p->len) > (nl_msg_p->len - offset)) {
            ML_ERROR("%s %s", &prefix[0], "Malformed message.");
            break;
        }

        switch (cmd_p->cmd) {

        case W1_CMD_READ:
            if (cmd_p->len == 9) {
                temperature = (float)(int16_t)((cmd_p->data[1] << 8)
                                               | (cmd_p->data[0] << 0));
                temperature *= 0.0625;

                ML_INFO("%s Temperature: %.2f C", &prefix[0], temperature);
            }

            break;

        default:
            break;
        }

        offset += sizeof(*cmd_p);
        offset += cmd_p->len;
    }
}

static void handle_cn_message_done(struct one_wire_t *self_p,
                                   struct cn_msg *cn_msg_p)
{
    struct w1_netlink_msg *nl_msg_p;
    uint16_t offset;

    offset = 0;

    while (offset < cn_msg_p->len) {
        nl_msg_p = (struct w1_netlink_msg *)(&cn_msg_p->data[offset]);

        switch (nl_msg_p->type) {

        case W1_MASTER_ADD:
        case W1_MASTER_REMOVE:
        case W1_MASTER_CMD:
        case W1_SLAVE_REMOVE:
            break;

        case W1_SLAVE_ADD:
            handle_nl_message_slave_add(self_p, nl_msg_p);
            break;

        case W1_SLAVE_CMD:
            handle_nl_message_slave_command(self_p, nl_msg_p);
            break;

        default:
            ML_WARNING("Unknown message type %d.", nl_msg_p->type);
            break;
        }

        offset += sizeof(*nl_msg_p);
        offset += nl_msg_p->len;
    }
}

static void *onewire_main(struct one_wire_t *self_p)
{
    unsigned char buf[1024];
    ssize_t size;
    struct sockaddr_nl addr;
    struct nlmsghdr *message_p;

    pthread_setname_np(self_p->pthread, "one_wire");

    message_p = (struct nlmsghdr *)&buf[0];

    self_p->fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

    if (self_p->fd == -1) {
        ML_ERROR("socket");

        return (NULL);
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = 23; /* What is this? */

    if (bind(self_p->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        ML_ERROR("bind");
        
        goto out;
    }

    while (true) {
        size = read(self_p->fd, &buf[0], sizeof(buf));

        if (size <= 0) {
            if ((size == -1) && (errno == EINTR)) {
                continue;
            } else {
                break;
            }
        }

        if (size < sizeof(*message_p)) {
            ML_ERROR("Short data.");
            continue;
        }

        switch (message_p->nlmsg_type) {

        case NLMSG_ERROR:
            ML_INFO("Error message received.\n");
            break;

        case NLMSG_DONE:
            handle_cn_message_done(self_p, NLMSG_DATA(message_p));
            break;

        default:
            break;
        }
    }

 out:
    close(self_p->fd);

    return (NULL);
}

void one_wire_init(struct one_wire_t *self_p)
{
    self_p->send_seq = 0;
    ml_log_object_init(&self_p->log_object, "one-wire", ML_LOG_INFO);
    ml_log_object_register(&self_p->log_object);
}

void one_wire_start(struct one_wire_t *self_p)
{
    pthread_create(&self_p->pthread,
                   NULL,
                   (void *(*)(void *))onewire_main,
                   self_p);
}

int one_wire_read_temperature(struct one_wire_t *self_p,
                              uint8_t *slave_id_p,
                              float *temperature_p)
{
    return (-ENOSYS);
}
