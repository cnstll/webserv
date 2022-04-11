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
		o << "TOTAL NUMBER OF LOCATION BLOCS: " << countOfLocationBlocks << std::endl;
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
			if (line[matchField] != ' '){
				std::cout << "CHAR: " << line[matchField] << std::endl;
				std::cerr << "ERROR: server fields should be separated by 1 whitespace\n";
				exit(EXIT_FAILURE);
			}
			it->second = std::string(line, matchField + 1,  line.length() - (matchField + 2));
			//std::cout << "FIELD: " << it->first << " VALUE: " << it->second << std::endl;
			break;
		}
		++it;
	}
}

void Server::parseConfig(const std::string &bloc){
	size_t startOfLine, endOfLine, endOfBloc;
	size_t matchField;
	size_t foundBrace;
	startOfLine = endOfLine = 0;
	std::string line;
	const std::string locationToken = "location";
	endOfBloc = bloc.length();
	//std::cout << "IN PARSING: \n" << bloc << std::endl;
	configMap::iterator it;
	while (endOfLine < endOfBloc){
		startOfLine = bloc.find("\n", startOfLine) + 1;
		endOfLine = bloc.find("\n", startOfLine);
		it = serverConfigFields.begin();
		line = bloc.substr(startOfLine, endOfLine - startOfLine);
		if (line.find(locationToken) != std::string::npos){
			addLocationBlocConfig();
			locationConfigFields[countOfLocationBlocks - 1].uriPath = findLocationPath(line);
			//std::cout << "LOCATION_PATH: " << locationConfigFields[countOfLocationBlocks - 1].uriPath << std::endl;
			size_t endOfLocationBloc = bloc.find("}", endOfLine + 1);
			while (1){
				startOfLine = bloc.find("\n", startOfLine) + 1;
				endOfLine = bloc.find("\n", startOfLine);
				if (endOfLine >= endOfLocationBloc)
					break;
				line = bloc.substr(startOfLine, endOfLine - startOfLine);
				if (line[line.length() - 1] != ';'){
					std::cerr << "ERROR: expected ';' at end of line\n";
					std::cerr << "CHAR: " << line[line.length() - 1] << std::endl;
					std::cerr << "LINE: " << line << std::endl;
					exit(EXIT_FAILURE);
				}
				parseLocationFields(line);
			}

		} else {
			while (it != serverConfigFields.end()){
				matchField = bloc.substr(startOfLine, endOfLine - startOfLine).find(it->first);
				if (matchField != std::string::npos){
					matchField += it->first.length();
					if (bloc[startOfLine + matchField] != ' '){
						std::cout << "CHAR: " << bloc[startOfLine + matchField] << std::endl;
						std::cerr << "ERROR: server fields should be separated by 1 whitespace\n";
						exit(EXIT_FAILURE);
					}
					it->second = std::string(bloc, startOfLine + matchField + 1, endOfLine - (startOfLine + matchField + 2));
					//std::cout << "FIELD: " << it->first << " VALUE: " << it->second << std::endl;
					break;
				}
				++it;
			}
		}
	}
};


/*
** --------------------------------- ACCESSOR ---------------------------------
*/
	Server::configMap Server::getServerConfig(void) const{
		return serverConfigFields;
	};

/*
** --------------------------------- STATIC ---------------------------------
*/
std::string Server::validServerFields[] = {

		"listen",
		"server_name",
		"root",
		"error_page",
		""
};

std::string Server::validLocationFields[] = {

		"index",
		"root",
		"methods",
		"autoindex",
		"client_max_body_size",
		""
};

/* ************************************************************************** */