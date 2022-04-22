#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include "../core/Server.hpp"

class Server;

class CgiHandler
{
private:
    void _writeBodyToScript();
    void _setUpEnv();
    void _setUpArgs();
    void _executeScript();
    void _serverRoot();
    void _callocEnv(size_t size);
    std::string _messageBody;
    int _serverSocket;
    void _cgiDispatch();
    Server &_currentServer;
    std::map<std::string, std::string> _parsedRequest;
    void _addVarToEnv(std::string str_to_add);
    char *_args[3];
    char **_environment;
    void _freeEnv();

    std::map<std::string, std::string> initRequestToEnvMap()
    {
        std::map<std::string, std::string> requestToEnvMap;
        requestToEnvMap["REQUEST_METHOD"] = "method";
        requestToEnvMap["SCRIPT_NAME"] = "requestURI";
        requestToEnvMap["CONTENT_TYPE"] = "Content-Type";
        requestToEnvMap["QUERY_STRING"] = "queryString";
        requestToEnvMap["HTTP_USER_AGENT"] = "User-Agent";
        requestToEnvMap["HTTP_CONNECTION"] = "Connection";
        requestToEnvMap["CONTENT_LENGTH"] = "Content-Length";
        return requestToEnvMap;
    };
    std::map<std::string, std::string> requestToEnvMap;

public:
    CgiHandler(std::map<std::string, std::string> &parsedRequest, std::string &, int serverSocket, Server &);
    ~CgiHandler();
    void handleCGI(int fd);

    /*-------------- EXCEPTION! -------------*/
    class internalServerError : public std::exception
    {
    public:
        virtual const char *what(void) const throw();
    };
    class CgiError : public std::exception
    {
    public:
        virtual const char *what(void) const throw();
    };
};
#endif