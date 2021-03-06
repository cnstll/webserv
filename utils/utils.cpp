#include "utils.hpp"
#include <dirent.h>

void log(std::string logMessage)
{
  std::ofstream myfile;
  myfile.open ("./log/server.log", std::fstream::app);
  myfile << logMessage << std::endl;
  myfile.close();
}

std::string readFileIntoString(const std::string& path) {
    std::ifstream input_file(path.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '"
             << path << "'" << std::endl;
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int strlen_list(char **strList) {
    int i = 0;
    while (strList[i])
        i++;
    return i;
}

int check(int return_value, std::string const &error_msg)
{
  if (return_value < 0)
  {
    log(error_msg);
    return -1;
  }
  return 1;
}

const char *string_to_c_str(const std::string& str)
{
    const char *ret_str = str.c_str();
    return (ret_str);
}

const char *file_to_c_string(const std::string& path)
{
    std::string str = readFileIntoString(path);
    const char *c_str = string_to_c_str(str);
    return (c_str);
}

bool checkErrorFlags(int event)
{
  if (event & EPOLLERR)
    return (false);
  if (event & EPOLLHUP)
    return (false);
  if (event & EPOLLRDHUP)
    return (false);
  return (true);
}

bool checkHangUpFlags(int event)
{

  if (event & EPOLLHUP)
    return (false);
  if (event & EPOLLRDHUP)
    return (false);
  return (true);
}


int checkFatal(int return_value, const std::string &error_msg)
{
	if (return_value < 0)
	{
		std::cerr << error_msg << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		return 1;
}

bool isInUpdatedFds(struct epoll_event *events, int fd, int countOfFdActualized)
{
  for (int i = 0; i < countOfFdActualized; i++)
  {
    if (events[i].data.fd == fd)
      return true;
  }
  return false;
}

bool isSeverFd(int fd, std::map<int, Server> serverMap)
{
  std::map<int, Server>::iterator iter = serverMap.begin();
  while (iter != serverMap.end())
  {
    if (iter->first == fd)
      return 1;
    ++iter;
  }
  return 0;
}

bool isADir(std::string directoryPath)
{  
  DIR *dh;
  
  dh = opendir (directoryPath.c_str());
  
  if ( !dh )
    return 0;
  closedir ( dh );
  return 1;
}

bool doesFileExist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else if (isADir(name)) {
        return true;
    } else {
        return false;
    }   
}

void printErrorAndExit(const std::string &errorMessage){
    std::cerr << errorMessage;
    exit(EXIT_FAILURE);
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

int stringToNumber(std::string str){
    int result;          //number which will contain the result
    std::istringstream convert(str); // stringstream used for the conversion constructed with the contents of 'Text' 
    if ( !(convert >> result) ) //give the value to 'result' using the characters in the stream
        result = 0;             //if that fails set 'result' to 0
    return (result);
}

std::string numberToString(int &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string
}

std::string numberToString(const int &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string
}

std::string numberToString(size_t &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string
}

std::vector<std::string> tokenizeValues(std::string &str){
    size_t startOfToken = 0, endOfToken = 0;
    std::vector<std::string> tokenized;
    while (endOfToken != std::string::npos)
    {
        startOfToken = str.find_first_not_of(" ", endOfToken);
        endOfToken = str.find(" ", startOfToken);
        if (endOfToken != std::string::npos){
            tokenized.push_back(str.substr(startOfToken, endOfToken - startOfToken));
            endOfToken += 1;
        }
    }
    tokenized.push_back(str.substr(startOfToken, str.length() - startOfToken));
    return tokenized;
}

bool strIsInVector(const std::string &str, const std::vector<std::string> v){
    std::vector<std::string>::const_iterator iterVec = v.begin();
    while (iterVec != v.end())
    {
        if (*iterVec == str)
            return true;
        ++iterVec;
    }
    if (iterVec == v.end())
        return false;
    return true;
}