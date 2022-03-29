#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <map>
# include <string>
# ifndef ROOT_DIR
# define ROOT_DIR "/core/server_root"
# endif

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
		int parse(void);
		std::string getRequestedUri(void);
    std::string getPathToFile(void);
		std::string getHttpMethod(void);
    uint16_t getError(void) const;
    std::map<std::string, std::string> &getParsedRequest(void);
    void printFullRequest(void);
    void printFullParsedRequest(void);
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
    void initParsedRequestMap(void);
		std::string _fullRequest;
		// * map<std::string, std::string> with all the elements. The elements gets updated if necessary
		// * Look into unordered_map that might be quicker than map
		//std::string _generalHeader; //* No need to separate general headers from request header?
		std::string _requestLine;
		std::string _requestHeader;
		std::string _requestBody;
		std::string _entityHeader; //* Fields no specified in the RFC are saved here for each request
    std::string _uri;
    std::string root_dir;
    uint16_t _requestParsingError;
    std::map<std::string, std::string> _parsedHttpRequest;
    static std::string _validRequestFields[];
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */