#ifndef ENV_CPP
# define ENV_CPP

#include <iostream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<iostream>
#include<map>

class cgiParams
{
    private:
        void    _writeBodyToScript();
        void    _setUpEnv();
        void    _setUpArgs();
        void    _executeScript();
        char **_calloc_str_list(size_t size);
        std::string _messageBody;
        int _serverSocket;
    public:
        char *_args[3];
        char **_environment;
        cgiParams();
        cgiParams(std::map<std::string, std::string> &parsedRequest, std::string &, int serverSocket);
        ~cgiParams();
        cgiParams &operator=(const cgiParams &newenv);
        char **str_add(std::string  str_to_add);
        int strlen_list(char **str_list);
        void    handleCGI();
        
        std::map<std::string, std::string> requestToEnvMap =
            {{"REQUEST_METHOD", "method"},
             {"QUERY_STRING", "message-body"},
             {"SCRIPT_NAME", "requestURI"},
             {"CONTENT_TYPE", "Content-Type"},
             {"HTTP_USER_AGENT", "User-Agent"},
             {"HTTP_CONNECTION", "Connection"},
             {"CONTENT_LENGTH", "Content-Length"}};
};
#endif