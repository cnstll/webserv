#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <map>
# include <string>
# include "Server.hpp"
#include <ctime>

# ifndef URI_MAX_LEN
# define URI_MAX_LEN 128
# endif

class Server;

class Request
{
	public:
		Request(std::string fullRequest, Server &serv);
		// Request( Request const & src );
		~Request();

		// Request &		operator=( Request const & rhs );
		void append(char *str, std::size_t readBytes);
		void clear(void);
    int parseBody(void);
    int parseHeader(Server &server);
		std::string getRequestedUri(void);
    std::string getPathToFile(void);
		std::string getHttpMethod(void);
    int getError(void) const;
    std::map<std::string, std::string> &getParsedRequest(void);
    void setErrorCode(int code);
    int getErrorCode(void);
    void printFullRequest(void);
    std::string getFullRequest(void);
    void printFullParsedRequest(void);
    void writeFullRequestToFile(const char *filename);
    void writeStrToFile(const std::string &str, const char *filename);
    std::string unchunckedRequest(int startOfBody);
    //!experimentation -> is it still an experimentation now ?
    bool timeout(void);
    void addFdInfo(int fd);
    int fd;    
    bool headerParsed;
    time_t _inactiveTime;
  
  private:

    time_t _birth; 
		void extractUri( void );
    void initParsedRequestMap(void);
    int checkMethodInLocationBloc(void);
    int checkIfMethodIsImplemented(Server &server);
    void TrimQueryString(size_t endOfURI);
		std::string _fullRequest;
    Server &_currentServer;
    std::string _uri;
    unsigned int _requestParsingError;
    std::map<std::string, std::string> _parsedHttpRequest;
    static std::string _validRequestFields[];
};


#endif /* ********************************************************* REQUEST_H */