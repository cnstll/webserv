#ifndef RESPONSE_HPP
#define RESPONSE_HPP


#include "core.hpp"


# include <iostream>
#include <stdlib.h>
#include<sys/socket.h>  
# define ROOT_DIR "server_root"


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
    std::string timeAsString();
    const char *convertToCString();

public:
    Response(int code = 500);
    std::string codeToReasonPhrase(int);
    Response(const Response &newResponse);
    ~Response();
    void sendResponse(int clientSocket);
    void addBody(std::string pathname);

    Response &operator=(const Response &newResponse);
};
#endif