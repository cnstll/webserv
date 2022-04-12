#include <string>
#include <sstream>

std::string readFileIntoString(const std::string& path);
bool doesFileExist(const std::string &name);
void printErrorAndExit(const std::string &errorMessage);
size_t getLineFromPosition(const std::string &str, size_t pos);
std::string numberToString(size_t &Number);
std::string numberToString(int &Number);