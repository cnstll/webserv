#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include "core.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "../cgi/cgiHandler.hpp"
#include <sys/wait.h>
#include <iostream>
#define MAX_EVENTS 1000000
#define READ_SIZE 30000
#define REQUEST_READ_SIZE 8096
#define SERVER_PORT 18000
#define MAX_QUEUE 10000
std::string CGI_EXTENSION = ".py";

int server_fd;

int check (int return_value, std::string const &error_msg){
  if (return_value < 0){
    std::cerr << error_msg << std::endl;
    // exit(EXIT_FAILURE);
    return -1;
  }
  return 1;
}

int setup_server(int port, int backlog){
  int connexion_fd;
  struct sockaddr_in server_addr;

  check((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)), "socket error");
  int yes = 1;

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
  {
    perror("setsockopt");
    exit(1);
  }
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVER_PORT);
  check(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)), "bind error");
  check(listen(server_fd, MAX_QUEUE), "listen error");
  return server_fd;
}

int accept_new_connexion(int server_fd){
  socklen_t addr_in_len = sizeof(struct sockaddr_in);
  struct sockaddr_in connexion_address;
  int connexion_fd;

  if (check(connexion_fd = accept(server_fd, (struct sockaddr *)&connexion_address, &addr_in_len), "failed accept"))
  {
    return connexion_fd;
    std::cout << connexion_fd << std::endl;
  }
  return -1;
}

void monitor_socket_action(int epoll_fd, int fd_to_monitor, uint32_t events_to_monitor, int action){
  //Set a epoll event object to parametrize monitored fds
  struct epoll_event event_parameters;
  //Fd that will be added to the epoll
  event_parameters.data.fd = fd_to_monitor;
  event_parameters.events = events_to_monitor;
  //event_parameters.data.ptr = NULL;
  check((epoll_ctl(epoll_fd, action, fd_to_monitor, &event_parameters)), "epoll_ctl error");
}

void make_fd_non_blocking(int fd){
  int flags;
  check((flags = fcntl(fd, F_GETFL, NULL)), "flags error");
  flags |= O_NONBLOCK;
  check((fcntl(fd, F_SETFL, flags)), "fcntl error");
}

bool check_error_flags(int event){
  if (event & EPOLLHUP || event & EPOLLERR)
  {
    if (event & EPOLLHUP)
      write(0, "EPPOLHUP\n", 9);
    if (event & EPOLLERR)
      write(0, "EPPOLERR\n", 9);
    return (false);
  }
  return (true);
}

int is_request_done(Request *rq, int &contentLength, int &startOfBody)
{
  int lengthRecvd = rq->getFullRequest().length() - startOfBody;
  if (contentLength == lengthRecvd)
    return (1);
  return 0;
}

int parseHeader(Request *rq)
{
  int contentLength = 0;

  if (rq->getFullRequest().find("\r\n\r\n") != std::string::npos)
  {
    rq->parse();
    if (rq->getHttpMethod() == "POST")
      return (1);
    else
      return 2;
  }
  return 0;
}

int recv_request(const int &fd, Request *rq){
  int read_bytes;
  char request_buffer[REQUEST_READ_SIZE + 1];
  static bool headerParsed = 0;
  int parsed;
  int contentSize;
  int startOfBody;

  bzero(&request_buffer, REQUEST_READ_SIZE + 1);
  while ((read_bytes = recv(fd, &request_buffer, REQUEST_READ_SIZE, 0)) > 0)
  {
    rq->append(request_buffer, read_bytes);
    if (!headerParsed)
    {
      parsed = parseHeader(rq);
      if (parsed)
      {
        if (parsed == 2)
          return (read_bytes);
        else
        {
          headerParsed = 1;
          startOfBody = rq->getFullRequest().find("\r\n\r\n") + 4;
          contentSize = atoi(rq->getParsedRequest()["Content-Length"].c_str());
        }
      }
    }
    if (is_request_done(rq, contentSize, startOfBody))
    {
      headerParsed = 0;
      return (read_bytes);
    }
    bzero(&request_buffer, REQUEST_READ_SIZE);
  }
  return -1;
}

