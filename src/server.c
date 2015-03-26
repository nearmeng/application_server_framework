#include "server.h"
#include "asf_op.h"


static struct event_base *evbase_accept;
static workqueue_t workqueue;

/* Signal handler function (defined below). */
static void sighandler(int signal);

static void closeClient(client_t *client) {
    if (client != NULL) {
        if (client->fd >= 0) {
            close(client->fd);
            client->fd = -1;
        }
    }
}

static void closeAndFreeClient(client_t *client) {
    if (client != NULL) {
        closeClient(client);
        if (client->buf_ev != NULL) {
            bufferevent_free(client->buf_ev);
            client->buf_ev = NULL;
        }
        /*
        if (client->evbase != NULL) {
            event_base_free(client->evbase);
            client->evbase = NULL;
        }
        */
        if (client->response != NULL) {
            free(client->response);
            client->response = NULL;
        }
        free(client);
    }
}



/* work to do */
static void server_job_function(struct job *job) {
    client_t *client = (client_t *)job->user_data;

    char recv_data[4096];

    struct evbuffer* input;
    int data_len;
    int nbytes;

    int ret = 0;

/* echo 操作
    nbytes =  bufferevent_read(client->buf_ev, data, sizeof(data));
    if(nbytes <= 0)
    {
        break;
    }
    bufferevent_write(client->buf_ev, data, nbytes);
*/
    //1. get to know the length
    input = bufferevent_get_input(client->buff_ev);
    data_len = (int)evbuffer_get_length(input);

    //2. use the protobuf to unpack the msg
    nbytes = bufferevent_read(client->buf_ev, recv_data, data_len);
    if(nbytes <= 0)
    {
        printf("bufferevent_read %d bytes error!\n", data_len);
        return;
    }
    CsPkg *msg_in;
    msg_in = cs__pkg__unpack(NULL, data_len, (uint8_t*) &recv_data);

    //3. 对接收到的数据包进行相应的处理 
    if(msg_in != NULL)
    {
        switch(msg_in->head_pkg->msg_id)
        {
            case LOGIN_MSG:
                ret = deal_login_msg(msg_in, client);
                break;
            case LOC_REPORT_MSG:
                ret = deal_loc_report_msg(msg_in, client);
                break;
            default:
                printf("Error msg recved!\n");
                break;
            }
        }
        cs__pkg__free_unpacked(msg_in, NULL);

        if(ret != 0)
        {
            printf("deal msg error, msg_id[%d], client[%d]", msg_in->head_pkg->msg_id, client->fd);
            return;
        }

        bufferevent_write(client->buf_ev, client->response, client->response_len);
    }
    else
    {
        printf("msg decode error!\n");
        return;
    }

}



/**
 * Called by libevent when there is data to read.
 */
void buffered_on_read(struct bufferevent *bev, void *arg) {
    client_t *client = (client_t *)arg;
    job_t *job;

    
    /* Create a job object and add it to the work queue. */
    if ((job = malloc(sizeof(*job))) == NULL) {
        warn("failed to allocate memory for job state");
        closeAndFreeClient(client);
        return;
    }
    job->job_function = server_job_function;
    job->user_data = client;

    workqueue_add_job(&workqueue, job);

}

/**
 * Called by libevent when the write buffer reaches 0.  We only
 * provide this because libevent expects it, but we don't use it.
 */
void buffered_on_write(struct bufferevent *bev, void *arg) {
}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void buffered_on_error(struct bufferevent *bev, short what, void *arg) {
    closeAndFreeClient((client_t *)arg);
}


