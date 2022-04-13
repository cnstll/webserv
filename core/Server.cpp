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
// TODO: Check that field exist asa valid field | Check that field is not already saved to the map

void Server::parseLocationFields(const std::string& line){
	//std::cerr << "ENTERED PARSED LOCATION FIELDS\n";
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
			//std::cout << "FIELD: " << it->first << " VALUE: " << it->second << std::endl;
			break;
		}
		++it;
	}
	if (it == locationConfigFields[countOfLocationBlocks - 1].locationConfigFields.end())
		printErrorAndExit("ERROR: unknown field in location bloc - FaultyLine: \'" + line + "\'\n");

}

void Server::checkInstructionEOL(const std::string &line, bool hasLocationLineToken){
	size_t eol = (hasLocationLineToken ? line.find("{") : line.find(";"));
	if (line.substr(eol + 1).find_first_not_of("\n") != std::string::npos){
		printErrorAndExit("ERROR: wrong synthax for eol - FaultyLine: \'" + line + "\'\n");
	}
}

void Server::checkWhitespacesInInstructionLine(const std::string &line, bool hasLocationLineToken){
	//std::cerr << "ENTERED CHECK WITHESPACES\n";
	const std::string forbiddenWhiteSpaces = "\r\v\f";
	size_t startOfToken =0, endOfToken =0;
	size_t eol = (hasLocationLineToken ? line.find("{") : line.find(";"));
	if (line.find_first_of(forbiddenWhiteSpaces) != std::string::npos) // check for forbidden 
		printErrorAndExit("ERROR: unauthorized whitespaces found - FaultyLine: \'" + line + "\'\n");
	startOfToken = line.find_first_not_of(" ", endOfToken + 1);
	if (line.find_first_of(forbiddenWhiteSpaces + "\t", startOfToken) != std::string::npos) // check for forbidden whitespaces + \t after first token
		printErrorAndExit("ERROR: unauthorized whitespaces found - FaultyLine: \'" + line + "\'\n");
	while (startOfToken < eol && endOfToken < eol){
		//std::cerr << "EOL: " << eol << " START: " << startOfToken << " END: " << endOfToken << std::endl;
		if (endOfToken != 0 && startOfToken - endOfToken > 2)
			printErrorAndExit("ERROR: too many spaces within instruction - FaultyLine: \'" + line + "\'\n");
		endOfToken = line.find(" ", startOfToken) - 1;
		startOfToken = line.find_first_not_of(" ", endOfToken + 1);
	}
}

bool Server::lineHasLocationToken(const std::string &line){
	std::string locationToken = "location";
	if (line.find(locationToken) != std::string::npos)
		return true;
	else
		return false;
}

bool Server::isEmptyLine(const std::string &line){
	if (line.find_first_not_of(" ") == std::string::npos)
		return true;
	else
		return false;
}

void Server::parseLocationBloc(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine){
	addLocationBlocConfig();
	locationConfigFields[countOfLocationBlocks - 1].uriPath = findLocationPath(line);
	// std::cout << "LOCATION_PATH: " << locationConfigFields[countOfLocationBlocks - 1].uriPath << std::endl;
	size_t endOfLocationBloc = bloc.find("}", endOfLine + 1);
	//std::cerr << "ENTERED PARSELOCATIONBLOC\n";
	while (1)
	{
		startOfLine = bloc.find("\n", startOfLine) + 1;
		endOfLine = bloc.find("\n", startOfLine);
		if (endOfLine >= endOfLocationBloc)
			break;
		line = bloc.substr(startOfLine, endOfLine - startOfLine);
				// std::cerr << "LINE LENGTH: " << line.length() << std::endl;
		// std::cerr << "LINE CHECKED: " << line << std::endl;
		// std::cerr << "FIND RET FOR '{': " << line.find("{") << std::endl;
		// std::cerr << "FIND RET FOR ';': " << line.find(";") << std::endl;
		// std::cerr << "HAS LOCATION RET: " << lineHasLocationToken(line) << std::endl;
		if (isEmptyLine(line))
			continue;
		checkInstructionEOL(line, false);
		checkWhitespacesInInstructionLine(line, false);
		parseLocationFields(line);
	}
}

void Server::parseMainInstructionsFields(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine){
	size_t matchField;
	configMap::iterator it;
	it = serverConfigFields.begin();
	//std::cerr << "ENTERED Main INSTRUCTION PARSING\n";
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
}

void Server::parseConfig(const std::string &bloc){
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
		// std::cerr << "LINE LENGTH: " << line.length() << std::endl;
		// std::cerr << "LINE CHECKED: " << line << std::endl;
		// std::cerr << "FIND RET FOR '{': " << line.find("{") << std::endl;
		// std::cerr << "FIND RET FOR ';': " << line.find(";") << std::endl;
		// std::cerr << "EOB: " << endOfBloc << " SOL: " << startOfLine << " EOL: " << endOfLine << std::endl;
		// std::cerr << "CHAR EOB: " << bloc[endOfBloc] << " SOL: " << bloc[startOfLine] << " EOL: " << endOfLine << std::endl;
		// std::cerr << "HAS LOCATION RET: " << lineHasLocationToken(line) << std::endl;
		if (isEmptyLine(line))
			continue;
		checkWhitespacesInInstructionLine(line, lineHasLocationToken(line));
		checkInstructionEOL(line, lineHasLocationToken(line));
		if (lineHasLocationToken(line)){
			parseLocationBloc(bloc, line, startOfLine, endOfLine);
		} else {
			parseMainInstructionsFields(bloc, line, startOfLine, endOfLine);
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
		"upload_dir",
		"cgi",
		""
};

/* ************************************************************************** */