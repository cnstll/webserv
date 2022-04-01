#include"cgiParams.hpp"



int cgiParams::strlen_list(char **strList) {
    int i = 0;
    while (strList[i])
        i++;
    return i;
}

char **cgiParams::str_add(std::string str_to_add)
{
	int		i;
	char	**dup_list;
	int		list_size;

	i = 0;
	list_size = strlen_list(_environment);
	dup_list = calloc_str_list(list_size + 2);
	if (!dup_list)
		return (NULL);
	while (i < list_size)
	{
		dup_list[i] = strdup(_environment[i]);
		if (!dup_list[i])
			return (NULL);
		i++;
	}
	dup_list[i] = strdup(str_to_add.c_str());
	if (!dup_list[i])
		return (NULL);
    free(_environment);
    _environment = dup_list;
	return (dup_list);
}


char	**cgiParams::calloc_str_list(size_t size)
{
	char	**str_list;

	str_list = (char **)calloc(size, sizeof(char *));
	return (str_list);
}


cgiParams::cgiParams(void)
{
    _environment = calloc_str_list(1);
    str_add("GATEWAY_INTERFACE=CGI/1.1");
    str_add("HTTP_CONNECTION=keep-alive");
    str_add("REMOTE_ADDR=127.0.0.1");
    str_add("REMOTE_PORT=18000");
    str_add("REQUEST_METHOD=GET");
    str_add("SERVER_PROTOCOL=HTTP/1.1");
    str_add("SERVER_NAME=webserv");
    str_add("SERVER_ADDR=127.0.0.1");
    str_add("HTTP_USER_AGENT=Mozilla/5.0 Gecko/20100101 Firefox/67.0");


    int i = 0;
    return ;
}

cgiParams::cgiParams(std::map<std::string, std::string> &parsedRequest)
{
	    _environment = calloc_str_list(1);
    str_add("GATEWAY_INTERFACE=CGI/1.1");
    str_add("SERVER_NAME=webserv");
    str_add("SERVER_PROTOCOL=HTTP/1.1");
    str_add("SERVER_SOFTWARE=webserv");



	std::map<std::string, std::string>::iterator it = requestToEnvMap.begin();

	while (it != requestToEnvMap.end())
	{
		if (parsedRequest[it->second] != "")
		{
			std::string str = it->first + "=" + parsedRequest[it->second];
			str_add(str.c_str());
		}
		++it;
	}

    str_add("SERVER_ADDR=127.0.0.1");
    str_add("REMOTE_ADDR=127.0.0.1");
    str_add("REMOTE_PORT=18000");
    return ;
}


cgiParams::~cgiParams(void)
{
    return ;
}

cgiParams& cgiParams::operator=(const cgiParams& newenv)
{
    return *this;
}


// int main(int argc, char **argv, char **envp)
// {
// 	cgiParams env_to_send;
//     // int pid;
//     // char **things_to_add_also = ft_split(str, '\n');

//     // int i = 0;
//     // pid = fork();
//     // if (!pid)
//     //     execve("./ubuntu_cgi_tester", argv, things_to_add_also);
//     return 0;
// }