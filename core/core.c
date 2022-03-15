#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
//#include <signal.h>
#define MAX_EVENTS 10
#define READ_SIZE 30000
#define SERVER_PORT 18000
#define MAX_QUEUE 100

int server_fd;
int count_of_fd_monitored = 0;

void check (int return_value, char *error_msg){
  if (return_value < 0){
    perror(error_msg);
    exit(EXIT_FAILURE);
  }
}

int setup_server(int port, int backlog){
  int connexion_fd;
  int read_bytes;
  struct sockaddr_in server_addr;

  check((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)), "socket error");

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVER_PORT);
  check(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)), "bind error");
  check(listen(server_fd, MAX_QUEUE), "listen error");
  return server_fd;
}

int accept_new_connexion(int server_fd){
  socklen_t addr_in_len = sizeof(struct sockaddr_in);
  struct sockaddr_in connexion_address;
  int connexion_fd;

  check(connexion_fd = accept(server_fd, (struct sockaddr *) &connexion_address, &addr_in_len), "failed accept");
  return connexion_fd;
}

void monitor_socket_operation(int epoll_fd, int fd_to_monitor, uint32_t events_to_monitor, int operation){
  //Set a epoll event object to parametrize monitored fds
  struct epoll_event event_parameters;
  //Fd that will be added to the epoll
  event_parameters.data.fd = fd_to_monitor;
  event_parameters.events = events_to_monitor;
  check((epoll_ctl(epoll_fd, operation, fd_to_monitor, &event_parameters)), "epoll_ctl error");
  if (operation == EPOLL_CTL_ADD)
    count_of_fd_monitored++;
}

void make_fd_non_blocking(int fd){
  int flags;
  check((flags = fcntl(fd, F_GETFL, NULL)), "flags error");
  flags |= O_NONBLOCK;
  check((fcntl(fd, F_SETFL, flags)), "fcntl error");
}

//void handle_sig(int sig, siginfo_t *info, void *ucontext){
//  (void)(info);
//  (void)(ucontext);
//  printf("SIG HANDLER...\n");
//  if (sig == SIGINT){
//    printf("Handling SIGINT...\n");
//    close(server_fd);
//    check(-1, "server error");
//  }
//}
//
//void set_signal_handling(void){
//  static struct sigaction sa;
//  sa.sa_sigaction = &handle_sig;
//	sa.sa_flags = SA_RESTART | SA_SIGINFO;
//    //Set up a signal listener
//  check(sigaction(SIGINT, &sa, NULL), "sigaction error");
//}

int main(){

  int server_fd;
  int connexion_fd;
  int epoll_fd;

  uint8_t sent_line[] = "HTTP/1.1 200 OK \nDate: Mon, 27 Jul 2009 12:28:53 GMT \nServer: Webserv B**** (He uses arcch btw.) \nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT \nContent-Length: 135 \nContent-Type: text/html; charset=iso-8859-1 \nConnection: Keep-Alive \n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\
<html>\
<head>\
   <title>You Are AWESOME WOW</title>\
</head>\
<body>\
   <h1>Fantastic</h1>\
   <p>VERY VERY FANTASTIC</p>\
   <p>INCREDIBLE</p>\
</body>\
</html>"
;

  uint8_t received_line[READ_SIZE + 1];
  int read_bytes;
  int count_of_fd_actualized = 0;
  server_fd = setup_server(SERVER_PORT, MAX_QUEUE);

  //Prep a set of epoll event struct to register listened events
  struct epoll_event *events_received = calloc(MAX_EVENTS, sizeof(struct epoll_event));

  //Set up an epoll instance
  check((epoll_fd = epoll_create(1)), "epoll error");
  //Use epoll_ctl to add the server socket to epoll to monitor events from the server
  monitor_socket_operation(epoll_fd, server_fd, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP, EPOLL_CTL_ADD);
  while (1){
    printf("\nwaiting for a connection on port %d\n", SERVER_PORT);
    printf("count of fd actualized %d\n", count_of_fd_actualized);
    printf("count of fd monitored %d\n", count_of_fd_monitored);
    check((count_of_fd_actualized = epoll_wait(epoll_fd, events_received, MAX_EVENTS, -1)), "epoll_wait error");
    for (int i = 0; i < count_of_fd_actualized; i++){
      printf("idx: %d - fd: %d - event mask %d\n", i, events_received[i].data.fd, events_received[i].events);
      if (events_received[i].data.fd == server_fd){
        if (events_received[i].events & EPOLLOUT) write(0, "EPPOLLOUT SERV\n", 15);
        if (events_received[i].events & EPOLLIN)  write(0, "EPPOLLIN SERV\n", 14);
        if (events_received[i].events & EPOLLHUP || events_received[i].events & EPOLLERR){
          close(server_fd);
          check(-1, "server error\n");
        }
        if (events_received[i].events & EPOLLRDHUP) write(0, "EPPOLLRDHUP SERV\n", 17);
        check((connexion_fd = accept_new_connexion(server_fd)), "\naccept error\n");
        make_fd_non_blocking(connexion_fd);
        monitor_socket_operation(epoll_fd, connexion_fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET, EPOLL_CTL_ADD);
        printf("Connexion accepted for fd: %d\n", connexion_fd);
      }
      else {
        if (events_received[i].events & EPOLLHUP || events_received[i].events & EPOLLERR){
          if (events_received[i].events & EPOLLHUP) write(0, "EPPOLHUP\n", 9);
          if (events_received[i].events & EPOLLERR) write(0, "EPPOLERR\n", 9);
          close(events_received[i].data.fd);
          break;
        }
        else if (events_received[i].events & EPOLLRDHUP){
          write(0, "EPPOLLRDHUP\n", 12);
          printf("CLOSING CONNECTION FOR FD: %d\n", events_received[i].data.fd);
          count_of_fd_monitored--;
          close(events_received[i].data.fd);
          break;
        }
        else if (events_received[i].events & EPOLLIN)
        {
          write(0, "EPPOLLIN\n", 9);
          //monitor_socket_operation(epoll_fd, events_received[i].data.fd, EPOLLOUT | EPOLLET | EPOLLRDHUP, EPOLL_CTL_MOD);
          bzero(&received_line, READ_SIZE);
          while ((read_bytes = read(events_received[i].data.fd, received_line, READ_SIZE)) > 0){
            write(STDOUT_FILENO, received_line, read_bytes);
            if (received_line[read_bytes - 1] == '\n')
              break;
            bzero(received_line, READ_SIZE);
          }
          check(read_bytes, "\nread error\n");
          if (events_received[i].events & EPOLLOUT){
            write(0, "EPPOLLOUT\n", 10);
            write(events_received[i].data.fd, (char*)sent_line, strlen((char *)sent_line));
            monitor_socket_operation(epoll_fd, events_received[i].data.fd, EPOLLIN | EPOLLET | EPOLLRDHUP, EPOLL_CTL_MOD);
          }
        }
        else
          ;//Do we have any other case to handle here ?
      }
    }
  }
  close(server_fd);
}
