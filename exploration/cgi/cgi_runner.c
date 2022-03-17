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

char	**str_list_join(char **s1, char **s2)
{
	char	**ret_list;
	int		i;
	int		len1;
	int		len2;

	len1 = strlen_list(s1);
	len2 = strlen_list(s2);
	ret_list = calloc_str_list(len1 + len2 + 1);
	i = 0;
	while (ret_list && i < len1)
	{
		ret_list[i] = strdup(s1[i]);
		if (!ret_list[i])
			return (NULL);
		i++;
	}
	i = 0;
	while (ret_list && i < len2)
	{
		ret_list[len1 + i] = strdup(s2[i]);
		if (!ret_list[len1 + i])
			return (NULL);
		i++;
	}
	return (ret_list);
}
char *str = "\n\
COMSPEC=C:\\Windows\\system32\\cmd.exe\n\
DOCUMENT_ROOT=C:/Program Files (x86)/Apache Software Foundation/Apache2.4/htdocs\n\
GATEWAY_INTERFACE=CGI/1.1\n\
HOME=/home/SYSTEM\n\
HTTP_ACCEPT=text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
HTTP_ACCEPT_CHARSET=ISO-8859-1,utf-8;q=0.7,*;q=0.7\n\
HTTP_ACCEPT_ENCODING=gzip, deflate, br\n\
HTTP_ACCEPT_LANGUAGE=en-us,en;q=0.5\n\
HTTP_CONNECTION=keep-alive\n\
HTTP_HOST=example.com\n\
HTTP_USER_AGENT=Mozilla/5.0 Gecko/20100101 Firefox/67.0\n\
PATH=/home/SYSTEM/bin:/bin:/cygdrive/c/progra~2/php:/cygdrive/c/windows/system32:...\n\
PATHEXT=.COM\n\
PATH_INFO=/foo/bar\n\
PATH_TRANSLATED=C:\\Program Files (x86)\\Apache Software Foundation\\Apache2.4\\htdocs\\foo\\bar\n\
QUERY_STRING=var1=value1&var2=with%20percent%20encoding\n\
REMOTE_ADDR=127.0.0.1\n\
REMOTE_PORT=63555\n\
REQUEST_METHOD=GET\n\
REQUEST_URI=/cgi-bin/printenv.pl/foo/bar?var1=value1&var2=with%20percent%20encoding\n\
SCRIPT_FILENAME=C:/Program Files (x86)/Apache Software Foundation/Apache2.4/cgi-bin/printenv.pl\n\
SCRIPT_NAME=/cgi-bin/printenv.pl\n\
SERVER_ADDR=127.0.0.1\n\
SERVER_ADMIN=(server admin's email address)\n\
SERVER_NAME=127.0.0.1\n\
SERVER_PORT=80\n\
SERVER_PROTOCOL=HTTP/1.1\n\
SERVER_SIGNATURE=\"\"\n\
SERVER_SOFTWARE=\"Apache/2.4.39 (Win32) PHP/7.3.7\"\n\
SYSTEMROOT=\"C:\\Windows\"\n\
TERM=\"cygwin\"\n\
";


    // while (environment_for_cgi_program[i])
    // {
    //     printf("%s\n", environment_for_cgi_program[i]);
    //     i++;
    // }


int main(int argc, char **argv, char **envp)
{
    int pid;
    char **copy_of_current_env = str_list_dup(envp);
    char **things_to_add_also = ft_split(str, '\n');
    char **environment_for_cgi_program = str_list_join(things_to_add_also, copy_of_current_env);

    int i = 0;

    pid = fork();
    if (!pid)
    {
        execve("./ubuntu_cgi_tester", argv, environment_for_cgi_program);
    }
    return 0;int main(int argc, char **argv, char **envp)
{
    int pid;
    char **copy_of_current_env = str_list_dup(envp);
    char **things_to_add_also = ft_split(str, '\n');
    char **environment_for_cgi_program = str_list_join(things_to_add_also, copy_of_current_env);

    int i = 0;

    pid = fork();
    if (!pid)
    {
        execve("./ubuntu_cgi_tester", argv, environment_for_cgi_program);
    }
    return 0;
}
}