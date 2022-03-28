#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# define ROOT_DIR "server_root"

// Request       = Request-Line            
//                 *(( general-header      
//                  | request-header       
//                  | entity-header ) CRLF)
//                 CRLF
//                 [ message-body ]

class Request
{
	public:
		Request();
		Request(std::string fullRequest);
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );
		void append(char *str);
		void clear(void);
		void parse(void);
		std::string getRequestedUri(void);
		void printFullRequest(void);

	private:
	
		//  general-header = Cache-Control
    //                 | Connection       
    //                 | Date             
    //                 | Pragma           
    //                 | Trailer          
    //                 | Transfer-Encoding
    //                 | Upgrade          
    //                 | Via              
    //                 | Warning          
		//Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
		// request-header = Accept          
                  //  | Accept-Charset  
                  //  | Accept-Encoding 
                  //  | Accept-Language 
                  //  | Authorization   
                  //  | Expect          
                  //  | From            
                  //  | Host            
                  //  | If-Match        
                  //  | If-Modified-Sinc
                  //  | If-None-Match   
                  //  | If-Range        
                  //  | If-Unmodified-Si
                  //  | Max-Forwards    
                  //  | Proxy-Authorizat
                  //  | Range           
                  //  | Referer         
                  //  | TE              
                  //  | User-Agent     
		void extractUri( void );
		std::string _fullRequest;
		// * map<std::string, std::string> with all the elements. The elements gets updated if necessary
		// * Look into unordered_map that might be quicker than map
		//std::string _generalHeader; //* No need to separate general headers from request header?
		std::string _requestLine;
		std::string _requestHeader;
		std::string _requestBody;
		std::string _entityHeader; //* Fields no specified in the RFC are saved here for each request
    std::string _uri;
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */