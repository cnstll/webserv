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
    ErrCodeMap[301] = "Moved Permanently";
    ErrCodeMap[302] = "Found";
    ErrCodeMap[307] = "Temporary Redirect";
    ErrCodeMap[308] = "Permanent Redirect";
    ErrCodeMap[400] = "Bad Request";
    ErrCodeMap[403] = "Forbiden";
    ErrCodeMap[404] = "Not Found";
    ErrCodeMap[405] = "Method Not Allowed";
    ErrCodeMap[408] = "Request Timeout";
    ErrCodeMap[411] = "Length Required";
    ErrCodeMap[413] = "Payload Too Large";
    ErrCodeMap[414] = "URI Too Long";
    ErrCodeMap[418] = "I'm a teapot";
    ErrCodeMap[451] = "Unavailable For Legal Reasons";
    ErrCodeMap[500] = "Internal Server Error";
    ErrCodeMap[501] = "Not Implemented";
    ErrCodeMap[502] = "Bad Gateway";
    ErrCodeMap[503] = "Service Unavailable";
    ErrCodeMap[504] = "Gateway Timeout";
    ErrCodeMap[505] = "HTTP Version Not Supported";
    return ErrCodeMap;
}

std::string Response::getErrorContent(int errCode)
{
    std::string errPathname = _currentServer.getServerConfigField("error_pages_dir") + "/" + numberToString(errCode) + ".html"; 
    if(doesFileExist(errPathname))
        return (readFileIntoString(errPathname));
    return "<Html>" + getCodeStatus(errCode) + "</Html>";
}

std::string Response::getCodeStatus(int errCode)
{
    std::map<int, std::string>::iterator it;
    it = _ErrCodeMap.find(errCode);
    if (it != _ErrCodeMap.end())
        return it->second;
    return "Untreated Error";
}

std::string Response::codeToReasonPhrase(int statusCode){
    return getCodeStatus(statusCode);
}

// Response::Response(int errorCode, Server &serv)
    // : _statusCode(errorCode),
    //   _Date(timeAsString()),_ServerName(serv.getServerConfigField("server_name")),
    //   _ContentType("text/html; charset=UTF-8"),
    //   _Connection(serv.getRequestField("Connection")),
    //   _currentServer(serv)
// {
// 
    // _uri = serv.getRequestField("requestURI");
    // if (_Connection == "" && _statusCode != 408)
        // _Connection = "keep-alive";
    // _ErrCodeMap = initErrCodeMap();
    // std::string statusCode = numberToString(_statusCode);
    // this->_ReasonPhrase = codeToReasonPhrase(errorCode);
    // _Status = "HTTP/1.1 " + statusCode + " " + _ReasonPhrase; 
// };


Response::Response(int errorCode, Server &serv)
    : _statusCode(errorCode),
      _Date(timeAsString()),_ServerName(serv.getServerConfigField("server_name")),
      _ContentType("text/html; charset=UTF-8"),
      _currentServer(serv){

    _Connection = "keep-alive";
    if (_statusCode >= 400){
        if (_statusCode == 408)
            _Connection = "close";
        _uri = "";
    }
    else
        _uri = serv.getRequestField("requestURI");
    _ErrCodeMap = initErrCodeMap();
    std::string statusCode = numberToString(_statusCode);
    this->_ReasonPhrase = codeToReasonPhrase(_statusCode);
    _Status = "HTTP/1.1 " + statusCode + " " + _ReasonPhrase;
};


Response::~Response(void)
{
    return;
}

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

std::string Response::directoryContents ( std::string pathname )
{  
  DIR *dh;
  struct dirent * contents;
  std::string ret;
  std::string newpath = pathname.substr(strlen(_currentServer.getServerConfigField("root").c_str()) + 2);
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
    std::string dirName = contents->d_name;
    if (dirName != "." && dirName != "..")
    {

    std::string name = "<li> <a href=\"" + _uri + "/" + dirName + "\">" + contents->d_name + "</a> </li>";
    ret += name += "\n";
    }
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
        if (!isADir(pathname))
        {
        std::ifstream input_file(pathname.c_str());
        _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
        }
        else {

            int autoindex = 1;
            if (autoindex) {
            _Content = directoryContents(pathname);
            } else {
                _Content = "<Html>Is A directory, for more clarity, turn Autoindexing on in the server's config</Html>";
            }
        }
   }
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

void Response::addBody(std::string pathname, Server *currentServer)
{
    char buf[10];

    if (_statusCode >= 300)
        _Content = getErrorContent(_statusCode);
    else
    {
        if (!isADir(pathname))
        {
            std::ifstream input_file(pathname.c_str());
            _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
        }
        else
        {
            std::string autoindex = currentServer->getLocationField(_uri, "autoindex");
            if (autoindex == "on")
                _Content = directoryContents(pathname);
            else
                _Content = "<Html>Is A directory, for more clarity, turn Autoindexing on in the server's config</Html>";
        }
    }
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

void Response::addBody()
{
    char buf[10];
    bzero(buf, 10);
    if (_statusCode >= 300)
        _Content = getErrorContent(_statusCode);    
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

void Response::addBody(int errorCode)
{
    _Content = getErrorContent(errorCode);
    size_t size = _Content.size();
    _ContentLength = numberToString(size);
}

void Response::sendResponse(int clientSocket){

    std::string CRLF = "\n";
    std::string contype;
    std::string packagedResponse = _Status + CRLF +
        "Date: " + _Date + CRLF +
        "Server: " + _ServerName + CRLF +
        "Content-Type: " + _ContentType + CRLF +
        "Connection: " + _Connection + CRLF;
    
    if (_ContentLength != "")
        packagedResponse += "Content-Length: " + _ContentLength + CRLF;
    if (_statusCode >= 300 && _statusCode < 400)
    {
        std::string redirectStr = _currentServer.getLocationField(_uri, "return");
        std::vector<std::string> v = tokenizeValues(redirectStr);
        _Location = v[1];
        packagedResponse += "Location: " + _Location + CRLF;
    }
    else{
        packagedResponse = packagedResponse + CRLF + _Content;
    }
    // std::cout << "RESPONSE-----------------------------------\n";
    // std::cout << packagedResponse << std::endl;
    // std::cout << "-----------------------------------RESPONSE\n";
    int i;
    if ((i = write(clientSocket, packagedResponse.c_str(), packagedResponse.size())) < 0){
        // exit(EXIT_FAILURE); //! thats no good, throw exception here?
    }
}
void Response::setConnectionField(const std::string &value){
    _Connection = value;
}

void Response::sendErrorResponse(int clientSocket, int errorCode){
    _statusCode = errorCode;
    addBody();
    sendResponse(clientSocket);
};
