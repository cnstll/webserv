#ifndef RESPONSE_HPP
#define RESPONSE_HPP


#include "core.hpp"


# include <iostream>
#include <stdlib.h>
#include<sys/socket.h>
#include <map>  
# ifndef ROOT_DIR
# define ROOT_DIR "/core/server_root"
# endif


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
    //! this obviously shouldn't be hardcoded... where should this info come from? the config file probably?
    std::string _Location = "/index.html";
    std::string timeAsString();
    const char *convertToCString();

public:
    Response(int code = 500);
    std::string codeToReasonPhrase(int);
    Response(const Response &newResponse);
    Response(std::map<std::string, std::string> &parsedRequest, int errorCode);
    ~Response();
    void sendResponse(int clientSocket);
    void addBody(std::string pathname);
    Response &operator=(const Response &newResponse);
};
#endif