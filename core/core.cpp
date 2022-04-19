#include "core.hpp"

std::string CGI_EXTENSION = ".py";

int server_fd;

int check(int return_value, std::string const &error_msg)
{
  if (return_value < 0)
  {
    std::cerr << error_msg << std::endl;
    return -1;
  }
  return 1;
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

Server *portPicker(std::map<int, Server> &serverMap, int fd)
{
  Server *currentServer;
  std::map<int, Server>::iterator iter = serverMap.begin();
  while (iter != serverMap.end())
  {
    if (iter->second.requestMap.find(fd) != iter->second.requestMap.end())
    {
      // currentRequest = iter->second.requestMap[events[i].data.fd];
      currentServer = &iter->second;
      break;
    }
    ++iter;
  }
  return currentServer;
}

void  setupServers(std::map<int, Server> &serverMap, std::vector<Server> bunchOfServers, int epollFd)
{
  std::vector<Server>::iterator servIter = bunchOfServers.begin();
  while (servIter != bunchOfServers.end())
  {
    int tmpServerfd = servIter->setupServer(servIter->getServerPort(), MAX_QUEUE);
    serverMap[tmpServerfd] = *servIter;
    monitor_socket_action(epollFd, tmpServerfd, EPOLLIN | EPOLLOUT, EPOLL_CTL_ADD);
    ++servIter;
  }
}
bool isInUpdatedFds(struct epoll_event *events, int fd, int countOfFdActualized)
{
  for (int i = 0; i < countOfFdActualized; i++)
  {
    if (events[i].data.fd == fd)
      return true;
  }
  return false;
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
  setupServers(serverMap, bunchOfServers, epoll_fd);

  while (1)
  {
    check((count_of_fd_actualized = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT)), "epoll_wait error");

    // print_events(events, count_of_fd_actualized);
    for (int i = 0; i < count_of_fd_actualized; i++)
    {
      if (check_error_flags(events[i].events) == false)
        continue;
      if (isSeverFd(events[i].data.fd, serverMap))
      {
        check_error_flags(events[i].events);
        if (!check((connexion_fd = serverMap[events[i].data.fd].acceptNewConnexion(events[i].data.fd)), "accept error"))
          continue;
        monitor_socket_action(epoll_fd, connexion_fd, EPOLLIN | EPOLLHUP | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET, EPOLL_CTL_ADD);
      }
      else if (events[i].events & EPOLLIN)
      {
        currentServer = portPicker(serverMap, events[i].data.fd);
        currentRequest = currentServer->requestMap[events[i].data.fd];
        recv_bytes = currentServer->recvRequest(events[i].data.fd, *currentRequest); // recv_request(events[i].data.fd, currentRequest, *currentServer);
        if (recv_bytes == -1)
          continue;
        if (recv_bytes == 0)
          break;
        currentRequest->parseBody();
        std::string requestedURI = currentRequest->getRequestedUri();
        if (events[i].events & EPOLLOUT)
          currentServer->respond(events[i].data.fd);
      }
    }
    // closeInnactiveConnections
      std::map<int, Server>::iterator iterServ = serverMap.begin();
      while (iterServ != serverMap.end())
      {
        std::map<int,   Request*>::iterator iterReq = iterServ->second.requestMap.begin();
        while (iterReq != iterServ->second.requestMap.end())
        {
          // std::cout << iterReq->first << std::endl;
          if (!isInUpdatedFds(events, iterReq->first, count_of_fd_actualized))
          {
            if (iterReq->second->timeout())
            {
              std::cout << iterReq->first << std::endl;
              Response resp(iterReq->second->getParsedRequest(), 408, *currentServer);
              resp.addBody();
              resp.sendResponse(iterReq->first);
              if (close(iterReq->first) == -1)
                perror("EEEERNO:");
              iterServ->second.requestMap.erase(iterReq->first);
              iterReq = iterServ->second.requestMap.begin();
              std::cout << "connection timed out" << std::endl;
              break ;
            }
          }
          else{
            iterReq->second->_inactiveTime = 0;
            // std::cout << iterReq->second->timeout() << std::endl;
          }
          ++iterReq;
        }
        ++iterServ;
      }
 
    //! Gotta check whatever request that exists has had events happen and otherwise timeouts, we should be easily able to check for, or actually how do we iterate over a map?
    // Iterate through All open requests, check that against actualized fd's, if one isn't thereclose the connection and delete the request?
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