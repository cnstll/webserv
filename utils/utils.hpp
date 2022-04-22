#include "../core/Server.hpp"
#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <dirent.h>

class Server;
void log(std::string );
bool isSeverFd(int fd, std::map<int, Server> serverMap);
bool isInUpdatedFds(struct epoll_event *events, int fd, int countOfFdActualized);
int check(int return_value, std::string const &error_msg);
bool checkHangUpFlags(int event);
bool checkErrorFlags(int event);
int checkFatal(int return_value, const std::string &error_msg);

bool doesFileExist(const std::string &name);
bool isADir(std::string directoryPath);
std::string readFileIntoString(const std::string& path);
void printErrorAndExit(const std::string &errorMessage);
size_t getLineFromPosition(const std::string &str, size_t pos);
std::string numberToString(size_t &Number);
std::string numberToString(int &Number);
std::string numberToString(const int &Number);
int stringToNumber(std::string str);
std::vector<std::string> tokenizeValues(std::string &str);
bool strIsInVector(const std::string &str, const std::vector<std::string> v);
int strlen_list(char **strList);
