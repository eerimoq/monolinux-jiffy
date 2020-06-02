/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Monolinux project.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include "bunga_server.h"
#include "ml/ml.h"

struct execute_command_t {
    char *command_p;
    int res;
    struct {
        char *buf_p;
        size_t size;
    } output;
    struct {
        struct bunga_server_t *server_p;
        struct bunga_server_client_t *client_p;
    } bunga;
    struct ml_queue_t *queue_p;
};

struct log_client_t {
    struct bunga_server_client_t *client_p;
    int log_fd;
};

static struct bunga_server_client_t clients[2];
static struct log_client_t log_clients[2];
static struct ml_queue_t queue;
static int epoll_fd;

static ML_UID(uid_execute_command_complete);

static struct log_client_t *log_client_from_client(
    struct bunga_server_client_t *client_p)
{
    return (&log_clients[client_p - &clients[0]]);
}

static struct bunga_server_client_t *log_client_to_client(
    struct log_client_t *client_p)
{
    return (&clients[client_p - &log_clients[0]]);
}

static void log_client_init(struct log_client_t *self_p)
{
    struct epoll_event event;

    self_p->log_fd = open("/dev/kmsg", O_RDONLY | O_NONBLOCK);

    if (self_p->log_fd != -1) {
        event.data.fd = self_p->log_fd;
        event.events = EPOLLIN;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, self_p->log_fd, &event);
    }
}

static void log_client_destroy(struct log_client_t *self_p)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, self_p->log_fd, NULL);
    close(self_p->log_fd);
    self_p->log_fd = -1;
}

static void on_client_connected(struct bunga_server_t *self_p,
                                struct bunga_server_client_t *client_p)
{
    (void)self_p;

    printf("Bunga client connected.\n");

    log_client_init(log_client_from_client(client_p));
}

static void on_client_disconnected(struct bunga_server_t *self_p,
                                   struct bunga_server_client_t *client_p)
{
    (void)self_p;

    printf("Bunga client disconnected.\n");

    log_client_destroy(log_client_from_client(client_p));
}

static void execute_command_job(struct execute_command_t *command_p)
{
    FILE *fout_p;

    fout_p = open_memstream(&command_p->output.buf_p, &command_p->output.size);

    if (fout_p != NULL) {
        command_p->res = ml_shell_execute_command(command_p->command_p, fout_p);
    } else {
        command_p->res = -ENOMEM;
    }

    fclose(fout_p);

    ml_queue_put(command_p->queue_p, command_p);
}

static void on_execute_command_req(struct bunga_server_t *self_p,
                                   struct bunga_server_client_t *client_p,
                                   struct bunga_execute_command_req_t *request_p)
{
    struct execute_command_t *command_p;

    command_p = ml_message_alloc(&uid_execute_command_complete, sizeof(*command_p));
    command_p->command_p = strdup(request_p->command_p);
    command_p->bunga.server_p = self_p;
    command_p->bunga.client_p = client_p;
    command_p->queue_p = &queue;
    ml_spawn((ml_worker_pool_job_entry_t)execute_command_job, command_p);
}

static void on_get_file_req(struct bunga_server_t *self_p,
                            struct bunga_server_client_t *client_p,
                            struct bunga_get_file_req_t *request_p)
{
    (void)client_p;
    (void)request_p;

    struct bunga_get_file_rsp_t *response_p;

    response_p = bunga_server_init_get_file_rsp(self_p);
    response_p->error_p = strerror(ENOSYS);
    bunga_server_reply(self_p);
}

static void on_put_file_req(struct bunga_server_t *self_p,
                            struct bunga_server_client_t *client_p,
                            struct bunga_put_file_req_t *request_p)
{
    (void)client_p;
    (void)request_p;

    struct bunga_put_file_rsp_t *response_p;

    response_p = bunga_server_init_put_file_rsp(self_p);
    response_p->error_p = strerror(ENOSYS);
    bunga_server_reply(self_p);
}

static void handle_execute_command_complete(struct execute_command_t *command_p)
{
    struct bunga_execute_command_rsp_t *response_p;
    char *output_p;
    size_t offset;
    size_t size;
    char saved;
    size_t chunk_size;
    struct bunga_server_t *server_p;
    struct bunga_server_client_t *client_p;

    server_p = command_p->bunga.server_p;
    client_p = command_p->bunga.client_p;

    /* Output. */
    output_p = command_p->output.buf_p;
    size = command_p->output.size;
    chunk_size = 96;

    for (offset = 0; offset < size; offset += chunk_size) {
        if ((size - offset) < 96) {
            chunk_size = (size - offset);
        }

        saved = output_p[offset + chunk_size];
        output_p[offset + chunk_size] = '\0';

        response_p = bunga_server_init_execute_command_rsp(server_p);
        response_p->output_p = &output_p[offset];
        bunga_server_send(server_p, client_p);

        output_p[offset + chunk_size] = saved;
    }

    /* Command result. */
    response_p = bunga_server_init_execute_command_rsp(server_p);

    if (command_p->res != 0) {
        response_p->error_p = strerror(-command_p->res);
    }

    bunga_server_send(server_p, client_p);

    free(command_p->command_p);
    free(command_p->output.buf_p);
    ml_message_free(command_p);
}

