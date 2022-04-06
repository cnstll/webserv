#include "Request.hpp"
#include "utils.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <vector>
#include <fstream>
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Request::Request() : _fullRequest(), root_dir(ROOT_DIR), _requestParsingError(200){
	// std::cout << "Request - Constructor called\n";
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

	//check if method is valid {"GET", "POST, "DELETE"}
	_parsedHttpRequest["method"] = std::string(_fullRequest, head, tail - head);
	int i = 0;
	while (i < 3){
		if (_parsedHttpRequest["method"].compare(methods[i]) == 0)
			break;
		i++;
	}
	if (i == 3){
		_requestParsingError = 405; //"Not implemented"
		return -1;
	}
	head = tail + 1;
	tail = _fullRequest.find(' ', head);
	_parsedHttpRequest["requestURI"] = std::string(_fullRequest, head, tail - head);
	std::size_t queryPos = _parsedHttpRequest["requestURI"].find("?", 0);
	if (queryPos != std::string::npos){
		//std::cout << "\n THERE IS A QUERY IN THIS URI ! \n";
		_parsedHttpRequest["queryString"] = std::string(_parsedHttpRequest["requestURI"], queryPos + 1, tail - (queryPos + 1));
		_parsedHttpRequest["requestURI"] = std::string(_parsedHttpRequest["requestURI"], 0, queryPos);
		//std::cout << "CORRECTED URI: " << _parsedHttpRequest["requestURI"] << " - EXTRACTED QS: " << _parsedHttpRequest["queryString"] << std::endl;
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
		{
			_requestParsingError = 301;
			return -1;
		}
		_requestParsingError = 404; //"Not Found" 
		//std::cout << "THIS IS YOUR requested URI: " << _parsedHttpRequest["requestURI"] << std::endl;
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
		//std::cout << "Field: " << field << " - Value: " << value<< "\n";
		if (_parsedHttpRequest.count(field) == 0){
			//_requestParsingError = 400; //Bad Request
			//return -1;
			continue;
		}
		_parsedHttpRequest[field] = value;
	}
	//writeFullRequestToFile("fullrequest.log");
	//Next come msg-body
	//if (_parsedHttpRequest["Content-Type"].compare("application/x-www-form-urlencoded") == 0){
		head = tail + 4;
		tail = _fullRequest.length();
		_parsedHttpRequest["message-body"] = std::string(_fullRequest, head, tail - head);
		//writeStrToFile(_parsedHttpRequest["message-body"], "body.log");
	//}
	//std::string multipartType = "multipart/form-data;";
	//if (_parsedHttpRequest["Content-Type"].compare(0, multipartType.length(), multipartType) == 0){
	//	std::cout << "\nEntered in if\n";
	//	head = tail + 4;
	//	std::size_t beginSeparator = _parsedHttpRequest["Content-Type"].find("=", 0) + 1;
	//	std::size_t endSeparator = _parsedHttpRequest["Content-Type"].find(";", beginSeparator);
	//	if (endSeparator == std::string::npos)
	//		endSeparator = _parsedHttpRequest["Content-Type"].length();
	//	std::string partSeparator = std::string(_parsedHttpRequest["Content-Type"] , beginSeparator, endSeparator);
	//	std::cout << "\n Separator: " << partSeparator << std::endl;
	//	_parsedHttpRequest["message-body"] = std::string(_fullRequest, head, tail - head);
	//}
		//std::cout << "THIS IS MY BODY--------------------\n ";
		//std::cout << _parsedHttpRequest["message-body"] << std::endl;
		//std::cout << "----------------------END OF MY BODY\n";
	//! parse body - which cases ? form ? Uploaded files ?
	//printFullParsedRequest();
	return 0;
}

/**
 * @brief Append str to the _fullRequest string
 * 
 * @param str c string appended to _fullRequest
 */
void Request::append(char *str, std::size_t readBytes){
	_fullRequest.append(str, readBytes);
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