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

int countServerBlocs(const std::string &config){
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
  // std::cout << "START: " << serverBlocStart << std::endl;
  while (checkStart < configLen){
    nextClosingBrace = config.find("}", checkStart);
    if (nextClosingBrace == std::string::npos)
      printErrorAndExit("ERROR: no closing brace for server bloc");
    // if another server is within the chunk > error
    //std::cout << "CS: " << checkStart << " CS CHAR: " << config.at(checkStart) << " NB CHAR: " << config.at(nextClosingBrace) << " configLen: " << configLen << std::endl;

    //find if a location bloc is in the chunk
    nextLocationBloc = config.substr(checkStart, nextClosingBrace - checkStart).find(locationToken);
    // std::cout << "CS: " << checkStart << " CB: " << nextClosingBrace << " NEXTLOC: " << nextLocationBloc << std::endl;
    if (nextLocationBloc == std::string::npos){
      endOfBloc = nextClosingBrace;
      // std::cout << "FOUND: " << endOfBloc << std::endl;
      break;
    } else {
      if (chunkHasToken(config.substr(checkStart, nextClosingBrace - checkStart), serverToken))
        printErrorAndExit("ERROR: server bloc enclosed in another server bloc\n");
      nextLocationBloc += checkStart;
      if (chunkHasToken(config.substr(nextLocationBloc + locationToken.length(), nextClosingBrace - nextLocationBloc - locationToken.length()), locationToken))
        printErrorAndExit("ERROR: location bloc enclosed in another location bloc");
      checkStart = nextClosingBrace + 1;
    }
  }
  return endOfBloc;
}

void checkBlocsAndParse(const std::string &config, std::vector<Server> &servers){
  size_t startOfBloc = 0;
  size_t endOfBloc = 0;
  const std::string serverToken = "server {";
  int potentialNumberOfServers = countServerBlocs(config);
  // std::cerr << "ENTERED PARSE SERVER BLOC\n";
  int i = 0;
  // TODO: LOCATION BLOC OUTSIDE OF SERVER BLOC NOT HANDLED
  while (potentialNumberOfServers > i){
    startOfBloc = config.find(serverToken, startOfBloc);
    //Find end of server bloc and do multiple checks
    endOfBloc = findEndOfBloc(config, startOfBloc + serverToken.length() + 1);// search for end of server bloc after server token
    servers.push_back(Server());
    servers[i].parseServerConfigFields(config.substr(startOfBloc, endOfBloc));
    std::cout << servers[i];
    startOfBloc = endOfBloc + 1;
    i++;
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
    //std::cout << "SL: " << startOfLine << " - EL: " << endOfLine << " - LINE: " << line << std::endl;
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
  preParsing(config);
  std::vector<Server> bunchOfServers;
  checkBlocsAndParse(config, bunchOfServers);
}