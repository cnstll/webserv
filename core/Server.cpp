#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
	initServerConfig();
}

// Server::Server( const Server & src )
// {
	// (void)src;
// }


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}


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
  std::map<std::string, std::string>::const_iterator it = config.begin();
  o << "BEGINNING OF SERVER CONFIG---------------------------------\n";
	while(it != config.end()){
    o << "Field: " << it->first << " - Value: " << it->second << std::endl;
    ++it;
  }
  o << "---------------------------------------END OF SERVER CONFIG\n";
}

void Server::initServerConfig(){
	int i = 0;
	//fill map with all possible allowed fields in server
	while (Server::validServerFields[i] != ""){
		config[Server::validServerFields[i]] = std::string();
		i++;
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/
	Server::configMap Server::getServerConfig(void) const{
		return config;
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