#include "Server.hpp"
#include "utils.hpp"
#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

size_t countChar(const std::string &str, char c){
  size_t counter = 0;
  int i = 0;
  while (str[i])
    if (str[i++] == c)
      counter++;
  return counter;
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

bool checkLocationBloc(const std::string &config, size_t blocStart, size_t blocEnd){
  size_t startBrace = config.substr(blocStart, blocEnd - blocStart).find("{");
  // std::cerr << "SB: " << startBrace << " L: " << config.substr(blocStart, blocEnd - blocStart) << std::endl;
  //Location is not enclosed between {} or additionnal '{'
  if (startBrace == std::string::npos || startBrace != config.substr( blocStart, blocEnd - blocStart).rfind("{")) // additionnal "{" in a location bloc
    return false;
  else if (config.substr(blocStart, blocEnd - blocStart).find("}") != std::string::npos) //additionnal '}' in a location bloc
    return false;
  else
    return true;
}

size_t findEndOfBloc(const std::string &config, size_t serverBlocStart){
  size_t braceFound = 0;
  size_t hasLocation = 0;
  size_t endOfBloc = 0;
  const std::string locationToken = "location";
  size_t checkStart = serverBlocStart;
  while (1){
    braceFound = config.find("}", checkStart); // go to next closing brace
    hasLocation = config.substr(checkStart, braceFound - checkStart).find(locationToken);
    // std::cerr << "BEFORE BF: " << braceFound << " - CS: " << checkStart << " - HL: " << hasLocation << std::endl;
    // std::cerr << "BF: " << braceFound << " - CS: " << checkStart << " - HL: " << hasLocation << std::endl;
    if (hasLocation == std::string::npos){
      if (config.substr(serverBlocStart, braceFound - serverBlocStart).find("server") != std::string::npos)
        printErrorAndExit("ERROR: Wrong synthaxe for server bloc\n");
      endOfBloc = braceFound;
      break;
    }
    hasLocation += checkStart; // To make locationToken absolut
    if (checkLocationBloc(config, hasLocation, braceFound) == false) 
        printErrorAndExit("ERROR: Wrong synthaxe for location bloc\n");
    else
      checkStart = braceFound + 1;
  }
  return endOfBloc;
}

    // std::cout << "IN: " << config.substr(startOfBloc) << std::endl;
    // std::cout <<  "EOB: " << endOfBloc << " Line: " << getLineFromPosition(config, endOfBloc) << std::endl;
    // std::cout << "endOFBLOC: " << endOfBloc << std::endl;
    // std::cout << "StartOFBLOC: " << startOfBloc << std::endl;

void parseServerBloc(const std::string &config, std::vector<Server> servers, size_t countOfServers){
  size_t startOfBloc = 0;
  size_t endOfBloc;
  const std::string serverToken = "server {";
  size_t serverNum = 0;
  std::cerr << "ENTERED PARSE SERVER BLOC\n";
  while (serverNum < countOfServers){
    startOfBloc += config.substr(startOfBloc).find(serverToken);
    if (startOfBloc == std::string::npos)
      printErrorAndExit("ERROR: Wrong synthaxe for server bloc\n");
    endOfBloc = findEndOfBloc(config, startOfBloc + serverToken.length()); // search for end of server bloc after server token
    servers.at(serverNum).parseConfig(std::string(config, startOfBloc, endOfBloc - startOfBloc + 1));
    startOfBloc = endOfBloc + 1;
    std::cout << servers[serverNum];
    ++serverNum;
  }
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
  parseServerBloc(config, bunchOfServers, countOfServers);
}