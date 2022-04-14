#include "core.hpp"
#include "Response.hpp"
#include "../utils/utils.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include<sys/socket.h>
#include <map>
#include <dirent.h>
#include <string.h>

std::map<int, std::string> Response::initErrCodeMap()
{
    std::map<int, std::string> static ErrCodeMap;
    ErrCodeMap[200] = "OK";
    ErrCodeMap[201] = "Created";
    ErrCodeMap[300] = "Multiple Choice";
    ErrCodeMap[301] = "Moved Permanently";
    ErrCodeMap[302] = "Found";
    ErrCodeMap[303] = "See Other";
    ErrCodeMap[304] = "Not Modified";
    ErrCodeMap[307] = "Temporary Redirect";
    ErrCodeMap[308] = "Permanent Redirect";
    ErrCodeMap[400] = "Bad Request";
    ErrCodeMap[401] = "Unauthorized";
    ErrCodeMap[402] = "Payment Required";
    ErrCodeMap[403] = "Forbiden";
    ErrCodeMap[404] = "Not Found";
    ErrCodeMap[405] = "Method Not Allowed";
    ErrCodeMap[406] = "Not Acceptable";
    ErrCodeMap[407] = "Proxy Authentication Required";
    ErrCodeMap[408] = "Request Timeout";
    ErrCodeMap[409] = "Conflict";
    ErrCodeMap[410] = "Gone";
    ErrCodeMap[411] = "Length Required";
    ErrCodeMap[412] = "Precondition Failed";
    ErrCodeMap[413] = "Payload Too Large";
    ErrCodeMap[414] = "URI Too Long";
    ErrCodeMap[415] = "Unsupported Media Type";
    ErrCodeMap[416] = "Range Not Satisfiable";
    ErrCodeMap[417] = "Expectation Failed";
    ErrCodeMap[418] = "I'm a teapot";
    ErrCodeMap[421] = "Misdirected Request";
    ErrCodeMap[422] = "Unprocessable Entity";
    ErrCodeMap[423] = "Locked";
    ErrCodeMap[426] = "Upgrade Required";
    ErrCodeMap[428] = "Precondition Required";
    ErrCodeMap[429] = "Too Many Requests";
    ErrCodeMap[431] = "Request Header Fields Too Large";
    ErrCodeMap[451] = "Unavailable For Legal Reasons";
    ErrCodeMap[500] = "Internal Server Error";
    ErrCodeMap[501] = "Not Implemented";
    ErrCodeMap[502] = "Bad Gateway";
    ErrCodeMap[503] = "Service Unavailable";
    ErrCodeMap[504] = "Gateway Timeout";
    ErrCodeMap[505] = "HTTP Version Not Supported";
    ErrCodeMap[506] = "Variant Also Negotiates";
    ErrCodeMap[510] = "Not Extended";
    ErrCodeMap[511] = "Network Authentication Required";
    return ErrCodeMap;
}

std::string Response::getErrorContent(int errCode)
{
    char buf[3];
    sprintf(buf, "%d", errCode);
    std::string errPathname = "." + std::string(ROOT_DIR) + "/" + buf + ".html"; 
    if(doesFileExist(errPathname))
        return (readFileIntoString(errPathname));
    return "<Html>" + getCodeStatus(errCode) + "</Html>";
}

std::string Response::getCodeStatus(int errCode)
{
    std::map<int, std::string>::iterator it = _ErrCodeMap.find(errCode);
    if (it != _ErrCodeMap.end())
        return it->second;
    return "";
}

std::string Response::codeToReasonPhrase(int statusCode){
    return getCodeStatus(statusCode);
}

Response::Response(int code)
    : _statusCode(code),
      _Date(timeAsString()), _Server("Webserv"), _ContentLength(""),
      _ContentType("text/html"), _Connection("Keep-Alive"){
    

    char buf[3];
    sprintf(buf,"%i", _statusCode);
    this->_ReasonPhrase = Response::codeToReasonPhrase(code);
    _Status = "HTTP/1.1 " + std::string(buf) + " " + _ReasonPhrase; 
};

Response::Response(std::map<std::string, std::string> &parsedRequest, int errorCode)
    : _statusCode(errorCode),
      _Date(timeAsString()), _Server("Webserv"), _ContentLength(""),
      _ContentType(parsedRequest["Content-Type"]), _Connection(parsedRequest["Connection"]), _Location("./index.html"){

    char buf[3];
    _ErrCodeMap = initErrCodeMap();
    sprintf(buf,"%i", _statusCode);
    this->_ReasonPhrase = codeToReasonPhrase(errorCode);
    _Status = "HTTP/1.1 " + std::string(buf) + " " + _ReasonPhrase; 
};

// Response::Response(const Response &newResponse)
// {
//     return;
// }

Response::~Response(void)
{
    return;
}

// Response &Response::operator=(const Response &newResponse)
// {
//     return *this;
// }

std::string Response::timeAsString()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str("");
    str.append(buffer);
    str.append(" GMT");
    return (str);
}

bool isADir2(std::string directoryPath)
{  
  DIR *dh;
  
  dh = opendir (directoryPath.c_str());
  
  if ( !dh )
    return 0;
  closedir ( dh );
  return 1;
}



std::string Response::directoryContents ( std::string pathname )
{  
  DIR *dh;
  struct dirent * contents;
  std::string ret;
  std::string newpath = pathname.substr(strlen(ROOT_DIR) + 2);
  newpath += "/";

  
  dh = opendir ( pathname.c_str() );
  
  if ( !dh )
  {
    _statusCode = 404;
    return getErrorContent(404);
  }
  ret += "<Html>\n <h1> This is Otto's index, he's very proud of it so don't be rude now... </h1> <h3>";
  while ( ( contents = readdir ( dh ) ) != NULL )
  {
    std::string name = "<li> <a href=\"" + std::string(contents->d_name) + "\">" + contents->d_name + "</a> </li>";
    ret += name += "\n";
  }
  ret += "</h3>";
  ret += "</Html>\n";
  closedir ( dh );
  return ret;
}

void Response::addBody(std::string pathname)
{
    char buf[10];

    if (_statusCode >= 300)
        _Content = getErrorContent(_statusCode);    
    else{
        if (!isADir2(pathname))
        {
        std::ifstream input_file(pathname.c_str());
        _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
        }
        else {
            _Content = directoryContents(pathname);
        }
   }
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

void Response::addBody()
{
    char buf[10];
    if (_statusCode >= 300)
        _Content = getErrorContent(_statusCode);    
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

void Response::sendResponse(int clientSocket){

    std::string CRLF = "\n";
    std::string contype;
    std::string packagedResponse = _Status + CRLF +
        "Date: " + _Date + CRLF +
        "Server: " + _Server + CRLF +
        "Content-Length: " + _ContentLength + CRLF +
        "Content-Type: " + contype + CRLF +
        "Connection: " + _Connection + CRLF;


    if (_statusCode >= 300 && _statusCode < 400)
        packagedResponse += "Location: " + _Location + CRLF;
    packagedResponse = packagedResponse + CRLF + _Content;
    // ! Handle error here. 
    int i;
    if ((i = write(clientSocket, packagedResponse.c_str(), packagedResponse.size())) < 0){
        exit(EXIT_FAILURE); //! thats no good, throw exception here?
    }
}