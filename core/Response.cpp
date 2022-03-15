#include "core.hpp"


# include <iostream>
#include <stdlib.h>
#include<sys/socket.h>  
#include "Response.hpp"

std::string Response::codeToReasonPhrase(int statusCode){
    // ! To be modified with a table of code
    (void)statusCode;
    return "OK";
}

Response::Response(int code)
    : _statusCode(code),
      _Date(this->timeAsString()), _Server("Webserv"), _ContentLength(0),
      _ContentType("text/html"), _Connection("Keep-Alive"){
    

    char buf[3];
    sprintf(buf,"%i", _statusCode);
    _Status = "HTTP/1.1 " + std::string(buf) + " " + _ReasonPhrase; 
    this->_ReasonPhrase = Response::codeToReasonPhrase(code);
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

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str(buffer);
    str.append(" GMT");
    return (str);
}


void Response::addBody(std::string pathname)
{
    std::ifstream input_file(pathname);
    char buf[10];
    if (!input_file.is_open())
    {
        std::cerr << "Could not open the file - '"
                  << pathname << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
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
    if (send(clientSocket, packagedResponse.c_str(), packagedResponse.size(), 0) < 0){
        exit(EXIT_FAILURE);
    }
}
