#include "core.hpp"

std::string CGI_EXTENSION = ".py";

int server_fd;

int check(int return_value, std::string const &error_msg)
{
  if (return_value < 0)
  {
    std::cerr << error_msg << std::endl;
    // exit(EXIT_FAILURE);
    return -1;
  }
  return 1;
}

int setup_server(int port, int backlog)
{
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
  server_addr.sin_port = htons(port);
  check(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)), "bind error");
  check(listen(server_fd, backlog), "listen error");
  return server_fd;
}

int accept_new_connexion(int server_fd)
{
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

void monitor_socket_action(int epoll_fd, int fd_to_monitor, uint32_t events_to_monitor, int action)
{
  // Set a epoll event object to parametrize monitored fds
  struct epoll_event event_parameters;
  // Fd that will be added to the epoll
  event_parameters.data.fd = fd_to_monitor;
  event_parameters.events = events_to_monitor;
  // event_parameters.data.ptr = NULL;
  check((epoll_ctl(epoll_fd, action, fd_to_monitor, &event_parameters)), "epoll_ctl error");
}

void make_fd_non_blocking(int fd)
{
  int flags;
  check((flags = fcntl(fd, F_GETFL, NULL)), "flags error");
  flags |= O_NONBLOCK;
  check((fcntl(fd, F_SETFL, flags)), "fcntl error");
}

bool check_error_flags(int event)
{
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

bool isSeverFd(int fd, std::map<int, Server> serverMap)
{
  std::map<int, Server>::iterator iter = serverMap.begin();
  while (iter != serverMap.end())
  {
    if (iter->first == fd)
      return 1;
    ++iter;
  }
  return 0;
}

int main(int argc, char *argv[])
{
  int connexion_fd;
  int epoll_fd;
  int recv_bytes;
  int count_of_fd_actualized = 0;
  std::map<int, Request *> requestMap;
  std::map<int, Server> serverMap;
  Request *currentRequest;
  Server *currentServer;

  // test if config file exist, otherwise exit
  if (argc != 2)
  {
    std::cerr << "Wrong Number of arguments\n";
    exit(EXIT_FAILURE);
  }
  std::string config = configToString(argv[1]);
  preParsing(config);
  std::vector<Server> bunchOfServers;
  checkBlocsAndParse(config, bunchOfServers);
  // Prep a set of epoll event struct to register listened events
  struct epoll_event *events = (struct epoll_event *)calloc(MAX_EVENTS, sizeof(struct epoll_event));
  // Set up an epoll instance
  check((epoll_fd = epoll_create(1)), "epoll error");

  std::vector<Server>::iterator servIter = bunchOfServers.begin();
  while (servIter != bunchOfServers.end())
  {
    int tmpServerfd = setup_server(servIter->getServerPort(), MAX_QUEUE);
    serverMap[tmpServerfd] = *servIter;
    monitor_socket_action(epoll_fd, tmpServerfd, EPOLLIN | EPOLLOUT, EPOLL_CTL_ADD);
    ++servIter;
  }

  while (1)
  {
    check((count_of_fd_actualized = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT)), "epoll_wait error");
    for (int i = 0; i < count_of_fd_actualized; i++)
    {
      recv_bytes = 0;
      if (isSeverFd(events[i].data.fd, serverMap))
      {
        currentServer = &serverMap[events[i].data.fd];
        check_error_flags(events[i].events);
        if (!check((connexion_fd = currentServer->acceptNewConnexion(events[i].data.fd)), "accept error"))
          continue;
        monitor_socket_action(epoll_fd, connexion_fd, EPOLLIN | EPOLLHUP | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET, EPOLL_CTL_ADD);
      }
      else if (events[i].events & EPOLLIN)
      {
        std::map<int, Server>::iterator iter = serverMap.begin();
        while (iter != serverMap.end())
        {
          if (iter->second.requestMap.find(events[i].data.fd) != iter->second.requestMap.end())
          {
            currentRequest = iter->second.requestMap[events[i].data.fd];
            currentServer = &iter->second;
            break;
          }
          ++iter;
        }
        if (check_error_flags(events[i].events) == false)
        {
          perror("error while receiving data");
          // delete currentRequest;
          requestMap.erase(events[i].data.fd);
          break;
        }
        recv_bytes = currentServer->recvRequest(events[i].data.fd, *currentRequest); // recv_request(events[i].data.fd, currentRequest, *currentServer);
        if (recv_bytes == -1)
          continue;
        if (recv_bytes == 0)
          break;
        if (currentRequest->parse(*currentServer) < 0)
        {
          std::cout << "\nError while parsing currentRequest!!!\n";
        }
        std::string requestedURI = currentRequest->getRequestedUri();
        if (events[i].events & EPOLLOUT)
        {
          if (get_extension(currentRequest->getRequestedUri()) == CGI_EXTENSION)
          {
            std::string scriptPathname = currentServer->constructPath(requestedURI);
            cgiHandler cgiParams(currentRequest->getParsedRequest(), scriptPathname, events[i].data.fd);
            cgiParams.handleCGI(events[i].data.fd);
          }
          else if (currentRequest->getHttpMethod() == "DELETE")
          {
            const std::string fileToBeDeleted = "." + std::string(ROOT_DIR) + currentRequest->getRequestedUri();
            if (std::remove(fileToBeDeleted.c_str()) != 0)
            {
              Response resp(currentRequest->getParsedRequest(), 204);
              resp.sendResponse(events[i].data.fd);
            }
          }
          else
          {
            Response resp(currentRequest->getParsedRequest(), currentRequest->getError());
            resp.addBody(currentServer->constructPath(requestedURI));
            resp.sendResponse(events[i].data.fd);
            // delete currentRequest;
            // requestMap.erase(events[i].data.fd);
          }
          currentRequest->clear();
        }
        if (currentRequest->getParsedRequest()["Connection"] != "keep-alive")
        {
          close(events[i].data.fd);
          delete currentRequest;
          currentServer->requestMap.erase(events[i].data.fd);
        }
      }
    }
    //! Gotta check whatever request that exists has had events happen and otherwise timeouts, we should be easily able to check for, or actually how do we iterate over a map?
    // if (request2->getFullRequest() != "" && !count_of_fd_actualized)
    // {
    //   Response resp(request.getParsedRequest(), 408);
    //   resp.addBody(request.getPathToFile());
    //   resp.sendResponse(request.fd);
    //   request.clear();
    //   close(request.fd);
    // }
  }
  close(server_fd);
}