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
  std::map<std::string, std::string>::const_iterator it = serverConfigFields.begin();
  size_t i = 0;
	o << "BEGINNING OF SERVER CONFIG---------------------------------\n";
	while(it != serverConfigFields.end()){
		if (it->second != "")
    	o << "Field: " << it->first << " - Value: " << it->second << std::endl;
    ++it;
  }
	if (countOfLocationBlocks)
		o << "\nTOTAL NUMBER OF LOCATION BLOCS: " << countOfLocationBlocks;
	while (i < countOfLocationBlocks){
		o << "\nLOCATION FIELD no " << i << std::endl;
		o << "Uri path attached to location: " << locationConfigFields[i].uriPath << std::endl;
		it = locationConfigFields[i].locationConfigFields.begin();
		while(it != locationConfigFields[i].locationConfigFields.end()){
			if (it->second != "")
  	  	o << "Field: " << it->first << " - Value: " << it->second << std::endl;
  	  ++it;
  	}
		++i;
	}

  o << "---------------------------------------END OF SERVER CONFIG\n";
}

void Server::initServerConfig(){
	int i = 0;
	//fill map with all possible allowed fields in server
	while (Server::validServerFields[i] != ""){
		serverConfigFields[Server::validServerFields[i]] = std::string();
		i++;
	}
	countOfLocationBlocks = 0;
}

void Server::addLocationBlocConfig(){
	int i = 0;
	Location newLocation;
	//fill map with all possible allowed fields in server
	while (Server::validLocationFields[i] != ""){
		newLocation.locationConfigFields[Server::validLocationFields[i]] = std::string();
		i++;
	}
	locationConfigFields.push_back(newLocation);
	++countOfLocationBlocks;
}

bool Server::lineHasLocationToken(const std::string &line){
	std::string locationToken = "location /";
	return (line.find(locationToken) != std::string::npos);
}

bool Server::isEmptyLine(const std::string &line){
	return (line.find_first_not_of(" \t") == std::string::npos);
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

void Server::parseLocationFields(const std::string& line){
	configMap::iterator it;
	size_t matchField;
	it =  locationConfigFields[countOfLocationBlocks - 1].locationConfigFields.begin();
	while (it != locationConfigFields[countOfLocationBlocks - 1].locationConfigFields.end()){
		matchField = line.find(it->first);
		if (matchField != std::string::npos){
			matchField += it->first.length();
			if (it->second != "")
				printErrorAndExit("ERROR: field already exists in location bloc - FaultyLine: \'" + line + "\'\n");
			it->second = std::string(line, matchField + 1,  line.length() - (matchField + 2));
			break;
		}
		++it;
	}
	// if (it == locationConfigFields[countOfLocationBlocks - 1].locationConfigFields.end())
	// 	printErrorAndExit("ERROR: unknown field in location bloc - FaultyLine: \'" + line + "\'\n");

}
void Server::parseLocationBloc(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine){
	addLocationBlocConfig();
	locationConfigFields[countOfLocationBlocks - 1].uriPath = findLocationPath(line);
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
		parseLocationFields(line);
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
	// if (it == serverConfigFields.end())
	// 	printErrorAndExit("ERROR: unknown field in location bloc - FaultyLine: \'" + line + "\'\n");
}

std::string Server::constructPath(std::string &uri) {
	std::string fullPath;
	// configMap::iterator iter = serverConfigFields.begin();
	fullPath = serverConfigFields["root"] + uri;
	return fullPath;
	// while (iter != serverConfigFields.end()) 
	// {
	// 	iter->first["root"]
	// 	++iter;
	// }

	
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
		if (isEmptyLine(line))
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

/*
** --------------------------------- STATIC ---------------------------------
*/
std::string Server::validServerFields[] = {

		"index",
		"listen",
		"server_name",
		"root",
		"error_page",
		""
};

std::string Server::validLocationFields[] = {

		"root",
		"methods",
		"autoindex",
		"client_max_body_size",
		"upload_dir",
		""
};

/* ************************************************************************** */