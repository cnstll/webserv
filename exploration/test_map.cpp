#include <map>
#include <string>
#include <iostream>

int main(){
    std::map<std::string, std::string> datMap;
    datMap["colomban"] = "So bad";
    std::cout << "Size: " << datMap.size() << std::endl;
    std::cout << "DatMp: " << datMap["colomban"] << std::endl;  
    datMap["colomban"] = "So Good";
    std::cout << "Size: " << datMap.size() << std::endl;
    std::cout << "DatMp: " << datMap["colomban"] << std::endl;
    std::string leStringDeColomban = datMap["colomban"];
    std::cout << "Str: " << leStringDeColomban << std::endl;
    std::cout << "Size Str: " << leStringDeColomban.size() << std::endl;
    
    return 0;
}