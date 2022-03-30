#include "Request.hpp"
#include "utils.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <vector>
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Request::Request() : _fullRequest(), root_dir(ROOT_DIR), _requestParsingError(200){
	std::cout << "Request - Constructor called\n";
	initParsedRequestMap();

};

Request::Request(std::string fullRequest)  : _fullRequest(fullRequest), root_dir(ROOT_DIR), _requestParsingError(200)
{
	initParsedRequestMap();
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
// void Request::extractUri(void)
// {
// 	std::vector <std::string> tokens;

// 	std::stringstream ss(_fullRequest);
// 	std::string token;
// 	while (std::getline(ss, token, ' ')){
// 		tokens.push_back(token);
// 	}
// 	_uri = (tokens[1] == "/" ? root_dir + "/index.html" : root_dir + tokens[1]);
// 	for (int i = 0; i < tokens.size(); i++){
// 		std::cout << "\ntokens: " << tokens[i];
// 	}
// 	std::cout << "Extracted URI: " << _uri << std::endl;
//}

void Request::clear(){
	std::map<std::string, std::string>::iterator it = _parsedHttpRequest.begin();
	while (it != _parsedHttpRequest.end()){
		it->second = "";
		++it;
	}
	_fullRequest = std::string();
	_requestParsingError = 200;
}

/**
 * @brief Parse Request and fill in the parsedHttpRequest map
 * 
 */
int Request::parse(void){
	std::size_t head = 0;
	std::size_t tail = 0;
	std::vector<std::string> methods = {"GET", "POST", "DELETE"};
	tail = _fullRequest.find(' ', head);
	//std::cout << "Head: " << head << " - Tail: " << tail << std::endl;
	
	_parsedHttpRequest["method"] = std::string(_fullRequest, head, tail - head);
	int i = 0;
	while (i < 3){
		if (_parsedHttpRequest["method"].compare(methods[i]) == 0)
			break;
		i++;
	}
	if (i == 3){
		_requestParsingError = 501; //"Not implemented"
		return -1;
	}
	//check if method is valid {"GET", "POST, "DELETE"}
	head = tail + 1;
	tail = _fullRequest.find(' ', head);
	//std::cout << "Head: " << head << " - Tail: " << tail << std::endl;
	_parsedHttpRequest["requestURI"] = std::string(_fullRequest, head, tail - head);
	std::cout << "THIS IS YOUR requested URI: " << _parsedHttpRequest["requestURI"] << std::endl;
	//Check if path exists
	if (_parsedHttpRequest["requestURI"].compare("/") == 0){
		_parsedHttpRequest["requestURI"] = "/index.html";
	}
	if (!doesFileExist(getPathToFile())){
		_requestParsingError = 404; //"Not Found" 
		return -1;
	}
	head = tail + 1;
	tail = _fullRequest.find("\r\n", head);
	//std::cout << "Head: " << head << " - Tail: " << tail << std::endl;
	_parsedHttpRequest["httpVersion"] = std::string(_fullRequest, head, tail - head);
	//Check HTTP Version
	if (_parsedHttpRequest["httpVersion"].compare("HTTP/1.1") != 0){
		_requestParsingError = 505; // Not implemented
		return -1;
	}
	//Parse each line of the request header and entity header
	//When 2 CRLF are encountered the next bits will be message-body
	std::size_t _fullRequestLen = _fullRequest.length();
	std::size_t endOfHeader = _fullRequest.find("\r\n\r\n", tail);
	while (tail < _fullRequestLen && tail != endOfHeader){
		head = tail + 2;
		std::string field;
		std::string value;
		tail = _fullRequest.find(':', head);
		field = std::string(_fullRequest, head, tail - head);
		std::cout << "Field: " << field << "\n";
		head = tail + 2;
		tail = _fullRequest.find("\r\n", head);
		value = std::string(_fullRequest, head, tail - head);
		if (_parsedHttpRequest.count(field) == 0){
			//_requestParsingError = 400; //Bad Request
			//return -1;
			continue;
		}
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

void Request::printFullRequest(void){
	std::cout << "Full Request: " << "\n" << _fullRequest << "\n" << std::endl;
}

void Request::printFullParsedRequest(void){
	std::map<std::string, std::string>::iterator it = _parsedHttpRequest.begin();
	while (it != _parsedHttpRequest.end()){
		std::cout << "Key stored: " << it->first << " - Value stored: '" << it->second << "'" <<  std::endl;
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

std::string Request::getHttpMethod(void){
	return _parsedHttpRequest["method"];
}

uint16_t Request::getError(void) const {
	return _requestParsingError;
}

std::map<std::string, std::string> &Request::getParsedRequest(void){
	return _parsedHttpRequest;
}

/* ************************************************************************** */
void Request::initParsedRequestMap(){
		int i = 0;
	//fill map with all possible fields request and an empty string
	while (Request::_validRequestFields[i] != ""){
		_parsedHttpRequest[Request::_validRequestFields[i]] = std::string();
		i++;
	}
};

std::string Request::_validRequestFields[] = {

//Request line
		"method",
		"httpVersion",
		"requestURI",
		"Cache-Control",
    "Connection",
    "Date",
    "Pragma",
    "Trailer",
    "Transfer-Encoding",
    "Upgrade",
    "Via",
    "Warning",
//request-header
    "Accept",
    "Accept-Charset",
    "Accept-Encoding",
    "Accept-Language",
    "Authorization",
    "Expect",
    "From",
    "Host",
    "If-Match",
    "If-Modified-Sinc",
    "If-None-Match",
    "If-Range",
    "If-Unmodified-Si",
    "Max-Forwards",
    "Proxy-Authorizat",
    "Range",
    "Referer",
    "TE",
    "User-Agent",

//entity-header
    "Allow",
    "Content-Encoding",
    "Content-Language",
    "Content-Length",
    "Content-Location",
    "Content-MD5",
    "Content-Range",
    "Content-Type",
    "Expires",
    "Last-Modified",
    "extension-header",
		""
};