#include "Request.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iterator>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Request::Request() : _fullRequest(), root_dir(ROOT_DIR){
	std::cout << "Request - Constructor called\n";
};

Request::Request(std::string fullRequest)  : _fullRequest(fullRequest), root_dir(ROOT_DIR)
{
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
int Request::parse(void){
	std::size_t head = 0;
	std::size_t tail = 0;
	tail = _fullRequest.find(' ', head);
	//std::cout << "Head: " << head << " - Tail: " << tail << std::endl;
	_parsedHttpRequest["method"] = std::string(_fullRequest, head, tail - head);
	//check if method is valid {"GET", "POST, "DELETE"}
	head = tail + 1;
	tail = _fullRequest.find(' ', head);
	//std::cout << "Head: " << head << " - Tail: " << tail << std::endl;
	_parsedHttpRequest["requestURI"] = std::string(_fullRequest, head, tail - head);
	//Check if path exists
	head = tail + 1;
	tail = _fullRequest.find("\r\n", head);
	//std::cout << "Head: " << head << " - Tail: " << tail << std::endl;
	_parsedHttpRequest["httpVersion"] = std::string(_fullRequest, head, tail - head);
	//Check HTTP Version
	//Parse each line of the request header and entity header
	//When 2 CRLF are encountered the next bits will be message-body
	std::size_t _fullRequestLen = _fullRequest.length();
	std::size_t endOfHeader = _fullRequest.find("\r\n\r\n", tail);
	while (tail < _fullRequestLen && tail != endOfHeader){
		head = tail + 2;
		std::string field;
		std::string value;
		tail = _fullRequest.find(' ', head);
		field = std::string(_fullRequest, head, tail - head);
		head = tail + 1;
		tail = _fullRequest.find("\r\n", head);
		value = std::string(_fullRequest, head, tail - head);
		_parsedHttpRequest[field] = value;
	}
	//Next come msg-body
	//! parse body - which cases ? form ? Uploaded files ?
	return 0;
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
	_parsedHttpRequest.clear();
}

void Request::printFullRequest(void){
	std::cout << "Full Request: " << "\n" << _fullRequest << "\n" << std::endl;
}

void Request::printFullParsedRequest(void){
	std::map<std::string, std::string>::iterator it = _parsedHttpRequest.begin();
	while (it != _parsedHttpRequest.end()){
		std::cout << "Key stored: " << it->first << " - Value stored: " << it->second << std::endl;
		it++;
	}
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string Request::getRequestedUri(){
	return _parsedHttpRequest["requestURI"];
}

std::string Request::getPathToFile(void){
	return "." + root_dir + _parsedHttpRequest["requestURI"];
}

/* ************************************************************************** */