#include"cgiParams.hpp"
#include"../core/Request.hpp"

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
	dup_list = _calloc_str_list(list_size + 2);
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


char	**cgiParams::_calloc_str_list(size_t size)
{
	char	**str_list;

	str_list = (char **)calloc(size, sizeof(char *));
	return (str_list);
}

cgiParams::cgiParams(void)
{
    return ;
}

cgiParams::cgiParams(std::map<std::string, std::string> &parsedRequest, std::string &scriptPathname, int serverSocket) : _serverSocket(serverSocket)
{
	std::string CGI_EXECUTOR = "/usr/bin/python";

	_args[0] = (char *)CGI_EXECUTOR.c_str();
	_args[1] = (char *)scriptPathname.c_str();
	_args[2] = NULL;

	_environment = _calloc_str_list(1);
    str_add("GATEWAY_INTERFACE=CGI/1.1");
    str_add("SERVER_NAME=webserv");
    str_add("SERVER_PROTOCOL=HTTP/1.1");
    str_add("SERVER_SOFTWARE=webserv");


	_messageBody = parsedRequest["message-body"];
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
	return;
}

void	cgiParams::_executeScript()
{
	execve(_args[0], _args, _environment);
}

void	cgiParams::handleCGI()
{
	int pid = -1;
	pid = fork();
	if (!pid)
	{
		if (_messageBody != "")
			_writeBodyToScript();
		_executeScript();
	}
	//wait(NULL);
}

void	cgiParams::_writeBodyToScript()
{
	int fd[2];
	pipe(fd);
	int stdoutDup = dup(STDOUT_FILENO);
	int stdinDup = dup(STDIN_FILENO);
	dup2(_serverSocket, STDOUT_FILENO);
	dup2(fd[0], STDIN_FILENO);
	write(fd[1], _messageBody.c_str(), _messageBody.length());
	close(fd[1]);
}

cgiParams::~cgiParams(void)
{
    return ;
}

cgiParams& cgiParams::operator=(const cgiParams& newenv)
{
    return *this;
}