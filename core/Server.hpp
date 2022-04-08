#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>

class Server
{
	public:
		typedef typename std::map<std::string, std::string> configMap; 

		Server();
		~Server();

		void displayServerConfig(std::ostream &o = std::cout) const;
		configMap getServerConfig(void) const;
		class Location {
			Location();
			~Location();
			public:
				configMap parameters;

		};
		

	private:

		//Server &		operator=( Server const & rhs );
		//Server( Server const &src );
		void initServerConfig(void);
		static std::string validServerFields[];
		static std::string validLocationFields[];
		configMap config;
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */