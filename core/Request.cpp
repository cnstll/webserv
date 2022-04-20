#include "Request.hpp"
#include "../utils/utils.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <vector>
#include <fstream>
#include <algorithm>
# include <dirent.h>
# include <ctime>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::string fullRequest, Server &serv) :
_fullRequest(fullRequest), _currentServer(serv), _requestParsingError(200)
{
	initParsedRequestMap();
	_birth = time(0);
	_inactiveTime = 0;
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


/*
** --------------------------------- METHODS ----------------------------------
*/

void Request::clear(){
	std::map<std::string, std::string>::iterator it = _parsedHttpRequest.begin();
	while (it != _parsedHttpRequest.end()){
		it->second = std::string();
		++it;
	}
	_fullRequest = std::string();
	_requestParsingError = 200;
}

// bool isADir(std::string directoryPath)
// {  
//   DIR *dh;
  
//   dh = opendir (directoryPath.c_str());
  
//   if ( !dh )
//     return 0;
//   closedir ( dh );
//   return 1;
// }
int Request::checkMethodInLocationBloc(){
	std::string localMethods = _currentServer.getLocationField(_parsedHttpRequest["requestURI"], "methods");
	if (localMethods != ""){
		if (!strIsInVector(_parsedHttpRequest["method"], tokenizeValues(localMethods))){
			std::cout << "R METHOD: " << _parsedHttpRequest["method"] << std::endl;
			std::cout << "METHODS TOK: " << tokenizeValues(localMethods)[0] << std::endl;
			_requestParsingError = 405; // Method Not Allowed
			return -1;
		}
	}
	return 0;
}

int Request::checkIfMethodIsImplemented(Server &server){
	
	std::string *methods = server.getImplementedMethods();
	int i = 0;
	while (methods[i] != ""){
		if (methods[i] == _parsedHttpRequest["method"])
			return 0;
		++i;
	}
	if (i == 3)
		_requestParsingError = 501; // Method Not Implemented
	return -1;
}

void Request::TrimQueryString(size_t endOfURI){
		std::size_t queryPos = _parsedHttpRequest["requestURI"].find("?", 0);
	if (queryPos != std::string::npos){
		_parsedHttpRequest["queryString"] = std::string(_parsedHttpRequest["requestURI"], queryPos + 1, endOfURI - (queryPos + 1));
		_parsedHttpRequest["requestURI"] = std::string(_parsedHttpRequest["requestURI"], 0, queryPos);
	}
}
/**
 * @brief Parse Request and fill in the parsedHttpRequest map
 * 
 */
int Request::parseHeader(Server &server){
	std::size_t head = 0;
	std::size_t tail = 0;
	//check if method is valid {"GET", "POST, "DELETE"}
	tail = _fullRequest.find(' ', head);
	_parsedHttpRequest["method"] = std::string(_fullRequest, head, tail - head);
	if (checkIfMethodIsImplemented(server) < 0)
		return -1;
	head = tail + 1;
	tail = _fullRequest.find(' ', head);
	_parsedHttpRequest["requestURI"] = std::string(_fullRequest, head, tail - head);
	TrimQueryString(tail);
	if (_parsedHttpRequest["requestURI"].compare("/") == 0){
		_parsedHttpRequest["requestURI"] = "/index.html";
	}
	if (_parsedHttpRequest["requestURI"].size() > URI_MAX_LEN){
		_requestParsingError = 414; //"URI Too long"
		return -1;
	}
	if (_parsedHttpRequest["requestURI"] == "/makeCoffee"){
		_requestParsingError = 418; // "I am a tea pot"
		return -1;
	}
	if (_parsedHttpRequest["requestURI"] == "/censoredContent"){
		_requestParsingError = 451; // "Legal Reason"
		return -1;
	}
	// check if method is valid for the location bloc attached to the uri of the request
	checkMethodInLocationBloc();
	//! Here is we should check for redirects, before checking if the file exitsts as that won't be true if the file moved
	if (server.getLocationField(getRequestedUri(), "return") != "")
	{
		std::string redirectStr = server.getLocationField(getRequestedUri(), "return");
		std::vector<std::string> v = tokenizeValues(redirectStr);
		_requestParsingError = stringToNumber(v[0]);
		return -1;
	}
	if (!doesFileExist(server.constructPath(_parsedHttpRequest["requestURI"])))
	{
		_requestParsingError = 404; //"Not Found" 
		return -1;
	}
	head = tail + 1;
	tail = _fullRequest.find("\r\n", head);
	_parsedHttpRequest["httpVersion"] = std::string(_fullRequest, head, tail - head);
	//Check HTTP Version
	if (_parsedHttpRequest["httpVersion"].compare("HTTP/1.1") != 0){
		_requestParsingError = 505; // std::vector<std::string>::iterator it
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
		head = tail + 2;
		tail = _fullRequest.find("\r\n", head);
		value = std::string(_fullRequest, head, tail - head);
		if (_parsedHttpRequest.count(field) == 1)
			_parsedHttpRequest[field] = value;
	}
	if (getRequestField("method") == "POST" && getRequestField("Content-Length") == "" && getRequestField("Transfer-Encoding") != "chunked")
	{
		_requestParsingError = 411;
		return -1;
	}
	return 0;
}


int Request::parseBody(void){

		size_t tail =0, head =0;
		std::size_t endOfHeader = _fullRequest.find("\r\n\r\n", 0);
		if (endOfHeader == std::string::npos)
			return 0;
		head = endOfHeader + 4;
		tail = _fullRequest.length();
		if (_parsedHttpRequest["Transfer-Encoding"] == "chunked")
			_parsedHttpRequest["message-body"] = unchunckedRequest(head);	
		else
			_parsedHttpRequest["message-body"] = std::string(_fullRequest, head, tail - head);
		return 0;
}

bool Request::timeout(void)
{
	_inactiveTime = std::difftime(time(0), _birth);
	return _inactiveTime > 3;
}

std::string Request::unchunckedRequest(int startOfBody) 
{
  std::size_t head = 0;
	std::size_t tail = 0;
  std::string finalBody;
  std::string ogBody;
  std::string sizeStr;
  size_t chunckSize = 1;

  ogBody = _fullRequest.substr(startOfBody);
  while (chunckSize)
  {
    head = ogBody.find("\r\n", tail);
    chunckSize = strtol(ogBody.substr(tail, head).c_str(), NULL, 16);
    if (!chunckSize)
      return finalBody;
    head += 2;
    finalBody.append(ogBody, head, chunckSize);
    tail = head + chunckSize + 2;
  }
  return finalBody;
}

/**
 * @brief Append str to the _fullRequest string
 * 
 * @param str c string appended to _fullRequest
 */
void Request::append(char *str, std::size_t readBytes){
	_fullRequest.append(str, readBytes);
}

void Request::addFdInfo(int fd){
	this->fd = fd;
}


void Request::printFullRequest(void){
	std::cout << "\nBEGINNING OF FULL REQUEST -----------------\n";
	std::cout << _fullRequest << std::endl;
	std::cout << "-----------------------------END FULL REQUEST\n";
}

void Request::printFullParsedRequest(void){
	std::cout << "\nBEGINNING OF FULL PARSED REQUEST -----------------\n";
	std::map<std::string, std::string>::iterator it = _parsedHttpRequest.begin();
	while (it != _parsedHttpRequest.end()){
		std::cout << "Key stored: " << it->first << " - Value stored: '" << it->second << "'" <<  std::endl;
		it++;
	}
	std::cout << "-----------------------------END FULL PARSED REQUEST\n";
}



void Request::writeFullRequestToFile(const char *filename){
	std::ofstream ofs(filename, std::ios_base::app);
	ofs << _fullRequest;
  ofs.close();
};

void Request::writeStrToFile(const std::string &str, const char *filename){
	std::ofstream ofs(filename);
	ofs << str;
  ofs.close();
};


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string Request::getRequestField(const std::string &requestedField)
{
	std::map<std::string, std::string>::iterator it;
	it = _parsedHttpRequest.find(requestedField);
	return (it == _parsedHttpRequest.end() ? std::string("") : _parsedHttpRequest[requestedField]);
}

std::string Request::getRequestedUri(){
	return _parsedHttpRequest["requestURI"];
}

std::string Request::getHttpMethod(void){
	return _parsedHttpRequest["method"];
}

int Request::getError(void) const {
	return _requestParsingError;
}

std::map<std::string, std::string> &Request::getParsedRequest(void){
	return _parsedHttpRequest;
}

void Request::setErrorCode(int code){
	_requestParsingError = code;
}

int Request::getErrorCode(){

	return _requestParsingError;
}

std::string Request::getFullRequest(void){
	return _fullRequest;
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
		"queryString",
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
//message-body
		"message-body",
		""
};