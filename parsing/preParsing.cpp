#include "parsing.hpp"

static bool hasValidBlocOpening(const std::string &line){
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

static void checkBlocOpening(const std::string &line){
    size_t lineLen = line.length();
    size_t endingCharPos = lineLen - 1;
    if (line.at(endingCharPos) == '{' && hasValidBlocOpening(line) == false)
		  printErrorAndExit("ERROR: bad synthax - FaultyLine: \'" + line + "\'\n");
}

static void checkLineHasValidEol(const std::string &line){
  size_t lineLen = line.length();
  size_t endingCharPos = lineLen - 1;
  if (line.find_first_of(";{}", endingCharPos) == std::string::npos)
		printErrorAndExit("ERROR: bad eol - FaultyLine: \'" + line + "\'\n");
}

static void checkLineHasNoAdditionalEolChar(const std::string &line){
  size_t lineLen = line.length();
  if (lineLen > 1 && line.substr(0, lineLen - 1).find_first_of("{};") != std::string::npos)
    printErrorAndExit("ERROR: bad synthaxe - FaultyLine: \'" + line + "\'\n");
}

static void checkLineForbiddenWhitespaces(const std::string &line){
  size_t startOfFirstToken =0;
  startOfFirstToken = line.find_first_not_of(" \t");
  if (line.find_first_of("\t\r\f\v", startOfFirstToken + 1) != std::string::npos){
    printErrorAndExit("ERROR: unauthorized whitespace - FaultyLine: \'" + line + "\'\n");
  }
}

static int countTokenInLine(const std::string &line){
  size_t startOfToken = 0, endOfToken = 0;
  startOfToken = line.find_first_not_of(" \t");
  int count = 0;
  while (1){
    ++count;
    endOfToken = line.find(" ", startOfToken);
    if (endOfToken == std::string::npos)
      return count;
    endOfToken -= 1;
    startOfToken = line.find_first_not_of(" ", endOfToken + 1);
  }
  return count;
}

static void checkInstructionLineLayout(const std::string &line){
	size_t startOfToken =0, endOfToken =0;
  size_t lineLen = line.length();
  if (lineLen > 1 && line.at(lineLen - 2) == ' ')
    printErrorAndExit("ERROR: bad synthaxe - FaultyLine: \'" + line + "\'\n");
	if (countTokenInLine(line) < 2)
    printErrorAndExit("ERROR: too few parameters - FaultyLine: \'" + line + "\'\n");
  startOfToken = line.find_first_not_of(" \t");
	while (startOfToken < lineLen && endOfToken < lineLen){
    if (endOfToken != 0 && startOfToken - endOfToken > 2)
			printErrorAndExit("ERROR: too many spaces within instruction - FaultyLine: \'" + line + "\'\n");
		endOfToken = line.find(" ", startOfToken) - 1;
		startOfToken = line.find_first_not_of(" ", endOfToken + 1);
  }
}

static bool isBlocOpeningLine(const std::string &line){
  return (line.find("server {") != std::string::npos || line.find("location /") != std::string::npos);
}

static bool isBlocClosingLine(const std::string &line){
  size_t closingBracePos = line.find_first_not_of(" \t");
  if (closingBracePos == std::string::npos)
    return false;
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