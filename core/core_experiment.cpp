#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include "core.hpp"
#include "Response.hpp"
#include <iostream>
#define MAX_EVENTS 10000
#define READ_SIZE 30000
#define SERVER_PORT 18000
#define MAX_QUEUE 10000

int server_fd;

void check (int return_value, std::string const &error_msg){
  if (return_value < 0){
    std::cerr << error_msg << std::endl;
    exit(EXIT_FAILURE);
  }
}

int setup_server(int port, int backlog){
  int connexion_fd;
  int read_bytes;
  struct sockaddr_in server_addr;

  check((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)), "socket error");
int yes=1;

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
  {
    perror("setsockopt");
    exit(1);
  }
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

void monitor_socket_action(int epoll_fd, int fd_to_monitor, uint32_t events_to_monitor, int action){
  //Set a epoll event object to parametrize monitored fds
  struct epoll_event event_parameters;
  //Fd that will be added to the epoll
  event_parameters.data.fd = fd_to_monitor;
  event_parameters.events = events_to_monitor;
  check((epoll_ctl(epoll_fd, action, fd_to_monitor, &event_parameters)), "epoll_ctl error");
}

void make_fd_non_blocking(int fd){
  int flags;
  check((flags = fcntl(fd, F_GETFL, NULL)), "flags error");
  flags |= O_NONBLOCK;
  check((fcntl(fd, F_SETFL, flags)), "fcntl error");
}
void check_error_flags(int event)
{
  if (event & EPOLLHUP || event & EPOLLERR)
  {
    if (event & EPOLLHUP)
      write(0, "EPPOLHUP\n", 9);
    if (event & EPOLLERR)
      write(0, "EPPOLERR\n", 9);
    exit(1);
  }
}

int read_all(int fd)
{
  uint8_t received_line[READ_SIZE + 1];
  int read_bytes;

  bzero(&received_line, READ_SIZE);
  while ((read_bytes = recv(fd, received_line, READ_SIZE, MSG_DONTWAIT)) > 0)
  {
    if (received_line[read_bytes - 1] == '\n')
    {
      break;
    }
  }
  return read_bytes;
}

void print_events(struct epoll_event *events, int eventful_fds)
{
  for (int i = 0; i < eventful_fds; i++)
  {
    printf("fd:    %i\n", events[i].data.fd);
    if (events[i].events & EPOLLIN)
      printf("EPOLLIN\n");
    if (events[i].events & EPOLLOUT)
      printf("EPOLLOUT\n");
    if (events[i].events & EPOLLHUP)
      printf("EPOLLHUP\n");
    if (events[i].events & EPOLLERR)
      printf("EPOLLERR\n");
    if (events[i].events & EPOLLRDHUP)
      printf("EPOLLRDHUP\n");
    if (events[i].events & EPOLL_CLOEXEC)
      printf("EPOLlCLOEXEC\n");    
    if (events[i].events & EPOLLMSG)
      printf("EPOLlMSG\n");    
    if (events[i].events & EPOLLPRI)
      printf("EPOLlPRI\n");    
    if (events[i].events & EPOLLWAKEUP)
      printf("EPOLLWAKEUP\n");    
    //printf("something else happened\n");
    printf("\n\n");
  }
}

int main(){

  int server_fd;
  int count_response = 0;
  int connexion_fd;
  int epoll_fd;
  uint8_t received_line[READ_SIZE + 1];
  int read_bytes;
  int count_of_fd_actualized = 0;
  server_fd = setup_server(SERVER_PORT, MAX_QUEUE);

  //Prep a set of epoll event struct to register listened events
  struct epoll_event *events = (struct epoll_event *)calloc(MAX_EVENTS, sizeof(struct epoll_event));
//char yes='1'; // Solaris people use this

// Set up an epoll instance
check((epoll_fd = epoll_create(1)), "epoll error");
// Use epoll_ctl to add the server socket to epoll to monitor events from the server
monitor_socket_action(epoll_fd, server_fd, EPOLLIN | EPOLLOUT, EPOLL_CTL_ADD);

while (1)
{
  check((count_of_fd_actualized = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)), "epoll_wait error");
  print_events(events, count_of_fd_actualized);
  for (int i = 0; i < count_of_fd_actualized; i++)
  {
    if (events[i].data.fd == server_fd)
    {
      check_error_flags(events[i].events);
      check((connexion_fd = accept_new_connexion(server_fd)), "accept error");
      make_fd_non_blocking(connexion_fd);
      monitor_socket_action(epoll_fd, connexion_fd, EPOLLIN | EPOLLHUP | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET, EPOLL_CTL_ADD);
      printf("Connexion accepted for fd: %d\n", connexion_fd);
    }
    else if (events[i].events & EPOLLIN)
    {
      // check_error_flags(events[i].events); 
      if (events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
      {
        if (events[i].events & EPOLLHUP)
          write(0, "EPPOLHUP\n", 9);
        if (events[i].events & EPOLLERR)
          write(0, "EPPOLERR\n", 9);
        break ;
      }
      // ! ATTENTION REFACTO CHELOUX ICI, DESORDRE DES CHOSES >> A REFACTO
      // TODO: handle root path, fix tester, learn to read, make ragu bolognese
      
      check(read_bytes = read_all(events[i].data.fd), "read error");
      if (read_bytes == 0)
      {
        printf("Closing connexion for fd: %d\n", events[i].data.fd);
        close(events[i].data.fd);
        break;
      }
      if (events[i].events & EPOLLOUT){
        Response resp(200);
        resp.addBody("/mnt/nfs/homes/jescully/Documents/webserv/core/testfile.html");
        printf("Sending response to fd:  %d\n", events[i].data.fd);
        printf("count of response:  %d\n", ++count_response);
        resp.sendResponse(events[i].data.fd);
      }
    }
  }
}
  close(server_fd);
}
