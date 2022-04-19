#ifndef RESPONSE_HPP
#define RESPONSE_HPP


#include "core.hpp"
#include "Server.hpp"


# include <iostream>
#include <stdlib.h>
#include<sys/socket.h>
#include <map>  


class Server;

class Response
{
private:
    int _statusCode;
    std::string _Status;
    std::string _ReasonPhrase;
    std::string _Date;
    std::string _Server;
    std::string _Type;
    std::string _ContentLength;
    std::string _ContentType;
    std::string _Content;
    std::string _Connection;
    std::string _uri;
    //! this obviously shouldn't be hardcoded... where should this info come from? the config file probably?
    std::string _Location;
    std::string timeAsString();
    const char *convertToCString();
    std::string directoryContents (std::string pathname);
    std::map<int, std::string>  _ErrCodeMap;
    std::map<int, std::string> initErrCodeMap();

public:
    std::string getErrorContent(int errCode);
    std::string getCodeStatus(int errCode);
    Response(int code = 500);
    std::string codeToReasonPhrase(int);
    // Response(const Response &newResponse);
    Response(std::map<std::string, std::string> &parsedRequest, int errorCode);
    ~Response();
    void sendResponse(int clientSocket);
    void addBody(std::string pathname);
    void addBody(std::string pathname, Server *);
    void addBody();
    // Response &operator=(const Response &newResponse);
};
#endif