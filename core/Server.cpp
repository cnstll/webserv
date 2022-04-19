#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
	initServerConfig();
}

Server::Location::Location(){}

// Server::Server( const Server & src )
// {
	// (void)src;
// }


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server(){}
Server::Location::~Location(){}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

// Server &				Server::operator=( Server const & rhs )
// {
	// (void)rhs;
	// return *this;
// }

std::ostream &			operator<<( std::ostream & o, Server const & i )
{
	i.displayServerConfig(o);
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/
void Server::displayServerConfig(std::ostream &o) const{
  configMap::const_iterator itServ = serverConfigFields.begin();
  std::map<std::string, Location>::const_iterator itLoc = locationBlocs.begin();
	size_t i = 0;
	o << "BEGINNING OF SERVER CONFIG---------------------------------\n";
	while(itServ != serverConfigFields.end()){
		if (itServ->second != "")
    	o << "Field: " << itServ->first << " - Value: " << itServ->second << std::endl;
    ++itServ;
  }
	if (countOfLocationBlocks)
		o << "\nTOTAL NUMBER OF LOCATION BLOCS: " << countOfLocationBlocks;
	while (itLoc != locationBlocs.end()){
		o << "\nLOCATION FIELD no " << i << std::endl;
		o << "Uri path attached to location: " << itLoc->first << std::endl;
		configMap::const_iterator itLocFields = itLoc->second.fields.begin();
		while(itLocFields != itLoc->second.fields.end()){
			if (itLocFields->second != "")
  	  	o << "Field: " << itLocFields->first << " - Value: " << itLocFields->second << std::endl;
  	  ++itLocFields;
  	}
		++itLoc;
	}

  o << "---------------------------------------END OF SERVER CONFIG\n";
}

int Server::isRequestDone(Request &request, int &contentLength, int &startOfBody)
{
  if (request.getParsedRequest()["Transfer-Encoding"] == "chunked")
  {
    if (request.getFullRequest().find("\r\n0\r\n") != std::string::npos)
      return 1;
    else
      return 0;
  }
  int lengthRecvd = request.getFullRequest().length() - startOfBody;
  if (contentLength <= lengthRecvd)
    return (1);
  return 0;
}

int Server::parseHeader(Request &request)
{
  if (request.getFullRequest().find("\r\n\r\n") != std::string::npos)
  {
    request.parseHeader(*this);
    if (request.getHttpMethod() == "POST")
      return (1);
    else
      return 2;
  }
  return 0;
}

int check2(int return_value, std::string const &error_msg)
{
  if (return_value < 0)
  {
    std::cerr << error_msg << std::endl;
    // exit(EXIT_FAILURE);
    return -1;
  }
  return 1;
}

void Server::makeFdNonBlocking(int &fd)
{
  int flags;
  check2((flags = fcntl(fd, F_GETFL, NULL)), "flags error");
  flags |= O_NONBLOCK;
  check2((fcntl(fd, F_SETFL, flags)), "fcntl error");
}

int Server::acceptNewConnexion(int server_fd)
{
  socklen_t addr_in_len = sizeof(struct sockaddr_in);
  struct sockaddr_in connexion_address;
  int connexionFd;

  if (check2(connexionFd = accept(server_fd, (struct sockaddr *)&connexion_address, &addr_in_len), "failed accept"))
  {
	  makeFdNonBlocking(connexionFd);
	  requestMap[connexionFd] = new Request("", *this);
	  return connexionFd;
  }
  return -1;
}

std::string Server::getExtension(std::string &uri)
{
	if (uri != "")
	{
		size_t pos = uri.find_last_of(".");
		if (pos != std::string::npos)
			return (uri.substr(pos));
	}
	return (uri);
}

void Server::closeConnection(int fd)
{
	close(fd);
	requestMap.erase(fd);
}

void Server::respond(int fd)
{
	std::string cgiExtension = ".py";
	_currentRequest = requestMap[fd];
	std::string extension = _currentRequest->getRequestedUri();
	std::string uri = _currentRequest->getRequestedUri();
	std::string fullPath = constructPath(uri);
	if (getExtension(extension) == cgiExtension && _currentRequest->getError() <= 300)
	{
		cgiHandler cgiParams(_currentRequest->getParsedRequest(), fullPath, fd, *this);
		cgiParams.handleCGI(fd);
	}
	else if (_currentRequest->getHttpMethod() == "DELETE")
	{
		if (std::remove(fullPath.c_str()) != 0)
		{
			Response resp(_currentRequest->getParsedRequest(), 204, *this);
			resp.sendResponse(fd);
		}
	}
	else
	{
		Response resp(_currentRequest->getParsedRequest(), _currentRequest->getError(), *this);
		resp.addBody(fullPath, this);
		resp.sendResponse(fd);
	}
	if (_currentRequest->getParsedRequest()["Connection"] != "keep-alive")
		closeConnection(fd);
	else
		_currentRequest->clear();

}

int Server::setupServer(int port, int backlog)
{
  struct sockaddr_in server_addr;
  int serverFd;

  check2((serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)), "socket error");
  int yes = 1;
  if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
  {
    perror("setsockopt");
    exit(1);
  }
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  check2(bind(serverFd, (struct sockaddr *)&server_addr, sizeof(server_addr)), "bind error");
  check2(listen(serverFd, backlog), "listen error");
  return serverFd;
}

