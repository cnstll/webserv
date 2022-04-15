#ifndef PARSING_HPP
# define PARSING_HPP
# include "../core/Server.hpp"
# include "../utils/utils.hpp"
# include <map>
# include <vector>
# include <iterator>
# include <iostream>
# include <cstdio>
# include <cstring>
# include <fstream>
# include <string>

class Server;

std::string configToString(char *filepath);
void checkBlocsAndParse(const std::string &config, std::vector<Server> &servers);
void preParsing(const std::string& config);
#endif