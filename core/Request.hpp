#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <map>
# include <string>
# ifndef ROOT_DIR
# define ROOT_DIR "/core/server_root"
# endif

# ifndef URI_MAX_LEN
# define URI_MAX_LEN 128
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
    std::string donneMoiTonCorpsBabe(void);
    int getError(void) const;
    std::map<std::string, std::string> &getParsedRequest(void);
    void printFullRequest(void);
    void printFullParsedRequest(void);
    void writeFullRequestToFile(const char *filename);
    
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
    std::string _uri;
    std::string root_dir;
    uint16_t _requestParsingError;
    std::map<std::string, std::string> _parsedHttpRequest;
    static std::string _validRequestFields[];
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif /* ********************************************************* REQUEST_H */