#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP
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

std::string configToString(char *filepath);
int countServerBlocs(const std::string &config);
bool chunkHasToken(const std::string &chunk, const std::string &token);
size_t findEndOfBloc(const std::string &config, size_t serverBlocStart);
void checkBlocsAndParse(const std::string &config, std::vector<Server> &servers);
bool hasValidBlocOpening(const std::string &line);
void checkBlocOpening(const std::string &line);
void checkLineHasValidEol(const std::string &line);
void checkLineHasNoAdditionalEolChar(const std::string &line);
void checkLineForbiddenWhitespaces(const std::string &line);
void checkInstructionLineLayout(const std::string &line);
bool isBlocOpeningLine(const std::string &line);
bool isBlocClosingLine(const std::string &line);
void preParsing(const std::string& config);
#endif