static void print_kernel_message(char *message_p,
                                 struct bunga_server_t *server_p,
                                 struct bunga_server_client_t *client_p)
{
    unsigned long long secs;
    unsigned long long usecs;
    int text_pos;
    char *text_p;
    char *match_p;
    char header[32];
    struct bunga_log_entry_ind_t *indication_p;

    if (sscanf(message_p, "%*u,%*u,%llu,%*[^;]; %n", &usecs, &text_pos) != 1) {
        return;
    }

    text_p = &message_p[text_pos];
    match_p = strchr(text_p, '\n');

    if (match_p != NULL) {
        *match_p = '\0';
    }

    secs = (usecs / 1000000);
    usecs %= 1000000;

    snprintf(&header[0], sizeof(header), "[%5lld.%06lld]", secs, usecs);
    indication_p = bunga_server_init_log_entry_ind(server_p);

    if (bunga_log_entry_ind_text_alloc(indication_p, 2) != 0) {
        return;
    }

    indication_p->text.items_pp[0] = &header[0];
    indication_p->text.items_pp[1] = text_p;
    bunga_server_send(server_p, client_p);
}

static struct log_client_t *find_log_client(int log_fd)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (log_fd == log_clients[i].log_fd) {
            return (&log_clients[i]);
        }
    }

    return (NULL);
}

static bool handle_log(struct bunga_server_t *server_p, int log_fd)
{
    char message[1024];
    ssize_t size;
    struct log_client_t *client_p;

    client_p = find_log_client(log_fd);

    if (client_p == NULL) {
        return (false);
    }

    while (true) {
        size = read(log_fd, &message[0], sizeof(message) - 1);

        if (size <= 0) {
            break;
        }

        message[size] = '\0';
        print_kernel_message(&message[0],
                             server_p,
                             log_client_to_client(client_p));
    }

    return (true);
}

static void on_put_signal_event(int *fd_p)
{
    uint64_t value;
    ssize_t size;

    value = 1;
    size = write(*fd_p, &value, sizeof(value));
    (void)size;
}

static void *main()
{
    struct bunga_server_t server;
    uint8_t clients_input_buffers[2][256];
    uint8_t message[256];
    uint8_t workspace_in[256];
    uint8_t workspace_out[256];
    int put_fd;
    struct epoll_event event;
    int res;
    struct ml_uid_t *uid_p;
    void *message_p;
    uint64_t value;
    int i;

    printf("Starting a Bunga server on ':28000'.\n");

    epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) {
        printf("epoll_create1() failed.\n");
        return (NULL);
    }

    put_fd = eventfd(0, EFD_SEMAPHORE);

    if (put_fd == -1) {
        printf("eventfd() failed.\n");

        return (NULL);
    }

    event.events = EPOLLIN;
    event.data.fd = put_fd;

    res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, put_fd, &event);

    if (res == -1) {
        printf("epoll_ctl_add() failed.\n");

        return (NULL);
    }

    ml_queue_init(&queue, 32);
    ml_queue_set_on_put(&queue, (ml_queue_put_t)on_put_signal_event, &put_fd);

    res = bunga_server_init(&server,
                            "tcp://:28000",
                            &clients[0],
                            2,
                            &clients_input_buffers[0][0],
                            sizeof(clients_input_buffers[0]),
                            &message[0],
                            sizeof(message),
                            &workspace_in[0],
                            sizeof(workspace_in),
                            &workspace_out[0],
                            sizeof(workspace_out),
                            on_client_connected,
                            on_client_disconnected,
                            on_execute_command_req,
                            on_get_file_req,
                            on_put_file_req,
                            epoll_fd,
                            NULL);

    if (res != 0) {
        printf("Init failed.\n");

        return (NULL);
    }

    for (i = 0; i < 2; i++) {
        log_clients[i].log_fd = -1;
    }

    res = bunga_server_start(&server);

    if (res != 0) {
        printf("Start failed.\n");

        return (NULL);
    }

    printf("Server started.\n");

    while (true) {
        res = epoll_wait(epoll_fd, &event, 1, -1);

        if (res != 1) {
            break;
        }

        if (event.data.fd == put_fd) {
            res = read(put_fd, &value, sizeof(value));
            (void)res;
            uid_p = ml_queue_get(&queue, &message_p);

            if (uid_p == &uid_execute_command_complete) {
                handle_execute_command_complete(message_p);
            }
        } else {
            if (handle_log(&server, event.data.fd)) {
                continue;
            }

            bunga_server_process(&server, event.data.fd, event.events);
        }
    }

    return (NULL);
}

void bunga_server_linux_create(void)
{
    pthread_t pthread;

    pthread_create(&pthread, NULL, main, NULL);
}
