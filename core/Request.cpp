#include "Request.hpp"
#include "utils.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <vector>
#include <fstream>
#include <algorithm>
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Request::Request() : _fullRequest(), root_dir(ROOT_DIR), _requestParsingError(200){
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

void Request::clear(){
	std::map<std::string, std::string>::iterator it = _parsedHttpRequest.begin();
	while (it != _parsedHttpRequest.end()){
		it->second = std::string();
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
	// printFullRequest();
	//check if method is valid {"GET", "POST, "DELETE"}
	_parsedHttpRequest["method"] = std::string(_fullRequest, head, tail - head);
	std::vector<std::string>::iterator it;
	it = std::find(methods.begin(), methods.end(), _parsedHttpRequest["method"]);
	if (it == methods.end()){
		_requestParsingError = 405; // Method Not implemented
		return -1;
	}
	head = tail + 1;
	tail = _fullRequest.find(' ', head);
	_parsedHttpRequest["requestURI"] = std::string(_fullRequest, head, tail - head);
	std::size_t queryPos = _parsedHttpRequest["requestURI"].find("?", 0);
	if (queryPos != std::string::npos){
		_parsedHttpRequest["queryString"] = std::string(_parsedHttpRequest["requestURI"], queryPos + 1, tail - (queryPos + 1));
		_parsedHttpRequest["requestURI"] = std::string(_parsedHttpRequest["requestURI"], 0, queryPos);
	}
	if (_parsedHttpRequest["requestURI"].compare("/") == 0){
		_parsedHttpRequest["requestURI"] = "/index.html";
	}
	if (_parsedHttpRequest["requestURI"].size() > URI_MAX_LEN){
		_requestParsingError = 414; //"URI Too long"
		return -1;
	}
	if (!doesFileExist(getPathToFile())){
		if (_parsedHttpRequest["requestURI"] == "/redirect")
			_requestParsingError = 301;
		else
			_requestParsingError = 404; //"Not Found" 
		return -1;
	}

	head = tail + 1;
	tail = _fullRequest.find("\r\n", head);
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
		head = tail + 2;
		tail = _fullRequest.find("\r\n", head);
		value = std::string(_fullRequest, head, tail - head);
		//! The following two declarations are (annoyingly) only here for debbuging purposes
		char *thisField = (char *)field.c_str();
		char *thisValue = (char*)value.c_str();
		if (_parsedHttpRequest.count(field) == 1)
			_parsedHttpRequest[field] = value;
	}
	//Next come msg-body
		head = tail + 4;
		tail = _fullRequest.length();
		if (_parsedHttpRequest["Transfer-Encoding"] == "chunked")
		{
			_parsedHttpRequest["message-body"] = unchunckedRequest(head);	
		}
		else
		{
			_parsedHttpRequest["message-body"] = std::string(_fullRequest, head, tail - head);
		}
	return 0;
}

std::string Request::unchunckedRequest(int startOfBody) 
{
  std::size_t head = 0;
	std::size_t tail = 0;
  std::string finalBody;
  std::string ogBody;
  std::string sizeStr;
  size_t chunckSize = 1;
  size_t pos;

  ogBody = _fullRequest.substr(startOfBody);
  while (chunckSize)
  {
    head = ogBody.find("\r\n", tail);
    chunckSize = strtol(ogBody.substr(tail, head).c_str(), NULL, 16);
    if (!chunckSize)
    {
    //   std::cout << finalBody << std::endl;
      return finalBody;
    }
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

std::string Request::getFullRequest(void){
	return _fullRequest;
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

std::string Request::getRequestedUri(){
	return _parsedHttpRequest["requestURI"];
}

std::string Request::getPathToFile(void){
	return "." + root_dir + _parsedHttpRequest["requestURI"];
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

std::string Request::donneMoiTonCorpsBabe(void){
	return _parsedHttpRequest["message-body"];
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