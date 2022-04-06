#include "core.hpp"
#include "Response.hpp"
#include "utils.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include<sys/socket.h>
#include <map>

std::string getErrorContent(int errCode)
{
    std::map<int, std::string> ErrCodeMap =
        {{300, "<Html> Multiple Choice </Html>\n"},
         {301, "<Html> Moved Permanently </Html>\n"},
         {302, "<Html> Found </Html>\n"},
         {303, "<Html> See Other </Html>\n"},
         {304, "<Html> Not Modified </Html>\n"},
         {307, "<Html> Temporary Redirect </Html>\n"},
         {308, "<Html> Permanent Redirect </Html>\n"},
         {400, "<Html> Bad Request </Html>\n"},
         {401, "<Html> Unauthorized </Html>\n"},
         {402, "<Html> Payment Required </Html>\n"},
         {403, "<Html> Forbiden </Html>\n"},
         {404, "<Html> Not Found </Html>\n"},
         {405, "<Html> Method Not Allowed </Html>\n"},
         {406, "<Html> Not Acceptable </Html>\n"},
         {407, "<Html> Proxy Authentication Required </Html>\n"},
         {408, "<Html> Request Timeout </Html>\n"},
         {409, "<Html> Conflict </Html>\n"},
         {410, "<Html> Gone </Html>\n"},
         {411, "<Html> Length Required </Html>\n"},
         {412, "<Html> Precondition Failed </Html>\n"},
         {413, "<Html> Payload Too Large </Html>\n"},
         {414, "<Html> URI Too Long </Html>\n"},
         {415, "<Html> Unsupported Media Type </Html>\n"},
         {416, "<Html> Range Not Satisfiable </Html>\n"},
         {417, "<Html> Expectation Failed </Html>\n"},
         {418, "<Html> I'm a teapot </Html>\n"},
         {421, "<Html> Misdirected Request </Html>\n"},
         {422, "<Html> Unprocessable Entity </Html>\n"},
         {423, "<Html> Locked </Html>\n"},
         {426, "<Html> Upgrade Required </Html>\n"},
         {428, "<Html> Precondition Required </Html>\n"},
         {429, "<Html> Too Many Requests </Html>\n"},
         {431, "<Html> Request Header Fields Too Large </Html>\n"},
         {451, "<Html> Unavailable For Legal Reasons </Html>\n"},
         {500, "<Html> Internal Server Error </Html>\n"},
         {501, "<Html> Not Implemented </Html>\n"},
         {502, "<Html> Bad Gateway </Html>\n"},
         {503, "<Html> Service Unavailable </Html>\n"},
         {504, "<Html> Gateway Timeout </Html>\n"},
         {505, "<Html> HTTP Version Not Supported </Html>\n"},
         {506, "<Html> Variant Also Negotiates </Html>\n"},
         {510, "<Html> Not Extended </Html>\n"},
         {511, "<Html> Network Authentication Required </Html>\n"}};



    char buf[3];
    sprintf(buf, "%d", errCode);
    std::string errPathname = "." + std::string(ROOT_DIR) + "/" + buf + ".html"; 
    if(doesFileExist(errPathname))
        return (readFileIntoString(errPathname));
    std::map<int, std::string>::iterator it = ErrCodeMap.find(errCode);
    if (it != ErrCodeMap.end())
        return it->second;
    return "";
}


std::string Response::codeToReasonPhrase(int statusCode){
    // ! To be modified with a table of code
    (void)statusCode;
    return "OK";
}

Response::Response(int code)
    : _statusCode(code),
      _Date(timeAsString()), _Server("Webserv"), _ContentLength(""),
      _ContentType("text/html"), _Connection("Keep-Alive"){
    

    char buf[3];
    sprintf(buf,"%i", _statusCode);
    this->_ReasonPhrase = Response::codeToReasonPhrase(code);
    _Status = "HTTP/1.1 " + std::string(buf) + " " + _ReasonPhrase; 
};

Response::Response(std::map<std::string, std::string> &parsedRequest, int errorCode)
    : _statusCode(errorCode),
      _Date(timeAsString()), _Server("Webserv"), _ContentLength(""),
      _ContentType(parsedRequest["Content-Type"]), _Connection(parsedRequest["Connection"]){

    char buf[3];
    sprintf(buf,"%i", _statusCode);
    this->_ReasonPhrase = codeToReasonPhrase(errorCode);
    _Status = "HTTP/1.1 " + std::string(buf) + " " + _ReasonPhrase; 
};

Response::Response(const Response &newResponse)
{
    return;
}

Response::~Response(void)
{
    return;
}

Response &Response::operator=(const Response &newResponse)
{
    return *this;
}

std::string Response::timeAsString()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str("");
    str.append(buffer);
    str.append(" GMT");
    return (str);
}

void Response::addBody(std::string pathname)
{
    /*
    if response code > 200
    add body uses the content from the map in thhe response.
    */
    char buf[10];

    if (_statusCode >= 300)
        _Content = getErrorContent(_statusCode);    
    else{
        std::ifstream input_file(pathname.c_str());
        _Content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
   }
   const char *str = _Content.c_str();
    sprintf(buf, "%lu", _Content.size());
    _ContentLength = std::string(buf);
}

/**
 * @brief function used to send a Response to a client.
 * 
 */
void Response::sendResponse(int clientSocket){

    std::string CRLF = "\n";
    std::string contype;
    std::string packagedResponse = _Status + CRLF +
        "Date: " + _Date + CRLF +
        "Server: " + _Server + CRLF +
        "Content-Length: " + _ContentLength + CRLF +
        "Content-Type: " + contype + CRLF +
        "Connection: " + _Connection + CRLF;


    if (_statusCode >= 300 && _statusCode < 400)
        packagedResponse += "Location: " + _Location + CRLF;
    packagedResponse = packagedResponse + CRLF + _Content;
    // ! Handle error here. 
    size_t i;
    if ((i = write(clientSocket, packagedResponse.c_str(), packagedResponse.size())) < 0){
        exit(EXIT_FAILURE);
    }
}