int Server::recvRequest(const int &fd, Request &request){
  int read_bytes;
  char request_buffer[REQUEST_READ_SIZE + 1] = {};
  int parsed;
  int contentSize;
  int startOfBody;

//   bzero(&request_buffer, REQUEST_READ_SIZE + 1);
  while ((read_bytes = recv(fd, &request_buffer, REQUEST_READ_SIZE, 0)) > 0)
  {
    request.append(request_buffer, read_bytes);
    if (!request.headerParsed)
    {
      parsed = parseHeader(request)	;
      if (parsed)
	  {
		  request.headerParsed = 1;
		  if (parsed == 2)
		  {
			  request.headerParsed = 0;
			  return (read_bytes);
		  }
		  else
		  {
			  startOfBody = request.getFullRequest().find("\r\n\r\n") + 4;
			  contentSize = stringToNumber((request.getParsedRequest()["Content-Length"]));
			  if (contentSize > stringToNumber(getLocationField(request.getParsedRequest()["requestURI"], "client_max_body_size")) && getLocationField(request.getParsedRequest()["requestURI"], "client_max_body_size") != "")
			  {
				  Response resp(413, *this);
				  resp.sendResponse(fd);
				  closeConnection(fd);
				  request.headerParsed = 0;
				  return (-1);
			  }
		  }
	  }
	  else if (parsed == -1) {
		  return (1);
	  }
	}
  }
  if (isRequestDone(request, contentSize, startOfBody))
  {
	  request.headerParsed = 0;
	  return (1);
  }
  if (read_bytes == 0)
  {
	  printf("Closing connexion for fd: %d\n", fd);
	  closeConnection(fd);
	//   return (-1);
  }
  return read_bytes;
}

// void Server::closeConnection() {

// }

void Server::initServerConfig(){
	int i = 0;
	//fill map with all possible allowed fields in server
	while (Server::validServerFields[i] != ""){
		serverConfigFields[Server::validServerFields[i]] = std::string();
		i++;
	}
	countOfLocationBlocks = 0;
}

void Server::addLocationBlocConfig(std::string &uri){
	int i = 0;
	Location newLocation;
	//fill map with all possible allowed fields in server
	while (Server::validLocationFields[i] != ""){
		newLocation.fields[Server::validLocationFields[i]] = std::string();
		i++;
	}
	locationBlocs[uri] = newLocation;
	++countOfLocationBlocks;
}

bool Server::lineHasLocationToken(const std::string &line){
	std::string locationToken = "location /";
	return (line.find(locationToken) != std::string::npos);
}

bool Server::isEmptyLine(const std::string &line){
	return (line.find_first_not_of(" \t") == std::string::npos);
}

bool Server::isEndOfBloc(const std::string &line){
	size_t pos = line.find_first_not_of(" \t");
	if (pos == std::string::npos)
		return false;
	else if (line.at(pos) == '}')
		return true;
	else
		return false;
}

std::string Server::findLocationPath(const std::string &line){
	std::string locationPath;
	const std::string locationToken = "location";
	size_t startOfLocationToken = line.find(locationToken);
	size_t c = startOfLocationToken + locationToken.length();
	size_t startOfPath = line.find("/", startOfLocationToken);
	if (startOfPath == std::string::npos){
		std::cerr << "ERROR: No path found for location bloc\n";
		exit(EXIT_FAILURE);
	}
	if (startOfPath - c != 1 || line.substr(startOfLocationToken, startOfPath - startOfLocationToken).find_first_not_of(" ")){
		std::cerr << "ERROR: synthax error in location bloc\n";
		exit(EXIT_FAILURE);
	}
	c = line.find_first_of(" {", startOfPath);
	locationPath = line.substr(startOfPath, c - startOfPath);
	return locationPath;
}

void Server::checkMethodsInLocation(std::string &values, const std::string &line){
	
	std::vector<std::string> v;
	std::vector<std::string>::iterator itVec;
	int i;
	v = tokenizeValues(values);
	itVec = v.begin();
	while (itVec != v.end())
	{
		i = 0;
		while (implementedMethods[i] != "")
		{
			if (*itVec == implementedMethods[i])
				break;
			++i;
		}
		if (i == 3)
			printErrorAndExit("ERROR: unknown method - FaultyLine: \'" + line + "\'\n");
		++itVec;
	}
}

void Server::parseLocationFields(const std::string& line, const std::string &uri){
	size_t matchField;
	configMap::iterator it = locationBlocs[uri].fields.begin();
	while (it != locationBlocs[uri].fields.end()){
		matchField = line.find(it->first);
		if (matchField != std::string::npos){
			matchField += it->first.length();
			if (it->second != "")
				printErrorAndExit("ERROR: field already exists in location bloc - FaultyLine: \'" + line + "\'\n");
			it->second = std::string(line, matchField + 1,  line.length() - (matchField + 2));
			if (it->first == "methods")
				checkMethodsInLocation(it->second, line);
			break;
		}
		++it;
	}
	if (it == locationBlocs[uri].fields.end())
	 	printErrorAndExit("ERROR: unknown field in location bloc - FaultyLine: \'" + line + "\'\n");
}

void Server::parseLocationBloc(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine){
	std::string uri = findLocationPath(line);
	addLocationBlocConfig(uri);
	size_t endOfLocationBloc = bloc.find("}", endOfLine + 1);
	while (1)
	{
		startOfLine = bloc.find("\n", startOfLine) + 1;
		endOfLine = bloc.find("\n", startOfLine);
		if (endOfLine >= endOfLocationBloc)
			break;
		line = bloc.substr(startOfLine, endOfLine - startOfLine);
		if (isEmptyLine(line))
			continue;
		parseLocationFields(line, uri);
	}
}

void Server::parseMainInstructionsFields(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine){
	size_t matchField;
	configMap::iterator it;
	it = serverConfigFields.begin();
	while (it != serverConfigFields.end()){
		matchField = bloc.substr(startOfLine, endOfLine - startOfLine).find(it->first);
		if (matchField != std::string::npos){
			matchField += it->first.length();
			if (it->second != "")
				printErrorAndExit("ERROR: field already exists in main server bloc - FaultyLine: \'" + line + "\'\n");
			it->second = std::string(bloc, startOfLine + matchField + 1, endOfLine - (startOfLine + matchField + 2));
			break;
		}
		++it;
	}
	if (it == serverConfigFields.end())
		printErrorAndExit("ERROR: unknown field in server bloc - FaultyLine: \'" + line + "\'\n");
}

std::string Server::constructPath(std::string &uri) {
	std::string fullPath;
	std::map<std::string, Location>::iterator it;
	it = locationBlocs.find(uri);
	if (it == locationBlocs.end())
		return serverConfigFields["root"] + uri;
	else
		return it->second.fields["root"] + uri;
}

void Server::parseServerConfigFields(const std::string &bloc){
	size_t startOfLine, endOfLine, endOfBloc;
	std::string line;
	std::string lineNumber;
	startOfLine = endOfLine = 0;
	endOfBloc = bloc.length();
	while (endOfLine <= endOfBloc){
		startOfLine = bloc.find("\n", startOfLine) + 1;
		endOfLine = bloc.find("\n", startOfLine);
		if (endOfLine == std::string::npos)
			break;
		line = bloc.substr(startOfLine, endOfLine - startOfLine);
		if (isEmptyLine(line) || isEndOfBloc(line))
			continue;
		if (lineHasLocationToken(line)){
			parseLocationBloc(bloc, line, startOfLine, endOfLine);
		} else {
			parseMainInstructionsFields(bloc, line, startOfLine, endOfLine);
		}
	}
	parsePort();
};

void Server::parsePort(void){
	std::cerr << "LISTEN: " << serverConfigFields["listen"] << std::endl;
	size_t findport = serverConfigFields["listen"].find(":");
	if (findport == std::string::npos)
		printErrorAndExit("ERROR: bad synthax for listen instruction\n");
	serverPort = stringToNumber(serverConfigFields["listen"].substr(findport+1));
	if (!serverPort)
		printErrorAndExit("ERROR: invalid server port");
}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/
Server::configMap Server::getServerConfig(void) const{
	return serverConfigFields;
};
int Server::getServerPort(void) const {
	return serverPort;
}
/**
 * @brief Retrieve a field in a specific location bloc
 * 
 * @param locationUri path directly following the location token used to identify the correct bloc
 * @param requestedField field to find in a location bloc
 * @return std::string either an empty string if the field is not found, or the corresponding string value
 */
std::string Server::getLocationField(const std::string &locationUri, const std::string &requestedField)
{
	std::map<std::string, Location>::iterator it = locationBlocs.find(locationUri);
	configMap::iterator itLoc;
	if (it == locationBlocs.end())
		return std::string("");
	else
	{
		itLoc = it->second.fields.find(requestedField);
		return (itLoc == it->second.fields.end() ? std::string("") : it->second.fields[requestedField]);
	}
}
std::string Server::getServerConfigField(const std::string &requestedField)
{
	configMap::iterator it;
	it = serverConfigFields.find(requestedField);
	return (it == serverConfigFields.end() ? std::string("") : serverConfigFields[requestedField]);
}

std::string *Server::getImplementedMethods(void){
	return implementedMethods;
}

/*
** --------------------------------- STATIC ---------------------------------
*/
std::string Server::validServerFields[] = {

		"index",
		"listen",
		"server_name",
		"root",
		"error_pages_dir",
		""
};

std::string Server::validLocationFields[] = {

		"root",
		"methods",
		"autoindex",
		"client_max_body_size",
		"upload_dir",
		"return",
		""
};

std::string Server::implementedMethods[] = {
	"GET",
	"POST",
	"DELETE",
	""
};

/* ************************************************************************** */