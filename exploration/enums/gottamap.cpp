#include <map>
#include <string>
#include <iostream>

std::string getError(int errCode)
{
    std::map<int, std::string> ErrCodeMap =
        {{400, "Bad Request"},
         {401, "Unauthorized"},
         {402, "Payment Required"},
         {403, "Forbiden"},
         {404, "Not found"},
         {405, "Method Not Allowed"},
         {406, "Not Acceptable"},
         {407, "Proxy Authentication Required"},
         {408, "Request Timeout"},
         {409, "Conflict"},
         {410, "Gone"},
         {411, "Length Required"},
         {412, "Precondition Failed"},
         {413, "Payload Too Large"},
         {414, "URI Too Long"},
         {415, "Unsupported Media Type"},
         {416, "Range Not Satisfiable"},
         {417, "Expectation Failed"},
         {418, "I'm a teapot"},
         {421, "Misdirected Request"},
         {422, "Unprocessable Entity"},
         {423, "Locked"}};


    auto it = ErrCodeMap.find(errCode);
    if (it != ErrCodeMap.end())
        return it->second;
    return "";
}

int main()
{
   std::cout << getError(400) << "\n";
   std::cout << getError(401) << "\n";
   std::cout << getError(402) << "\n";
   std::cout << getError(403) << "\n";
   std::cout << getError(404) << "\n";
   std::cout << getError(405) << "\n";
   std::cout << getError(406) << "\n";
   std::cout << getError(407) << "\n";
   std::cout << getError(408) << "\n";
   std::cout << getError(409) << "\n";
   std::cout << getError(410) << "\n";
   std::cout << getError(411) << "\n";
}