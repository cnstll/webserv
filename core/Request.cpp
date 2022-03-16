#include "Request.hpp"
#include <vector>
#include <sstream>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::string fullRequest)  : _fullRequest(fullRequest)
{
	_uri = extractUri();
}

Request::Request( const Request & src )
{
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Request &				Request::operator=( Request const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Request const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/
std::string Request::extractUri(void)
{
	std::string uri;
	std::vector <std::string> tokens;

	std::stringstream ss(_fullRequest);
	std::string token;
	while (std::getline(ss, token, ' ')){
		tokens.push_back(token);
	}
	uri = tokens[1];
	std::cout << "uri: " << uri;
	return uri;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string Request::getRequestedUri(){
	return _uri;
}

/* ************************************************************************** */