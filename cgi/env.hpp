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
//#include"./libft/libft.h"


    // Request specific variables:
    //     SERVER_PORT: TCP port (decimal).
    //     PATH_INFO: path suffix, if appended to URL after program name and a slash.
    //     PATH_TRANSLATED: corresponding full path as supposed by server, if PATH_INFO is present.
    //     QUERY_STRING: the part of URL after ? character. The query string may be composed of *name=value pairs separated with ampersands (such as var1=val1&var2=val2...) when used to submit form data transferred via GET method as defined by HTML application/x-www-form-urlencoded.
    //     REMOTE_HOST: host name of the client, unset if server did not perform such lookup.
    //     REMOTE_ADDR: IP address of the client (dot-decimal).
    //     Variables passed by user agent (HTTP_ACCEPT, HTTP_ACCEPT_LANGUAGE, HTTP_USER_AGENT, HTTP_COOKIE and possibly others) contain values of corresponding HTTP headers and therefore have the same sense.


class env
{
    private:
    public:
        char **_environment;
        env();
        env(std::map<std::string, std::string> &parsedRequest);
        ~env();
        env &operator=(const env &newenv);
        char **str_add(std::string  str_to_add);
        int strlen_list(char **str_list);
        char **calloc_str_list(size_t size);
    std::map<std::string, std::string> requestToEnvMap =
        {{"REQUEST_METHOD", "method"},
         {"QUERY_STRING", "message-body"},
         {"SCRIPT_NAME", "requestURI"},
         {"CONTENT_TYPE", "Content-Type"},
         {"CONTENT_LENGTH", "Content-Length"}};
};
#endif