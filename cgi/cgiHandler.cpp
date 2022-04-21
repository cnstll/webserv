#include"cgiHandler.hpp"
#include"../core/Response.hpp"
#include"../core/Request.hpp"
#include<exception>


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
	return ("EXECVE FAILED");
}


void	free_str_list(char **str_list, int size_to_free)
{
	int	i;

	i = 0;
	while (i < size_to_free && str_list[i])
	{
		free(str_list[i]);
		i++;
	}
	free(str_list);
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
    free_str_list(_environment, strlen_list(_environment));
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

cgiHandler::cgiHandler(std::map<std::string, std::string> &parsedRequest, std::string &scriptPathname, int serverSocket, Server &currentServer) : _serverSocket(serverSocket), _currentServer(currentServer)
{
	_parsedRequest = parsedRequest;
	std::string CGI_EXECUTOR = "/usr/bin/python";
	_args[0] = (char *)CGI_EXECUTOR.c_str();
	_args[1] = (char *)scriptPathname.c_str();
	_args[2] = NULL;
	requestToEnvMap = initRequestToEnvMap();
	_environment = _calloc_str_list(1);
	std::string path_info = "PATH_INFO=" + _currentServer.getLocationField(parsedRequest["requestURI"], "working_dir");
    std::string serverName = "SERVER_NAME=" + _currentServer.getServerConfigField("server_name");
    std::string serverPort = "SERVER_PORT=" + _currentServer.getServerPort();
	
	str_add("GATEWAY_INTERFACE=CGI/1.1");
    str_add("SERVER_PROTOCOL=HTTP/1.1");
    str_add("SERVER_SOFTWARE=webserv");
    str_add("SERVER_ADDR=127.0.0.1");
    str_add("REMOTE_ADDR=127.0.0.1");
    
	str_add(path_info.c_str());
    str_add(serverName.c_str());
    str_add(serverPort.c_str());

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
	return;
}

void	cgiHandler::_executeScript()
{
	std::string CGI_EXECUTOR = "/usr/bin/python";

	_args[0] = (char *)CGI_EXECUTOR.c_str();
	if (execve(_args[0], _args, _environment) < 0)
	{
		throw CgiError();
	}
}

void	cgiHandler::cgiDispatch()
{
	int pid = -1;
	int fd[2];
	
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
		Response resp(_parsedRequest, 500, _currentServer);
		resp.addBody();
		resp.sendResponse(fd);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		Response resp(_parsedRequest, 502, _currentServer);
		resp.addBody();
		resp.sendResponse(fd);
		exit(0);
	}
}

cgiHandler::~cgiHandler(void)
{
	free_str_list(_environment, strlen_list(_environment));
    return ;
}