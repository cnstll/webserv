#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#define MAX_EVENTS 10
#define READ_SIZE 30000
#define SERVER_PORT 18001
#define MAX_QUEUE 100

void check (int return_value, char *error_msg){
  if (return_value < 0){
    perror(error_msg);
    exit(EXIT_FAILURE);
  }
}

int setup_server(int port, int backlog){
  int server_fd;
  int connexion_fd;
  int read_bytes;
  struct sockaddr_in server_addr;

  check((server_fd = socket(AF_INET, SOCK_STREAM, 0)), "socket error");

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

void monitor_socket(int epoll_fd, int fd_to_monitor, uint32_t events_to_monitor){
  //Set a epoll event object to parametrize monitored fds
  struct epoll_event event_parameters;
  //Fd that will be added to the epoll
  event_parameters.data.fd = fd_to_monitor;
  event_parameters.events = events_to_monitor;
  check((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_to_monitor, &event_parameters)), "epoll_ctl error");
}

int main(){

  int server_fd;
  int connexion_fd;
  int epoll_fd;
  uint8_t sent_line[] = "HTTP/1.0 200 OK \r\n\r\nSO BAAAAAD";
  uint8_t received_line[READ_SIZE + 1];
  // int epollin_value = EPOLLIN;
  // int epollout_value = EPOLLOUT;
  // int epollhup_value = EPOLLHUP;
  int read_bytes;
  int number_of_events;
  server_fd = setup_server(SERVER_PORT, MAX_QUEUE);

  //Prep a set of epoll event struct to register listened events
  struct epoll_event *events_received = calloc(MAX_EVENTS, sizeof(struct epoll_event));

  //Set up an epoll instance
  check((epoll_fd = epoll_create(1)), "epoll error");
  //Use epoll_ctl to add the server socket to epoll to monitor events from the server
  monitor_socket(epoll_fd, server_fd, EPOLLIN | EPOLLOUT);
  while (1){
    //number_of_events = 0;
    printf("waiting for a connection on port %d\n", SERVER_PORT);
    check((number_of_events = epoll_wait(epoll_fd, events_received, MAX_EVENTS, -1)), "epoll_wait error");
    for(int i = 0; i < number_of_events; i++){
      printf("idx: %d - fd: %d - event mask %d\n", i, events_received[i].data.fd, events_received[i].events);
      if (events_received[i].data.fd == server_fd){
        if (events_received[i].events & EPOLLHUP || events_received[i].events & EPOLLERR){
          close(server_fd);
          check(-1, "server error");
        }
        check((connexion_fd = accept_new_connexion(server_fd)), "accept error");
      }
      else {
        // if (events_received[i].events & EPOLLRDHUP){
        //   write(0, "EPPOLRDHUP\n", 11);
        //   close(events_received[i].data.fd);
        //   break;
        // }
        // if (events_received[i].events & EPOLLOUT) write(0, "EPPOLOUT\n", 9);
        // if (events_received[i].events & EPOLLERR){
        //   write(STDOUT_FILENO, "\nEPOLERR\n", 10);
        //   close(events_received[i].data.fd);
        // } 
        //if (events_received[i].events & EPOLLIN){
        //monitor_socket(epoll_fd, connexion_fd, EPOLLET | EPOLLRDHUP);
        //write(0, "EPPOLINN\n", 9);
        //}
        bzero(&received_line, READ_SIZE);
        while ((read_bytes = read(connexion_fd, received_line, READ_SIZE)) > 0){
          write(STDOUT_FILENO, received_line, read_bytes); 
          if (received_line[read_bytes - 1] == '\n'){
            break;
          }
          bzero(received_line, READ_SIZE);
        }
        check(read_bytes, "read error");
        if (read_bytes == 0){
          write(STDOUT_FILENO, "closing connexion..", strlen("closing connexion.."));
          close(events_received[i].data.fd);
        }
        write(connexion_fd, (char*)sent_line, strlen((char *)sent_line));
        write(connexion_fd, "\0", 1);
        }
      //if (events_received[i].events & EPOLLHUP) write(0, "EPPOLHUP\n", 9);
    }
  }
  close(server_fd);
}