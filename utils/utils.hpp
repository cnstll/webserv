#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include "../core/Server.hpp"

class Server;

bool isSeverFd(int fd, std::map<int, Server> serverMap);
bool isInUpdatedFds(struct epoll_event *events, int fd, int countOfFdActualized);
std::string get_extension(std::string uri);
bool checkHangUpFlags(int event);
bool checkErrorFlags(int event);
bool isADir(std::string directoryPath);
std::string readFileIntoString(const std::string& path);
bool doesFileExist(const std::string &name);
void printErrorAndExit(const std::string &errorMessage);
size_t getLineFromPosition(const std::string &str, size_t pos);
std::string numberToString(size_t &Number);
std::string numberToString(int &Number);
int stringToNumber(std::string str);
std::vector<std::string> tokenizeValues(std::string &str);
bool strIsInVector(const std::string &str, const std::vector<std::string> v);