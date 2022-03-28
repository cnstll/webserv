#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include"./libft/libft.h"

int	strlen_list(char **str_list)
{
	int	i;

	i = 0;
	while (str_list[i])
		i++;
	return (i);
}

char	**calloc_str_list(int size)
{
	char	**str_list;

	str_list = (char **)calloc(size, sizeof(char *));
	return (str_list);
}

char	**str_list_dup(char **src_list)
{
	char	**list_dup;
	int		i;

	i = 0;
	list_dup = calloc_str_list(strlen_list(src_list) + 1);
	if (!list_dup)
		return (NULL);
	while (src_list[i])
	{
		list_dup[i] = strdup(src_list[i]);
		if (!list_dup[i])
			return (NULL);
		i++;
	}
	return (list_dup);
}

char	**str_add(char **str_list, char *str_to_add)
{
	int		i;
	char	**dup_list;
	int		list_size;

	i = 0;
	list_size = strlen_list(str_list);
	dup_list = calloc_str_list(list_size + 2);
	if (!dup_list)
		return (NULL);
	while (i < list_size)
	{
		dup_list[i] = strdup(str_list[i]);
		if (!dup_list[i])
			return (NULL);
		i++;
	}
	dup_list[i] = strdup(str_to_add);
	if (!dup_list[i])
		return (NULL);
	return (dup_list);
}

char *str = "\n\
GATEWAY_INTERFACE=CGI/1.1\n\
HTTP_ACCEPT=text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
HTTP_ACCEPT_CHARSET=ISO-8859-1,utf-8;q=0.7,*;q=0.7\n\
HTTP_ACCEPT_ENCODING=gzip, deflate, br\n\
HTTP_ACCEPT_LANGUAGE=en-us,en;q=0.5\n\
HTTP_CONNECTION=keep-alive\n\
HTTP_HOST=example.com\n\
HTTP_USER_AGENT=Mozilla/5.0 Gecko/20100101 Firefox/67.0\n\
REMOTE_ADDR=127.0.0.1\n\
REMOTE_PORT=18000\n\
REQUEST_METHOD=GET\n\
SERVER_ADDR=127.0.0.1\n\
SERVER_NAME=webserv\n\
SERVER_PROTOCOL=HTTP/1.1\n\
";

int main(int argc, char **argv, char **envp)
{
    int pid;
    char **things_to_add_also = ft_split(str, '\n');

    int i = 0;
    pid = fork();
    if (!pid)
        execve("./ubuntu_cgi_tester", argv, things_to_add_also);
    return 0;
}