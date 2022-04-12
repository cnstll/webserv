#ifndef SERVER_HPP
# define SERVER_HPP
# include "utils.hpp"
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <iterator>
//# include <cstdio>

class Server
{
	public:
		typedef typename std::map<std::string, std::string> configMap; 

		Server();
		~Server();

		void displayServerConfig(std::ostream &o = std::cout) const;
		configMap getServerConfig(void) const;
		void parseConfig(const std::string &config);
		class Location {
			public:
				Location();
				~Location();
				configMap locationConfigFields;
				std::string uriPath;
		};
		

	private:

		//Server &		operator=( Server const & rhs );
		//Server( Server const &src );
		void initServerConfig(void);
		void addLocationBlocConfig(void);
		std::string findLocationPath(const std::string &line);
		void parseLocationFields(const std::string& line);
		void checkInstructionEOL(const std::string &line, bool hasLocationLineToken);
		bool lineHasLocationToken(const std::string &line);
		void checkWhitespacesInInstructionLine(const std::string &line, bool hasLocationLineToken);
		void parseLocationBloc(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfline);
		void parseMainInstructionsFields(const std::string &bloc, std::string &line, size_t &startOfLine, size_t &endOfLine);
		static std::string validServerFields[];
		static std::string validLocationFields[];
		configMap serverConfigFields;
		std::vector<Location> locationConfigFields;
		size_t countOfLocationBlocks; 
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */