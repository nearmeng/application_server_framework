#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <signal.h>
#include "workqueue.h"
#include "../proto/cs_pkg.pb-c.h"


#define NAME_SIZE 20
#define PWD_SIZE 20

/* Port to listen on. */
#define SERVER_PORT 8888
/* Connection backlog (# of backlogged connections to accept). */
#define CONNECTION_BACKLOG 8
/* Number of worker threads.  Should match number of CPU cores reported in 
 * /proc/cpuinfo. */
#define NUM_THREADS 4

/* Behaves similarly to fprintf(stderr, ...), but adds file, line, and function
 information. */
#define errorOut(...) {\
    fprintf(stderr, "%s:%d: %s():\t", __FILE__, __LINE__, __FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
}

/**
 * Struct to carry around connection (client)-specific data.
 */
typedef struct client {
    /* The client's socket. */
    int fd;

    /* The event_base for this client. */
//    struct event_base *evbase;

    /* The bufferedevent for this client. */
    struct bufferevent *buf_ev;

    /* The output buffer for this client. */
    struct evbuffer *output_buffer;

    /* Here you can add your own application-specific attributes which
     * are connection-specific. */
} client_t;



enum msg_type
{
	MSG_TYPE_INVALID = 0,
	LOGIN_MSG = 1,
	LOC_REPORT_MSG = 2,
	MSG_TYPE_MAX
};

enum login_msg_type
{
    LOGIN_MSG_TYPE_INVALID = 0,
    PRINT_THE_NAME = 1,
    PRINT_THE_PASSWORD = 2,
    LOGIN_MSG_TYPE_MAX
};

#endif
