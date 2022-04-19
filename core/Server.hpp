#ifndef SERVER_HPP
# define SERVER_HPP
# include "../utils/utils.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "../cgi/cgiHandler.hpp"
# include <iostream>
# include <string>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>


#define REQUEST_READ_SIZE 16000

class Request;
class Server
{
	public:
		typedef typename std::map<std::string, std::string> configMap; 

		Server();
		~Server();
		void displayServerConfig(std::ostream &o = std::cout) const;
		configMap getServerConfig(void) const;
		int getServerPort(void) const;
		void parseServerConfigFields(const std::string &config);
		std::map<int, Request*> requestMap;
		std::string constructPath(std::string &uri);
		int parseHeader(Request &request);
		int recvRequest(const int &fd, Request &request);
		int isRequestDone(Request &Request, int &contentLength, int &startOfBody);
		int acceptNewConnexion(int serverFd);
		void makeFdNonBlocking(int &fd);
		int setupServer(int port, int backlog);
		void respond(int fd);
		void checkMethodsInLocation(std::string &values, const std::string &line);
		std::string getExtension(std::string &);
		std::string getLocationField(const std::string &locationUri, const std::string &requestedField);
		std::string getServerConfigField(const std::string &requestedField);
		std::string *getImplementedMethods(void);
		void	closeConnection(int fd);
		bool isEndOfBloc(const std::string &line);
		class Location {
			public:
				Location();
				~Location();
				configMap fields;
		};

	private:
		Request *_currentRequest;
		//Server &		operator=( Server const & rhs );
		//Server( Server const &src );
		void initServerConfig(void);
		void addLocationBlocConfig(std::string &uri);
		std::string findLocationPath(const std::string &line);
		void checkMethodsInLocation(const std::string &values);
		void parseLocationFields(const std::string& line, const std::string &uri);
		void checkInstructionEOL(const std::string &line, bool hasLocationLineToken);
		bool lineHasLocationToken(const std::string &line);
		void checkWhitespacesInInstructionLine(const std::string &line, bool hasLocationLineToken);
		void parseLocationBloc(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfline);
		void parseMainInstructionsFields(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine);
		bool isEmptyLine(const std::string &line);
		void parsePort(void);

		static std::string validServerFields[];
		static std::string validLocationFields[];
		static std::string implementedMethods[];
		configMap serverConfigFields;
		std::map<std::string, Location> locationBlocs;
		size_t countOfLocationBlocks;
		int serverPort; 
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */