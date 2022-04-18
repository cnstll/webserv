#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

#include <iostream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<iostream>
#include<exception>
#include<map>
#include <sys/wait.h>
#include "../core/Server.hpp"

class Server;

class cgiHandler
{
    private:
        void    _writeBodyToScript();
        void    _setUpEnv();
        void    _setUpArgs();
        void    _executeScript();
        void    _serverRoot();
        char **_calloc_str_list(size_t size);
        std::string _messageBody;
        int _serverSocket;
        void cgiDispatch();
        Server &_currentServer;
        std::map<std::string, std::string> _parsedRequest;

    public:
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
        char *_args[3];
        char **_environment;
        // cgiHandler();
        cgiHandler(std::map<std::string, std::string> &parsedRequest, std::string &, int serverSocket, Server &);
        ~cgiHandler();
        // cgiHandler &operator=(const cgiHandler &newenv);
        char **str_add(std::string  str_to_add);
        int strlen_list(char **str_list);
        void    handleCGI(int fd);
        

        std::map<std::string, std::string> initRequestToEnvMap() {
            std::map<std::string, std::string> requestToEnvMap;
            requestToEnvMap["REQUEST_METHOD"] = "method";
            requestToEnvMap["SCRIPT_NAME"] = "requestURI";
            requestToEnvMap["CONTENT_TYPE"] = "Content-Type";
            requestToEnvMap["QUERY_STRING"]  = "queryString";
            requestToEnvMap["HTTP_USER_AGENT"] = "User-Agent";
            requestToEnvMap["HTTP_CONNECTION"] = "Connection";
            requestToEnvMap["CONTENT_LENGTH"] = "Content-Length";
            return requestToEnvMap;
        };

        std::map<std::string, std::string> requestToEnvMap;
};
#endif