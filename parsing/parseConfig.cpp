#include "parsing.hpp"

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

static int countServerBlocs(const std::string &config){
  int counter = 0;
  size_t found = 0;
  size_t configLen = config.length();
  std::string serverToken = "server {";
  while (found < configLen){
    
    found = config.find(serverToken, found);
    if (found == std::string::npos){
      break;
    }
    found += serverToken.length() + 1;
    ++counter;
  }
  return counter;
};

static bool chunkHasToken(const std::string &chunk, const std::string &token){
  return (chunk.find(token) != std::string::npos);
}

static size_t findEndOfBloc(const std::string &config, size_t serverBlocStart){
  size_t nextClosingBrace = 0;
  size_t nextLocationBloc = 0;
  size_t configLen = config.length();
  size_t endOfBloc = 0;
  const std::string locationToken = "location /";
  const std::string serverToken = "server {";
  size_t checkStart = serverBlocStart;
  while (checkStart < configLen){
    nextClosingBrace = config.find("}", checkStart);
    if (nextClosingBrace == std::string::npos)
      printErrorAndExit("ERROR: no closing brace for server bloc");
    nextLocationBloc = config.substr(checkStart, nextClosingBrace - checkStart).find(locationToken);
    // if another server is within the chunk > error
    if (chunkHasToken(config.substr(checkStart, nextClosingBrace - checkStart), serverToken))
      printErrorAndExit("ERROR: server bloc enclosed in another server bloc\n");
    if (nextLocationBloc == std::string::npos){
      endOfBloc = nextClosingBrace;
      break;
    } else {
      nextLocationBloc += checkStart;
      //find if a location bloc is in the chunk
      if (chunkHasToken(config.substr(nextLocationBloc + locationToken.length(), nextClosingBrace - nextLocationBloc - locationToken.length()), locationToken))
        printErrorAndExit("ERROR: location bloc enclosed in another location bloc");
      checkStart = nextClosingBrace + 1;
    }
  }
  return endOfBloc;
}

void checkBlocsAndParse(const std::string &config, std::vector<Server> &servers){
  size_t startOfBloc = 0, endOfBloc = 0;
  const std::string serverToken = "server {";
  const std::string locationToken = "location /";
  int potentialNumberOfServers = countServerBlocs(config);
  int i = 0;
  while (potentialNumberOfServers > i){
    startOfBloc = config.find(serverToken, startOfBloc);
    if (chunkHasToken(config.substr(endOfBloc, startOfBloc - endOfBloc), locationToken))
      printErrorAndExit("ERROR: location bloc out of server bloc\n");
    //Find end of server bloc and do multiple checks
    endOfBloc = findEndOfBloc(config, startOfBloc + serverToken.length() + 1);
    servers.push_back(Server());
    servers[i].parseServerConfigFields(config.substr(startOfBloc, endOfBloc));
    std::cout << servers[i];
    startOfBloc = endOfBloc + 1;
    i++;
  }
  if (chunkHasToken(config.substr(startOfBloc, config.length() - startOfBloc), locationToken))
    printErrorAndExit("ERROR: location bloc out of server bloc\n");

}