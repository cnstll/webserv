#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>

class Request
{

	public:

		Request(std::string fullRequest);
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );
		std::string getRequestedUri( void );

	private:

		std::string extractUri( void );
		std::string _fullRequest;
		std::string _uri

};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */