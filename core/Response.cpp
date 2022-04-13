#include "core.hpp"
#include "Response.hpp"
#include "utils.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include<sys/socket.h>
#include <map>

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
    std::map<int, std::string>::iterator it = ErrCodeMap.find(errCode);
    if (it != ErrCodeMap.end())
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
      _ContentType(parsedRequest["Content-Type"]), _Connection(parsedRequest["Connection"]){

    char buf[3];
    sprintf(buf,"%i", _statusCode);
    this->_ReasonPhrase = codeToReasonPhrase(errorCode);
    _Status = "HTTP/1.1 " + std::string(buf) + " " + _ReasonPhrase; 
};

Response::Response(const Response &newResponse)
{
    return;
}

Response::~Response(void)
{
    return;
}

Response &Response::operator=(const Response &newResponse)
{
    return *this;
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

void Response::addBody(std::string pathname)
{
    /*
    if response code > 200
    add body uses the content from the map in thhe response.
    */
    char buf[10];

    if (_statusCode >= 300)
        _Content = getErrorContent(_statusCode);    
    else{
        std::ifstream input_file(pathname.c_str());
        _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
   }
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

void Response::addBody()
{
    /*
    if response code > 200
    add body uses the content from the map in thhe response.
    */
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
    size_t i;
    if ((i = write(clientSocket, packagedResponse.c_str(), packagedResponse.size())) < 0){
        exit(EXIT_FAILURE);
    }
}