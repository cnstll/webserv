#include"CgiHandler.hpp"
#include"../core/Response.hpp"
#include"../core/Request.hpp"
#include<exception>




const char * CgiHandler::internalServerError::what( void ) const throw()
{
	return ("Internally, the server has errored. it is very sorry about this and rest assured it will not happen again");
}

const char * CgiHandler::CgiError::what( void ) const throw()
{
	return ("EXECVE FAILED");
}


void CgiHandler::_callocEnv(size_t size)
{

	_environment = (char **)calloc(size, sizeof(char *));
	if (!_environment)
		throw internalServerError();
}

void CgiHandler::_addVarToEnv(std::string str_to_add)
{
	int		i;
	char	**dup_list;
	int		list_size;

	i = 0;
	list_size = strlen_list(_environment);
	dup_list = (char **)calloc(list_size + 2, sizeof(char *));
	if (!dup_list)
		throw internalServerError();
	while (i < list_size)
	{
		dup_list[i] = strdup(_environment[i]);
		if (!dup_list[i])
				throw internalServerError();
		i++;
	}
	dup_list[i] = strdup(str_to_add.c_str());
	if (!dup_list[i])
		throw internalServerError();
    _freeEnv();
    _environment = dup_list;
}

void	CgiHandler::_freeEnv()
{
	int	i;
	int size =  strlen_list(_environment);

	i = 0;
	while (i < size && _environment[i])
	{
		free(_environment[i]);
		i++;
	}
	free(_environment);
}

CgiHandler::CgiHandler(std::map<std::string, std::string> &parsedRequest, std::string &scriptPathname, int serverSocket, Server &currentServer) : _serverSocket(serverSocket), _currentServer(currentServer)
{
	_parsedRequest = parsedRequest;
	std::string CGI_EXECUTOR = "/usr/bin/python";
	_args[0] = (char *)CGI_EXECUTOR.c_str();
	_args[1] = (char *)scriptPathname.c_str();
	_args[2] = NULL;
	requestToEnvMap = initRequestToEnvMap();
	_callocEnv(1);
	std::string path_info = "PATH_INFO=" + _currentServer.getLocationField(parsedRequest["requestURI"], "working_dir");
    std::string serverName = "SERVER_NAME=" + _currentServer.getServerConfigField("server_name");
    std::string serverPort = "SERVER_PORT=" + _currentServer.getServerPort();
	
	_addVarToEnv("GATEWAY_INTERFACE=CGI/1.1");
    _addVarToEnv("SERVER_PROTOCOL=HTTP/1.1");
    _addVarToEnv("SERVER_SOFTWARE=webserv");
    _addVarToEnv("SERVER_ADDR=127.0.0.1");
    _addVarToEnv("REMOTE_ADDR=127.0.0.1");
    
	_addVarToEnv(path_info.c_str());
    _addVarToEnv(serverName.c_str());
    _addVarToEnv(serverPort.c_str());

	_messageBody = parsedRequest["message-body"];
	std::map<std::string, std::string>::iterator it = requestToEnvMap.begin();

	while (it != requestToEnvMap.end())
	{
		if (parsedRequest[it->second] != "")
		{
			std::string str = it->first + "=" + parsedRequest[it->second];
			_addVarToEnv(str.c_str());
		}
		++it;
	}
	return;
}

void	CgiHandler::_executeScript()
{
	std::string CGI_EXECUTOR = "/usr/bin/python";

	_args[0] = (char *)CGI_EXECUTOR.c_str();
	if (execve(_args[0], _args, _environment) < 0)
	{
		exit(EXIT_FAILURE);
	}
}

void	CgiHandler::_cgiDispatch()
{
	int pid = -1;
	int fd[2];
	
	if (pipe(fd) < 0)
		throw internalServerError();
	if ((pid = fork()) == -1)
		throw internalServerError();
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

void CgiHandler::handleCGI(int fd)
{
	try
	{
		_cgiDispatch();
	}
	catch (CgiHandler::internalServerError &e)
	{
		std::cerr << e.what() << '\n';
		Response resp(500, _currentServer);
		resp.addBody();
		resp.sendResponse(fd);
	}
	catch (CgiHandler::CgiError &e)
	{
		std::cerr << e.what() << '\n';
		Response resp(502, _currentServer);
		resp.addBody();
		resp.sendResponse(fd);
	}
}

CgiHandler::~CgiHandler(void)
{
	_freeEnv();
    return ;
}