/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void on_accept(evutil_socket_t fd, short ev, void *arg) {
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    client_t *client;

    client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        warn("accept failed");
        return;
    }

    /* Set the client socket to non-blocking mode. */
    if (evutil_make_socket_nonblocking(client_fd) < 0) {
        warn("failed to set client socket to non-blocking");
        close(client_fd);
        return;
    }

    /* Create a client object. */
    if ((client = malloc(sizeof(*client))) == NULL) {
        warn("failed to allocate memory for client state");
        close(client_fd);
        return;
    }
    memset(client, 0, sizeof(*client));
    client->fd = client_fd;

    /* Add any custom code anywhere from here to the end of this function
     * to initialize your application-specific attributes in the client struct.
     */

    if ((client->output_buffer = evbuffer_new()) == NULL) {
        warn("client output buffer allocation failed");
        closeAndFreeClient(client);
        return;
    }

    /* Create the buffered event.
     *
     * The first argument is the file descriptor that will trigger
     * the events, in this case the clients socket.
     *
     * The second argument is the callback that will be called
     * when data has been read from the socket and is available to
     * the application.
     *
     * The third argument is a callback to a function that will be
     * called when the write buffer has reached a low watermark.
     * That usually means that when the write buffer is 0 length,
     * this callback will be called.  It must be defined, but you
     * don't actually have to do anything in this callback.
     *
     * The fourth argument is a callback that will be called when
     * there is a socket error.  This is where you will detect
     * that the client disconnected or other socket errors.
     *
     * The fifth and final argument is to store an argument in
     * that will be passed to the callbacks.  We store the client
     * object here.
     */
    client->buf_ev = bufferevent_socket_new(evbase_accept, client_fd,
                                            BEV_OPT_CLOSE_ON_FREE);
    if ((client->buf_ev) == NULL) {
        warn("client bufferevent creation failed");
        closeAndFreeClient(client);
        return;
    }
    bufferevent_setcb(client->buf_ev, buffered_on_read, buffered_on_write,
                      buffered_on_error, client);

    /* We have to enable it before our callbacks will be
     * called. */
    bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);

}

/**
 * Run the server.  This function blocks, only returning when the server has 
 * terminated.
 */
int runServer(void) {
    evutil_socket_t listenfd;
    struct sockaddr_in listen_addr;
    struct event *ev_accept;
    int reuseaddr_on;

    /* Set signal handlers */
    sigset_t sigset;
    sigemptyset(&sigset);
    struct sigaction siginfo = {
        .sa_handler = sighandler,
        .sa_mask = sigset,
        .sa_flags = SA_RESTART,
    };
    sigaction(SIGINT, &siginfo, NULL);
    sigaction(SIGTERM, &siginfo, NULL);

    /* Create our listening socket. */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        err(1, "listen failed");
    }

    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(SERVER_PORT);
    if (bind(listenfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) 
        < 0) {
        err(1, "bind failed");
    }
    if (listen(listenfd, CONNECTION_BACKLOG) < 0) {
        err(1, "listen failed");
    }
    reuseaddr_on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
               sizeof(reuseaddr_on));

    /* Set the socket to non-blocking, this is essential in event
     * based programming with libevent. */
    if (evutil_make_socket_nonblocking(listenfd) < 0) {
        err(1, "failed to set server socket to non-blocking");
    }

    if ((evbase_accept = event_base_new()) == NULL) {
        perror("Unable to create socket accept event base");
        close(listenfd);
        return 1;
    }

    /* Initialize work queue. */
    if (workqueue_init(&workqueue, NUM_THREADS)) {
        perror("Failed to create work queue");
        close(listenfd);
        workqueue_shutdown(&workqueue);
        return 1;
    }

    /* We now have a listening socket, we create a read event to
     * be notified when a client connects. */
    ev_accept = event_new(evbase_accept, listenfd, EV_READ|EV_PERSIST,
                          on_accept, (void *)&workqueue);
    event_add(ev_accept, NULL);

    printf("Server running.\n");

    /* Start the event loop. */
    event_base_dispatch(evbase_accept);

    event_base_free(evbase_accept);
    evbase_accept = NULL;

    close(listenfd);
    killServer();

    printf("Server shutdown.\n");

    return 0;
}

/**
 * Kill the server.  This function can be called from another thread to kill
 * the server, causing runServer() to return.
 */
void killServer(void) {
    fprintf(stdout, "Stopping socket listener event loop.\n");
    if (event_base_loopexit(evbase_accept, NULL)) {
        perror("Error shutting down server");
    }
    fprintf(stdout, "Stopping workers.\n");
    workqueue_shutdown(&workqueue);
}

static void sighandler(int signal) {
    fprintf(stdout, "Received signal %d: %s.  Shutting down.\n", signal,
            strsignal(signal));
    killServer();
}

/* Main function for demonstrating the echo server.
 * You can remove this and simply call runServer() from your application. */
int main(int argc, char *argv[]) {
    
    //daemon(0,0);
    return runServer();
}
