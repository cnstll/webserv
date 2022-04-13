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

size_t countServerBlocs(const std::string &config){
  size_t counter = 0;
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

// void parseServerBloc(const std::string &config, std::vector<Server> servers, size_t countOfServers){
//   size_t startOfBloc = 0;
//   size_t endOfBloc;
//   size_t endOfLine;
//   const std::string serverToken = "server {";
//   size_t serverNum = 0;
//   // std::cerr << "ENTERED PARSE SERVER BLOC\n";
//   while (serverNum < countOfServers){
//     startOfBloc = config.find(serverToken, startOfBloc);
//     if (startOfBloc == std::string::npos)
//       printErrorAndExit("ERROR: Wrong synthaxe for server bloc\n");
//     endOfBloc = findEndOfBloc(config, startOfBloc + serverToken.length()); // search for end of server bloc after server token
//     startOfBloc = endOfBloc + 1;
//     ++serverNum;
//   }
// }

bool chunkHasToken(const std::string &chunk, const std::string &token){
  return (chunk.find(token) != std::string::npos);
}

size_t findEndOfBloc(const std::string &config, size_t serverBlocStart){
  size_t nextClosingBrace = 0;
  size_t nextLocationBloc = 0;
  size_t configLen = config.length();
  size_t endOfBloc = 0;
  const std::string locationToken = "location /";
  const std::string serverToken = "server {";
  size_t checkStart = serverBlocStart;
  while (nextClosingBrace < configLen){
    nextClosingBrace = config.find("}", checkStart);
    if (nextClosingBrace == std::string::npos)
      printErrorAndExit("ERROR: no closing brace for server bloc");
    // if another server is within the chunk > error
    if (chunkHasToken(config.substr(checkStart, nextClosingBrace - checkStart), serverToken))
      printErrorAndExit("ERROR: bad server bloc layout");
    //find if a location bloc is in the chunk
    nextLocationBloc = config.substr(checkStart, nextClosingBrace).find(locationToken);
    if (nextLocationBloc == std::string::npos){
      if (chunkHasToken(config.substr(nextLocationBloc + locationToken.length(), nextClosingBrace - nextLocationBloc + locationToken.length()), locationToken))
      endOfBloc = nextClosingBrace;
      break;
    } else {
      checkStart = nextClosingBrace + 1;
    }
  }
  return endOfBloc;
}

void checkBlocsUnity(const std::string &config){
  size_t startOfBloc = 0;
  size_t endOfBloc = 0;
  const std::string serverToken = "server {";
  // std::cerr << "ENTERED PARSE SERVER BLOC\n";
  while (endOfBloc < config.length()){
    startOfBloc = config.find(serverToken, startOfBloc);
    endOfBloc = findEndOfBloc(config, startOfBloc + serverToken.length() + 1);// search for end of server bloc after server token
    startOfBloc = endOfBloc + 1;
  }
}
    //servers.at(serverNum).parseConfig(std::string(config, startOfBloc, endOfBloc - startOfBloc + 1));
    //std::cout << servers[serverNum];

bool hasValidBlocOpening(const std::string &line){
  size_t lineLen = line.length();
  size_t startOfToken =0, endOfToken =0;
  const std::string serverToken = "server";
  const std::string locationToken = "location";
  startOfToken = line.find_first_not_of(" \t");
  endOfToken = line.find_first_of(" ", startOfToken);
  // std::cout << "EOT: " << endOfToken << " SOT: " << startOfToken << " LEN: " << locationToken.length() << " LLEN: " << lineLen << std::endl;
  if (line.substr(startOfToken, endOfToken).find(serverToken) != std::string::npos
    && (serverToken.length() == endOfToken - startOfToken)){
      if (endOfToken == lineLen - 2)
        return true;
      return false;
  } else if (line.substr(startOfToken, endOfToken).find(locationToken) != std::string::npos
    && (locationToken.length() == endOfToken - startOfToken)){
      startOfToken = line.find_first_not_of(" ", endOfToken + 1);
      if (startOfToken == std::string::npos || line.at(startOfToken) != '/')
        return false;
      endOfToken = line.find(" ", startOfToken);
      if (endOfToken != std::string::npos && endOfToken + 2 == lineLen)
        return true;
      return false;
  } else {
      return false;
  }

}

void checkBlocOpening(const std::string &line){
    size_t lineLen = line.length();
    size_t endingCharPos = lineLen - 1;
    if (line.at(endingCharPos) == '{' && hasValidBlocOpening(line) == false)
		  printErrorAndExit("ERROR: bad synthax - FaultyLine: \'" + line + "\'\n");
}

void checkLineHasValidEol(const std::string &line){
  size_t lineLen = line.length();
  size_t endingCharPos = lineLen - 1;
  if (line.find_first_of(";{}", endingCharPos) == std::string::npos)
		printErrorAndExit("ERROR: bad eol - FaultyLine: \'" + line + "\'\n");
}

void checkLineHasNoAdditionalEolChar(const std::string &line){
  size_t lineLen = line.length();
  if (lineLen > 1 && line.substr(0, lineLen - 1).find_first_of("{};") != std::string::npos)
    printErrorAndExit("ERROR: bad synthaxe - FaultyLine: \'" + line + "\'\n");
}

void checkLineForbiddenWhitespaces(const std::string &line){
  size_t startOfFirstToken =0;
  startOfFirstToken = line.find_first_not_of(" \t");
  if (line.find_first_of("\t\r\f\v", startOfFirstToken + 1) != std::string::npos){
    printErrorAndExit("ERROR: unauthorized whitespace - FaultyLine: \'" + line + "\'\n");
  }
}

void checkInstructionLineLayout(const std::string &line){
	size_t startOfToken =0, endOfToken =0;
  size_t lineLen = line.length();
  if (lineLen > 1 && line.at(lineLen - 2) == ' ')
    printErrorAndExit("ERROR: bad synthaxe - FaultyLine: \'" + line + "\'\n");
	startOfToken = line.find_first_not_of(" \t");
	while (startOfToken < lineLen && endOfToken < lineLen){
		//std::cerr << "EOL: " << eol << " START: " << startOfToken << " END: " << endOfToken << std::endl;
		if (endOfToken != 0 && startOfToken - endOfToken > 2)
			printErrorAndExit("ERROR: too many spaces within instruction - FaultyLine: \'" + line + "\'\n");
		endOfToken = line.find(" ", startOfToken) - 1;
		startOfToken = line.find_first_not_of(" ", endOfToken + 1);
  }
}

bool isBlocOpeningLine(const std::string &line){
  return (line.find("server {") != std::string::npos || line.find("location /") != std::string::npos);
}

bool isBlocClosingLine(const std::string &line){
  size_t closingBracePos = line.find_first_not_of(" \t");
  return (line.at(closingBracePos) == '}');
}

void preParsing(const std::string& config){
  std::string line;
  size_t startOfLine =0, endOfLine =0;
  const size_t configLen = config.length();
  while (startOfLine < configLen){
    endOfLine = config.find("\n", startOfLine);
    line = config.substr(startOfLine, endOfLine - startOfLine);
    if (line.length() == 0){
      startOfLine = endOfLine + 1;
      continue;
    }
    checkLineHasValidEol(line);
    checkLineHasNoAdditionalEolChar(line);
    checkLineForbiddenWhitespaces(line);
    checkBlocOpening(line);
    if (!isBlocOpeningLine(line) && !isBlocClosingLine(line))
      checkInstructionLineLayout(line);
    std::cout << "SL: " << startOfLine << " - EL: " << endOfLine << " - LINE: " << line << std::endl;
    startOfLine = endOfLine + 1;
  }
}

int main (int argc, char *argv[]){
  // test if config file exist, otherwise exit
  if (argc != 2){
    std::cerr << "Wrong Number of arguments\n";
    exit(EXIT_FAILURE);
  }
  std::string config = configToString(argv[1]);
  //std::string parsedConfig;
  size_t countOfServers;

  preParsing(config);
  checkBlocsUnity(config);
  countOfServers = countServerBlocs(config);
  std::cout << "CONFIG LEN: " << config.length() << std::endl;
  if (countOfServers == 0)
    printErrorAndExit("ERROR: Wrong synthaxe for server bloc\n");
  std::cout << "NB OF SERVER: " << countOfServers << std::endl;
  std::vector<Server> bunchOfServers;
  bunchOfServers.assign(countOfServers, Server());
  //parseServerBloc(config, bunchOfServers, countOfServers);
}