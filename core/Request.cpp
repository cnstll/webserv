#include "Request.hpp"
#include <vector>
#include <string>
#include <sstream>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Request::Request() : _fullRequest(), _uri("server_root/index.html"){
	std::cout << "Request - Constructor called\n";
};

Request::Request(std::string fullRequest)  : _fullRequest(fullRequest)
{
	extractUri();
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
void Request::extractUri(void)
{
	std::string root_dir(ROOT_DIR);
	std::vector <std::string> tokens;

	std::stringstream ss(_fullRequest);
	std::string token;
	while (std::getline(ss, token, ' ')){
		tokens.push_back(token);
	}
	_uri = (tokens[1] == "/" ? root_dir + "/index.html" : root_dir + tokens[1]);
	for (int i = 0; i < tokens.size(); i++){
		std::cout << "\ntokens: " << tokens[i];
	}
	std::cout << "Extracted URI: " << _uri << std::endl;
}

/**
 * @brief Parse Request and fill in Request member variables
 * 
 */
void Request::parse(void){
	// std::string root_dir(ROOT_DIR);
	// std::vector <std::string> tokens;

	// std::stringstream ss(_fullRequest);
	// std::string token;
	// while (std::getline(ss, token, '\n')){
	// 	tokens.push_back(token);
	// }
	// _uri = (tokens[1] == "/" ? root_dir + "/index.html" : root_dir + tokens[1]);
	// for (int i = 0; i < tokens.size(); i++){
	// 	std::cout << "\ntokens: " << tokens[i];
	// }
	// std::cout << "Extracted URI: " << _uri << std::endl;
	extractUri();
}

/**
 * @brief Append str to the _fullRequest string
 * 
 * @param str c string appended to _fullRequest
 */
void Request::append(char *str){
	_fullRequest.append(str);
}

/**
 * @brief Clearing all members in a request object
 * 
 */
void Request::clear(void){
	_fullRequest.clear();
	_uri.clear();
}

void Request::printFullRequest(void){
	std::cout << "Full Request: " << "\n" << _fullRequest << "\n" << std::endl;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string Request::getRequestedUri(){
	return _uri;
}

/* ************************************************************************** */