#include "Server.hpp"
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

void printErrorAndExit(const std::string &errorMessage){
    std::cerr << errorMessage;
    exit(EXIT_FAILURE);
}

size_t countChar(const std::string &str, char c){
  size_t counter = 0;
  int i = 0;
  while (str[i])
    if (str[i++] == c)
      counter++;
  return counter;
}

size_t getLineFromPosition(const std::string &str, size_t pos){
  size_t lineNum = 0, tail = 0, head = 0;

  while (tail != std::string::npos){
    tail = str.find("\n", head);
    ++lineNum;
    if (pos < tail)
      break;
    head = tail + 1;
  }
  return lineNum;
}

std::string configToString(char *filepath){

  std::ifstream configFile;
  std::string line;
  std::string config;
  std::multimap<std::string, std::map<std::string, std::string> > parsedConfigHolder; 
  configFile.open(filepath, std::ifstream::in);
  if (!configFile.is_open())
    printErrorAndExit("Configuration file could not be found\n");
  while (getline(configFile, line))
  {
      config += line;
      config += "\n";
  }
  configFile.close();
  return config;
}

// void checkMinimalConfigRequirements(const std::string &config){
//   size_t found = 0;
//   // Verify that at least 1 http block exists and is unique
//   found = config.find("http {");
//   if (found == std::string::npos || found != config.rfind("http {"))
//     printErrorAndExit("ERROR: configuration does not meet minimal requirements\n");
//   // Verify that at least 1 server block exists
//   found = config.find("server {");
//   if (found == std::string::npos)
//     printErrorAndExit("ERROR: configuration does not meet minimal requirements\n");
// }

// void checkConfigurationSynthax(const std::string &config){
//   size_t head = 0, tail = 0;
//   size_t counter = 0;
//   char buf[20];

//   //Verify that open braces get closing one
//   counter = countChar(config, '{');
//   if (!counter || counter != countChar(config, '}'))
//     printErrorAndExit("ERROR: Bad configuration synthax\n");
//   //That each line is either empty, begins with spaces and hashtag or are tokens separated with space and ending with ';' or are block opening/closing
//   while (1){
//     tail = config.find("\n", head);
//     if (tail == std::string::npos)
//       break;
//     if (tail - head == 0 || config.find_first_of("{};", tail - 1, 1) != std::string::npos)
//       head = tail + 1;
//     else {
//       std::memset(buf, 0, 20);
//       sprintf(buf, "%zu", getLineFromPosition(config, tail - 1));
//       printErrorAndExit("ERROR: Bad configuration synthax at line " + std::string(buf) + " with char \'" + config.at(tail - 1) + "\'\n");
//     }
//     head = tail + 1;
//   }
// }
// std::map<std::string, std::string> parseHttpBloc(const std::string &config){
//   std::map<std::string, std::string> httpConfig;
//   std::string validHttpFields[] = {"client_max_body_size", "index", ""};
//   int i = 0;
// 	//fill map with all possible allowed fields in http bloc
// 	while (validHttpFields[i] != ""){
// 		httpConfig[validHttpFields[i]] = std::string();
//     std::cout << "Field: " << validHttpFields[i] << std::endl;
// 		i++;
// 	}
//   std::map<std::string, std::string>::iterator it;

//   size_t startOfLine = 0, endOfLine = 0, endOfHttpBloc = 0;
//   size_t head = 0; //tail = 0;
//   const std::string httpBlocInit = "http {";
//   startOfLine = config.find(httpBlocInit, 0);
//   startOfLine += httpBlocInit.length() + 1;
//   endOfHttpBloc = config.find("}", startOfLine);
//   endOfLine = config.find("\n", startOfLine);
//   //std::cout << "FOUND: \'" << config[head] << "\'"<< std::endl;
//   while ( startOfLine < endOfHttpBloc){
//     std::cout << "STARTLINE: " << startOfLine << " ENDOFLINE: " << endOfLine << std::endl;
//     it = httpConfig.begin();
//     while ((!head || head == std::string::npos) && it != httpConfig.end()){
//       head = config.find(it->first, startOfLine);
//       ++it;
//     }
//     if (it == httpConfig.end())
//       printErrorAndExit("ERROR: Unknown configuration parameter\n");
//     else if (it->second != "")
//       printErrorAndExit("ERROR: Repeating configuration parameter\n");
//     else
//       it->second = std::string(config, it->first.length());
//     std::cout << "PARAMETER FOUND: " << it->first << " VALUE: " << it->second << std::endl;
//     startOfLine = endOfLine + 1;
//     endOfLine = config.find("\n", startOfLine);
//   }
//   return httpConfig;
// }
size_t countVirtualServers(const std::string &config){
  size_t counter = 0;
  size_t found = 0;
  size_t configLen = config.length();
  std::string serverToken = "server {";
  while (found < configLen){
    found = config.find(serverToken, found);
    if (found == std::string::npos)
      break;
    found += serverToken.length() + 1;
    ++counter;
  }
  return counter;
};
size_t findEndOfBloc(const std::string &config, size_t serverBlocStart){
  size_t braceFound = 0;
  size_t hasLocation = 0;
  size_t endOfBloc = 0;
  braceFound = config.find("}", serverBlocStart); // What if no closing brace?
  while (1){
    hasLocation = config.find("location", serverBlocStart, braceFound - serverBlocStart);
    if (hasLocation == std::string::npos){
      endOfBloc = braceFound;
      break;
    }
    else
    {
      if (config.find("{", hasLocation, braceFound) == std::string::npos) //Location is not enclosed between {}
        printErrorAndExit("ERROR: Wrong synthaxe for location bloc\n");
      serverBlocStart = braceFound + 1;
      braceFound = config.find("}", braceFound + 1); // go to next closing brace
    }
  }
  return endOfBloc;
}

void parseServerBloc(const std::string &config, std::vector<Server> *Servers){
  size_t endOfBloc = findEndOfBloc(config, 1);
  std::cout << "EOB: " << endOfBloc << " Line: " << getLineFromPosition(config, endOfBloc);
}

int main (int argc, char *argv[]){
  // test if config file exist, otherwise exit
  if (argc != 2){
    std::cerr << "Wrong Number of arguments\n";
    exit(EXIT_FAILURE);
  }
  std::string config = configToString(argv[1]);
  std::string parsedConfig;
  size_t countOfServers;
  //Server server;
  //server.displayServerConfig();
  //std::size_t head, tail;
  //checkMinimalConfigRequirements(config);
  //checkConfigurationSynthax(config);
  countOfServers = countVirtualServers(config);
  std::cout << "NB OF SERVER: " << countOfServers << std::endl;
  std::vector<Server> bunchOfServers;
  bunchOfServers.assign(countOfServers, Server());
  parseServerBloc(config, &bunchOfServers);
}