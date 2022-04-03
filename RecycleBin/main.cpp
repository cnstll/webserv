#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include "env.hpp"

// char *str = "\n\
// GATEWAY_INTERFACE=CGI/1.1\n\
// HTTP_ACCEPT=text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
// HTTP_ACCEPT_CHARSET=ISO-8859-1,utf-8;q=0.7,*;q=0.7\n\
// HTTP_ACCEPT_ENCODING=gzip, deflate, br\n\
// HTTP_ACCEPT_LANGUAGE=en-us,en;q=0.5\n\
// HTTP_CONNECTION=keep-alive\n\
// HTTP_HOST=example.com\n\
// HTTP_USER_AGENT=Mozilla/5.0 Gecko/20100101 Firefox/67.0\n\
// REMOTE_ADDR=127.0.0.1\n\
// REMOTE_PORT=18000\n\
// REQUEST_METHOD=GET\n\
// SERVER_ADDR=127.0.0.1\n\
// SERVER_NAME=webserv\n\
// SERVER_PROTOCOL=HTTP/1.1\n\
// ";

int main(int argc, char **argv, char **envp)
{
	env env_to_send;
    int pid;
    // char **things_to_add_also = ft_split(str, '\n');

    int i = 0;
    pid = fork();
    if (!pid)
         execve("./ubuntu_cgi_tester", argv, env_to_send._environment);
    return 0;
}