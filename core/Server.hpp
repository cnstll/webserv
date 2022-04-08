#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>

class Server
{

	public:

		Server();
		Server( Server const & src );
		~Server();

		Server &		operator=( Server const & rhs );
		class Location{
			public:
				std::map<std::string, std::string> parameters;

		};

	private:

		static std::string validServerFields[];
		static std::string validLocationFields[];
		std::map<std::string, std::string> parameters;
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */