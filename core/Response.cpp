#include "core.hpp"
#include "Response.hpp"
#include "utils.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include<sys/socket.h>
#include <map>

std::string Response::codeToReasonPhrase(int statusCode){
    // ! To be modified with a table of code
    (void)statusCode;
    return "OK";
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
    std::cout << "Copy Constructor for Response called" << std::endl;
    return;
}

Response::~Response(void)
{
    std::cout << "Default Destructor for Response called" << std::endl;
    return;
}

Response &Response::operator=(const Response &newResponse)
{
    std::cout << "Response Assignement Operator called" << std::endl;
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
    std::cout << str << std::endl;
    return (str);
}

void Response::addBody(std::string pathname)
{
    std::ifstream input_file(pathname);
    char buf[10];
    if (!input_file.is_open()) //!doesFileExist(pathname))
    {
        std::cerr << "Could not open the file - '"
                  << pathname << "'" << std::endl;
        //std::string errorPageNotFound = ;
        input_file.open(ROOT_DIR + std::string("/404.html"), std::ifstream::in);
        //exit(EXIT_FAILURE);
    }
    _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    // std::cout << _Content << std::endl;
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

/**
 * @brief function used to send a Response to a client.
 * 
 */
void Response::sendResponse(int clientSocket){

    std::string carriageReturn = "\n";
    std::string packagedResponse = _Status + carriageReturn +
        "Date: " + _Date + carriageReturn +
        "Server: " + _Server + carriageReturn +
        "Content-Length: " + _ContentLength + carriageReturn +
        "Content-Type: " + _ContentType + carriageReturn +
        "Connection: " + _Connection + carriageReturn +
        carriageReturn + _Content
    ;
    // ! Handle error here. 
    size_t i;
    //FILE *thisIsAFile = fopen("colomban.txt", "w");
    if ((i = write(clientSocket, packagedResponse.c_str(), packagedResponse.size())) < 0){
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, packagedResponse.c_str(), packagedResponse.size());
}