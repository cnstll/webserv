#ifndef CORE_HPP
#define CORE_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "../cgi/CgiHandler.hpp"
#include "../parsing/parsing.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#define MAX_EVENTS 1000000
#define READ_SIZE 30000
#define REQUEST_READ_SIZE 16000
#define MAX_QUEUE 10000
#define TIMEOUT 10000
const char *file_to_c_string(const std::string& path);
std::string readFileIntoString(const std::string& path);

#endif