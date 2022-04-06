#ifndef ENV_CPP
# define ENV_CPP

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


class cgiHandler
{
    private:
        void    _writeBodyToScript();
        void    _setUpEnv();
        void    _setUpArgs();
        void    _executeScript();
        char **_calloc_str_list(size_t size);
        std::string _messageBody;
        int _serverSocket;

        class internalServerError : public std::exception
        {
            virtual const char *what(void) const throw();
        };

    public:
        char *_args[3];
        char **_environment;
        cgiHandler();
        cgiHandler(std::map<std::string, std::string> &parsedRequest, std::string &, int serverSocket);
        ~cgiHandler();
        cgiHandler &operator=(const cgiHandler &newenv);
        char **str_add(std::string  str_to_add);
        int strlen_list(char **str_list);
        void    handleCGI();
        

        std::map<std::string, std::string> requestToEnvMap =
            {{"REQUEST_METHOD", "method"},
             {"SCRIPT_NAME", "requestURI"},
             {"CONTENT_TYPE", "Content-Type"},
             {"QUERY_STRING", "queryString"},
             {"HTTP_USER_AGENT", "User-Agent"},
             {"HTTP_CONNECTION", "Connection"},
             {"CONTENT_LENGTH", "Content-Length"}};
};
#endif