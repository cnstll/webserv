#ifndef RESPONSE_HPP
#define RESPONSE_HPP


#include "core.hpp"
#include "Server.hpp"


# include <iostream>
#include <stdlib.h>
#include<sys/socket.h>
#include <map>  
# ifndef ROOT_DIR
# define ROOT_DIR "/core/server_root"
# endif



// std::map<int, std::string> static ErrCodeMap = 
//         {{200, "OK"},
//          {201, "Created"},
//          {300, "Multiple Choice"},
//          {301, "Moved Permanently"},
//          {302, "Found"},
//          {303, "See Other"},
//          {304, "Not Modified"},
//          {307, "Temporary Redirect"},
//          {308, "Permanent Redirect"},
//          {400, "Bad Request"},
//          {401, "Unauthorized"},
//          {402, "Payment Required"},
//          {403, "Forbiden"},
//          {404, "Not Found"},
//          {405, "Method Not Allowed"},
//          {406, "Not Acceptable"},
//          {407, "Proxy Authentication Required"},
//          {408, "Request Timeout"},
//          {409, "Conflict"},
//          {410, "Gone"},
//          {411, "Length Required"},
//          {412, "Precondition Failed"},
//          {413, "Payload Too Large"},
//          {414, "URI Too Long"},
//          {415, "Unsupported Media Type"},
//          {416, "Range Not Satisfiable"},
//          {417, "Expectation Failed"},
//          {418, "I'm a teapot"},
//          {421, "Misdirected Request"},
//          {422, "Unprocessable Entity"},
//          {423, "Locked"},
//          {426, "Upgrade Required"},
//          {428, "Precondition Required"},
//          {429, "Too Many Requests"},
//          {431, "Request Header Fields Too Large"},
//          {451, "Unavailable For Legal Reasons"},
//          {500, "Internal Server Error"},
//          {501, "Not Implemented"},
//          {502, "Bad Gateway"},
//          {503, "Service Unavailable"},
//          {504, "Gateway Timeout"},
//          {505, "HTTP Version Not Supported"},
//          {506, "Variant Also Negotiates"},
//          {510, "Not Extended"},
//          {511, "Network Authentication Required"}};

class Server;

class Response
{
private:
    int _statusCode;
    std::string _Status;
    std::string _ReasonPhrase;
    std::string _Date;
    std::string _ServerName;
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
    Server &_currentServer;

public:
    std::string getErrorContent(int errCode);
    std::string getCodeStatus(int errCode);
    std::string codeToReasonPhrase(int);
    // Response(const Response &newResponse);
    Response(int code, Server &serv);
    Response(std::map<std::string, std::string> &parsedRequest, int errorCode, Server &serv);
    ~Response();
    void sendResponse(int clientSocket);
    void sendErrorResponse(int clientSocket, int errorCode);
    void addBody(std::string pathname);
    void addBody(std::string pathname, Server *);
    void addBody();
    // Response &operator=(const Response &newResponse);
};
#endif