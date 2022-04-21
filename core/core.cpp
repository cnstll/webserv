#include "core.hpp"

void monitorSocketAction(int epoll_fd, int fdToMonitor, uint32_t eventsToMonitor, int action)
{
  struct epoll_event event_parameters;
  event_parameters.data.fd = fdToMonitor;
  event_parameters.events = eventsToMonitor;
  log("Accepting connection on fd: " + numberToString(fdToMonitor));
  check((epoll_ctl(epoll_fd, action, fdToMonitor, &event_parameters)), "epoll_ctl error");
}

Server *portPicker(std::map<int, Server> &serverMap, int fd)
{
  Server *currentServer = NULL;
  std::map<int, Server>::iterator iter = serverMap.begin();
  while (iter != serverMap.end())
  {
    if (iter->second.requestMap.find(fd) != iter->second.requestMap.end())
    {
      currentServer = &iter->second;
      return currentServer;
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
    monitorSocketAction(epollFd, tmpServerfd, EPOLLIN | EPOLLOUT, EPOLL_CTL_ADD);
    ++servIter;
  }
}

void closeInactiveConnections(struct epoll_event *events, std::map<int, Server> &serverMap, int actualizedFdCount)
{
  std::map<int, Server>::iterator iterServ = serverMap.begin();
  while (iterServ != serverMap.end())
  {
    std::map<int, Request *>::iterator iterReq = iterServ->second.requestMap.begin();
    while (iterReq != iterServ->second.requestMap.end())
    {
      if (!isInUpdatedFds(events, iterReq->first, actualizedFdCount))
      {
        if (iterReq->second->timeout())
        {
          Response resp(408, iterServ->second);
          resp.addBody();
          resp.sendResponse(iterReq->first);
          iterServ->second.closeConnection(iterReq->first);
          iterReq = iterServ->second.requestMap.begin();
          return;
        }
      }
      else
      {
        iterReq->second->_inactiveTime = 0;
      }
      ++iterReq;
    }
    ++iterServ;
  }
}

int main(int argc, char *argv[])
{
  int connexion_fd;
  int epoll_fd;
  int recv_bytes;
  int count_of_fd_actualized = 0;
  std::map<int, Server> serverMap;
  Request *currentRequest;
  Server *currentServer;

  // test if config file exist, otherwise exit
  if (argc != 2)
  {
    std::cerr << "ERROR: Wrong Number of arguments\n";
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
      if (checkErrorFlags(events[i].events) == false)
        continue;
      if (isSeverFd(events[i].data.fd, serverMap))
      {
        checkErrorFlags(events[i].events);
        if (!check((connexion_fd = serverMap[events[i].data.fd].acceptNewConnexion(events[i].data.fd)), "accept error"))
          continue;
        monitorSocketAction(epoll_fd, connexion_fd, EPOLLIN | EPOLLHUP | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET, EPOLL_CTL_ADD);
      }
      else if (events[i].events & EPOLLIN)
      {
        if (!checkErrorFlags(events[i].events))
        {
          currentServer->closeConnection(events[i].data.fd);
          continue;
        }
        currentServer = portPicker(serverMap, events[i].data.fd);
        currentRequest = currentServer->requestMap[events[i].data.fd];
        recv_bytes = currentServer->recvRequest(events[i].data.fd, *currentRequest);
        if (recv_bytes == -1)
          continue;
        if (recv_bytes == 0)
          break;
        currentRequest->parseBody();
        std::string requestedURI = currentRequest->getRequestedUri();
        if (events[i].events & EPOLLOUT)
          currentServer->respond(events[i].data.fd);
        if (!checkHangUpFlags(events[i].events))
              currentServer->closeConnection(events[i].data.fd);
      }
    }
    closeInactiveConnections(events, serverMap, count_of_fd_actualized);
  }
}
