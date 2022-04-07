#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>


#define BUFSIZE           8192
#define MAXRECV           512


int
main(int ac, char **av)
{
    char                 *bf;
    int                   max;
    int                   status;
    int                   cc;
    int                   port;
    int                   connfd;
    int                   clientfd;
    int                   optval;
    int                   nonblock;
    unsigned int          addrsize;
    ssize_t               size;
    size_t                totalrecv;
    struct hostent       *server;
    struct sockaddr_in    server_addr;


    /*
     * Check arguments. Collect the user supplied port number to use.
     */
    if (ac < 3) {
        printf("[error] Usage: %s <portnumber> <block/nonblock (0/1)>\n", av[0]);
        exit(1);
    }

    port = atoi(av[1]);
    nonblock = atoi(av[2]);


    /*
     * Allocate a working buffer.
     */
    if ((bf = (char *) malloc(BUFSIZE)) == 0) {
        printf("[error] malloc(bf), [errno %d]\n", errno);
        exit(1);
    }


    /*
     * For this example we will only work on the local server.
     */
    if (gethostname(bf, BUFSIZE)) {
        printf("[error] gethostname(), [errno %d]\n", errno);
        exit(1);
    }


    /*
     * Open a socket for connecting to the server. We restrict the size
     * of buffers so that we can easily demonstrate our test.
     */
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[error] socket(), [errno %d]\n", errno);
        exit(1);
    }

    status = 1;

    optval = BUFSIZE * 2;
    if (setsockopt(connfd, SOL_SOCKET, SO_RCVBUF,
                   (void *) &optval, sizeof(int))) {
        printf("[error] setsockopt(SO_RCVBUF), [errno %d]\n", errno);
        goto cleanup;
    }


    /*
     * Check if we should go for non blocking mode...)
     */
    if (nonblock) {
        if (fcntl(connfd, F_SETFL, O_NONBLOCK) < 0) {
            printf("[error] fcntl(O_NONBLOCK), [errno %d]\n", errno);
            goto cleanup;
        }
    }

 

    /*
     * Collect our server address for the 'connect()' call.
     */
    if ((server = gethostbyname(bf)) == 0) {
        printf("[error] gethostbyname(), [errno %d]\n", errno);
        goto cleanup;
    }


    /*
     * Connect to the server.
     */
    size = server->h_length;
    (void) memset((void *) &server_addr, 0, sizeof(struct sockaddr_in));
    memcpy((void *) &server_addr.sin_addr.s_addr, (void *) server->h_addr, size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    addrsize = sizeof(struct sockaddr_in);

    /*
     * The while loop is only for Linux where sometimes the connect
     * would fail with EINPROGRESS. We just wait and retry.
     */
    while (connect(connfd, (struct sockaddr *) &server_addr, addrsize) < 0) {
        if (errno == EINPROGRESS) {
            sleep(1);
            continue;
        }
        printf("[error] connect(), [errno %d]\n", errno);
        goto cleanup;
    }


    /*
     * We now have a connection. For this small example what we need to
     * do is 'recv()' only when the user does request it by pressing return.
     */
    totalrecv = 0;
    max = 0;
    do {
        size = 0;
        (void) memset((void *) bf, 0, BUFSIZE);
        printf("[info] press return to receive data from server: ");
        cc = getc(stdin);

        while ((size = recv(connfd, (void *) bf, BUFSIZE, 0)) < BUFSIZE) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                printf("[warning] recv(), [errno %s]\n",
                       errno == EAGAIN ? "EAGAIN" : "EWOULDBLOCK");
                sleep(5);
            } else {
                printf("[error] recv(), [errno %d]\n", errno);
                break;
            }
        }

        max++;
        totalrecv += BUFSIZE;
        printf("[info] received %ld bytes [so far %ld] [%c%c%c%c]\n",
               BUFSIZE, totalrecv, bf[0], bf[1], bf[2], bf[3]);
    } while (max < MAXRECV);


    /*
     * Close our connection.
     */
    close(connfd);

    status = 0;


cleanup:
    close(connfd);

    exit(status);
}
