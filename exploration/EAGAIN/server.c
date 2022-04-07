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
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>


#define BUFSIZE           8192
#define MAXSEND           512


int
main(int ac, char **av)
{
    char                 *bf;
    int                   max;
    int                   status;
    int                   port;
    int                   serverfd;
    int                   clientfd;
    int                   optval;
    int                   nonblock;
    unsigned int          addrsize;
    ssize_t               size;
    size_t                totalsent;
    struct sockaddr_in    server_addr;
    struct sockaddr_in    client_addr;


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
     * Open a socket for receiving connections. We are the server.
     */
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[error] socket(), [errno %d]\n", errno);
        exit(1);
    }

    status = 1;

    (void) memset((void *) &server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    addrsize = sizeof(struct sockaddr_in);

    if (bind(serverfd, (struct sockaddr *) &server_addr, addrsize) < 0) {
        printf("[error] bind(), [errno %d]\n", errno);
        goto cleanup;
    }


    /*
     * For this small example we only accept 1 connection. So we use a backlog
     * queue to fit only 1 entry...
     */
    if (listen(serverfd, 1) < 0) {
        printf("[error] listen(), [errno %d]\n", errno);
        goto cleanup;
    }


    /*
     * Check incoming connection. We are only expecting 1 here.
     * By default the socket is blocking so we can step into the 'accept()'
     * call and it will wait until a connection request comes in. In real
     * life a better approach would be to 'poll' the socket to check if
     * if we have any pending request.
     */
    addrsize = sizeof(struct sockaddr_in);

    clientfd = accept(serverfd, (struct sockaddr *) &client_addr, &addrsize);
    if (clientfd < 0) {
        printf("[error] accept(), [errno %d]\n", errno);
        goto cleanup;
    }

    optval = BUFSIZE * 2;
    if (setsockopt(clientfd, SOL_SOCKET, SO_SNDBUF,
                   (void *) &optval, sizeof(int))) {
        printf("[error] setsockopt(SO_SNDBUF), [errno %d]\n", errno);
        goto cleanup;
    }


    /*
     * Check if we should go for non blocking mode...)
     */
    if (nonblock) {
        if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0) {
            printf("[error] fcntl(O_NONBLOCK), [errno %d]\n", errno);
            goto cleanup;
        }
    }


    /*
     * We now have a connection. In real life we would either fork() or create
     * a new thread to handle the connection. Here, since we won't receive
     * any other connection we simply handle it in the main program.
     * The idea is to show what would generate a 'EWOULDBLOCK/EAGAIN' error
     * on a send() call. So what we do is write data to the client until we
     * hit the error.
     */
    totalsent = 0;
    max = 0;

    (void) memset((void *) bf, (int) ((max % 26) + (int) 'a'), BUFSIZE);

    while (max < MAXSEND) {
        printf("[info] send(%c%c%c%c...) size %ld [sent so far %ld]: ",
               bf[0], bf[1], bf[2], bf[3], BUFSIZE, totalsent);

        if ((size = send(clientfd, (void *) bf, BUFSIZE, 0)) != BUFSIZE) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                printf("%s\n", errno == EAGAIN ? "EAGAIN" : "EWOULDBLOCK");
                sleep(5);
            } else {
                printf("errno %d\n", errno);
                goto cleanup;
            }
        } else {
            printf("sent\n");
            max++;
            totalsent += BUFSIZE;
            (void) memset((void *) bf, (int) ((max % 26) + (int) 'a'), BUFSIZE);
        }
    }


    /*
     * Close our connection.
     */
    close(clientfd);

    status = 0;


cleanup:
    close(serverfd);

    exit(status);
}
