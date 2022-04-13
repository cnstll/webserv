#include"cgiHandler.hpp"
#include"../core/Response.hpp"
#include<exception>

#include"../core/Request.hpp"

int cgiHandler::strlen_list(char **strList) {
    int i = 0;
    while (strList[i])
        i++;
    return i;
}

const char * cgiHandler::internalServerError::what( void ) const throw()
{
	return ("Internally, the server has errored. it is very sorry about this and rest assured it will not happen again");
}
const char * cgiHandler::CgiError::what( void ) const throw()
{
	return ("Internally, the server has errored. it is very sorry about this and rest assured it will not happen again");
}

char **cgiHandler::str_add(std::string str_to_add)
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


char	**cgiHandler::_calloc_str_list(size_t size)
{
	char	**str_list;

	str_list = (char **)calloc(size, sizeof(char *));
	if (!str_list)
		throw internalServerError();
	return (str_list);
}

cgiHandler::cgiHandler(void)
{
    return ;
}

cgiHandler::cgiHandler(std::map<std::string, std::string> &parsedRequest, std::string &scriptPathname, int serverSocket) : _serverSocket(serverSocket)
{
	std::string CGI_EXECUTOR = "/usr/bin/python";
	_parsedRequest = parsedRequest;
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

void	cgiHandler::_executeScript()
{
	if (execve(_args[0], _args, _environment) < 0)
	{
		throw CgiError();
	}
}

void	cgiHandler::cgiDispatch()
{
	int pid = -1;
	int fd[2];
	int stdoutDup;
	int stdinDup;
	
	if (pipe(fd) < 0)
		throw internalServerError();
	pid = fork();
	if (!pid)
	{
		if (dup2(_serverSocket, STDOUT_FILENO) == -1)
			throw CgiError();
		if (dup2(fd[0], STDIN_FILENO) == -1)
			throw CgiError();
		_executeScript();
	}
	if (write(fd[1], _messageBody.c_str(), _messageBody.length()) == -1)
		throw internalServerError();
	close(fd[1]);
}

void cgiHandler::handleCGI(int fd)
{
	try
	{
		cgiDispatch();
	}
	catch (cgiHandler::internalServerError &e)
	{
		std::cerr << e.what() << '\n';
		Response resp(_parsedRequest, 500);
		resp.addBody();
		resp.sendResponse(fd);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		Response resp(_parsedRequest, 500);
		resp.addBody();
		resp.sendResponse(fd);
		exit(0);
	}
}

cgiHandler::~cgiHandler(void)
{
    return ;
}

cgiHandler& cgiHandler::operator=(const cgiHandler& newenv)
{
    return *this;
}