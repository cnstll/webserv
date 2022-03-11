#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#define MAX_EVENTS 5
#define READ_SIZE 30000
#define PORT 8080
#define MAX_QUEUE 100

void makeSocketNonBlocking(int fd)
{
    int flags;
   
    flags = fcntl(fd, F_GETFL, NULL);

    if(-1 == flags)
    {
        printf("fcntl F_GETFL failed.%s", strerror(errno));
        exit(1);
    }

    flags |= O_NONBLOCK;

    if(-1 == fcntl(fd, F_SETFL, flags))   
    {
        printf("fcntl F_SETFL failed.%s", strerror(errno));
        exit(1);
    }       
}

int main()
{

char *hello = "HTTP/1.1 200 OK \nDate: Mon, 27 Jul 2009 12:28:53 GMT \nServer: Webserv B**** (He uses arcch btw.) \nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT \nContent-Length: 230 \nContent-Type: text/html; charset=iso-8859-1 \nConnection: Keep-Alive \n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\
<html>\
<head>\
   <title>400 Bad Request</title>\
</head>\
<body>\
   <h1>Bad Request</h1>\
   <p>Your browser sent a request that this server could not understand.</p>\
   <p>The request line contained invalid characters following the protocol string.</p>\
</body>\
</html>"
;
  int running = 1, event_count, i;
  size_t bytes_read;
  char read_buffer[READ_SIZE + 1];
  struct epoll_event server_event;
  struct epoll_event events[MAX_EVENTS];
  int epoll_fd;
  int server_fd, new_socket;
  long valread;
  struct sockaddr_in server_address;
  int addrlen = sizeof(server_address);


  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
  {
    perror("In socket");
    exit(EXIT_FAILURE);
  }
  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);
  //No Idea what it is, a phoque ?
 // memset(server_address.sin_zero, '\0', sizeof server_address.sin_zero);

  if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
  {
    perror("In bind");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, MAX_QUEUE) < 0)
  {
    perror("In listen");
    exit(EXIT_FAILURE);
  }
  epoll_fd = epoll_create(MAX_QUEUE); //Arg is ignored but must be > 0
  if(epoll_fd == -1)
  {
    fprintf(stderr, "Failed to create epoll file descriptor\n");
    return 1;
  }

 
  server_event.events = EPOLLIN;
  server_event.data.ptr = &server_fd; //why?
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &server_event))
  {
    fprintf(stderr, "Failed to add file descriptor to epoll\n");
    close(epoll_fd);
    return 1;
  }
  //printf("\n+++++++ Waiting for new connection ++++++++\n\n");

  while (running)
  {
    printf("\nPolling for input...\n");
    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if(event_count < 0)               
    {
      perror("In epoll_wait");
      exit(1);
    }
    printf("%d ready events\n", event_count);
    for(i = 0; i < event_count; i++)
    {
      if(events[i].data.ptr == &server_fd){
          //EPOLLHUP and EPOLLERR > exit
      }
      if ((new_socket = accept(server_fd, (struct sockaddr *)&server_address, (socklen_t *)&addrlen)) < 0)
      {
        perror("In accept");
        exit(EXIT_FAILURE);
      }
      printf("Reading file descriptor '%d' -- ", events[i].data.fd);
      bytes_read = read(events[0].data.fd, read_buffer, READ_SIZE);
      if (bytes_read <= 0)
        break;
      printf("%zd bytes read.\n", bytes_read);
      read_buffer[bytes_read] = '\0';
      printf("Read '%s'\n", read_buffer);
      memset(read_buffer, '\0', 30000);
      write(new_socket, hello, strlen(hello));
   }
  }
 
  if(close(epoll_fd))
  {
    fprintf(stderr, "Failed to close epoll file descriptor\n");
    return 1;
  }
  return 0;
}