void print_events(struct epoll_event *events, int eventful_fds){
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

std::string get_extension(std::string uri)
{
  if (uri != "")
  {
    size_t pos = uri.find_last_of(".");
    if (pos != std::string::npos)
      return (uri.substr(pos));
  }
  return (uri);
}

int main(){

  int server_fd;
  int count_response = 0;
  int connexion_fd;
  int epoll_fd;
  int recv_bytes;
  int count_of_fd_actualized = 0;
  Request request;
  server_fd = setup_server(SERVER_PORT, MAX_QUEUE);

  // Prep a set of epoll event struct to register listened events
  struct epoll_event *events = (struct epoll_event *)calloc(MAX_EVENTS, sizeof(struct epoll_event));
  // char yes='1'; // Solaris people use this

  // Set up an epoll instance
  check((epoll_fd = epoll_create(1)), "epoll error");
  // Use epoll_ctl to add the server socket to epoll to monitor events from the server
  monitor_socket_action(epoll_fd, server_fd, EPOLLIN | EPOLLOUT, EPOLL_CTL_ADD);

  while (1)
  {
    check((count_of_fd_actualized = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)), "epoll_wait error");
    // print_events(events, count_of_fd_actualized);
    for (int i = 0; i < count_of_fd_actualized; i++)
    {
      recv_bytes = 0;
      if (events[i].data.fd == server_fd)
      {
        check_error_flags(events[i].events);
        if (!check((connexion_fd = accept_new_connexion(server_fd)), "accept error"))
          continue;
        make_fd_non_blocking(connexion_fd);
        monitor_socket_action(epoll_fd, connexion_fd, EPOLLIN | EPOLLHUP | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET, EPOLL_CTL_ADD);
      }
      else if (events[i].events & EPOLLIN)
      {
        if (check_error_flags(events[i].events) < 0){
          perror("error while receiving data");
          request.clear();
          break;
        }
        recv_bytes = recv_request(events[i].data.fd, &request);
        
        if (recv_bytes == -1)
        {
          continue;
        }
        if (recv_bytes == 0)
        {
          printf("Closing connexion for fd: %d\n", events[i].data.fd);
          request.clear();
          close(events[i].data.fd);
          break;
        }
        //! The parsing shouldn't need to take place again here
        if (request.parse() < 0){
          std::cout << "\nError while parsing request!!!\n";
        }
      if (events[i].events & EPOLLOUT) {
        if (get_extension(request.getRequestedUri()) == CGI_EXTENSION) {
            std::string script_pathname = "." + std::string(ROOT_DIR) + request.getRequestedUri();
            cgiHandler cgiParams(request.getParsedRequest(), script_pathname, events[i].data.fd);
            //! lots of exceptions left to throw
            try
            {
              cgiParams.handleCGI();
            }
            catch (const std::exception &e)
            {
              std::cerr << e.what() << '\n';
              Response resp(request.getParsedRequest(), 500);
              resp.addBody(request.getPathToFile());
              resp.sendResponse(events[i].data.fd);
            }
          }
          else if (request.getHttpMethod() == "DELETE"){
            const std::string fileToBeDeleted = "." + std::string(ROOT_DIR) + request.getRequestedUri();
            if (std::remove(fileToBeDeleted.c_str()) != 0){
              Response resp(request.getParsedRequest(), 204);
              resp.sendResponse(events[i].data.fd);
            }
          }
          else
          {
            Response resp(request.getParsedRequest(), request.getError());
            resp.addBody(request.getPathToFile());
            resp.sendResponse(events[i].data.fd);
          }
        }
        request.clear();
      }
    }
  }
  close(server_fd);
}