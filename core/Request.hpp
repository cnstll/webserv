#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <map>
# include <string>
# include "Server.hpp"
# ifndef ROOT_DIR
# define ROOT_DIR "/core/server_root"
# endif
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
    std::string donneMoiTonCorpsBabe(void);
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
    //!experimentation
    bool timeout()
    {
      _inactiveTime = std::difftime(time(0), _birth);
      return _inactiveTime > 3;
    }
    void addFdInfo(int fd);
    int fd;    
    bool headerParsed;
    time_t _inactiveTime;
  
  private:

    time_t _birth; 
		void extractUri( void );
    void initParsedRequestMap(void);
		std::string _fullRequest;
    Server &_currentServer;
    std::string _uri;
    std::string root_dir;
    unsigned int _requestParsingError;
    std::map<std::string, std::string> _parsedHttpRequest;
    static std::string _validRequestFields[];
};


#endif /* ********************************************************* REQUEST_H */