#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <map>
# include <string>
# include "Server.hpp"
# ifndef URI_MAX_LEN
# define URI_MAX_LEN 254
# endif
# ifndef REQUEST_TIMEOUT
# define REQUEST_TIMEOUT 5
# endif

class Server;

class Request
{
	public:

		Request(std::string fullRequest, Server &serv);
		~Request();

		void append(char *str, std::size_t readBytes);
		void clear(void);
    int parseBody(void);
    int parseHeader(Server &server);

    std::string getRequestField(const std::string &requestedField);
		std::string getRequestedUri(void);
    std::string getPathToFile(void);
		std::string getHttpMethod(void);
    int getErrorCode(void);
    std::string getFullRequest(void);
    std::map<std::string, std::string> &getParsedRequest(void);
    void setErrorCode(int code);
    void printFullRequest(void);
    void printFullParsedRequest(void);
    std::string unchunckedRequest(int startOfBody);
    bool timeout(void);
    int contentSize;
    time_t inactiveTime;
    bool headerParsed;
  
  private:

		void extractUri( void );
    void initParsedRequestMap(void);
    int checkMethodInLocationBloc(void);
    int checkIfMethodIsImplemented(Server &server);
    void TrimQueryString(size_t endOfURI);
		std::string _fullRequest;
    Server &_currentServer;
    unsigned int _requestParsingError;
    time_t _birth; 
    std::string _uri;
    std::map<std::string, std::string> _parsedHttpRequest;
    static std::string _validRequestFields[];
};


#endif