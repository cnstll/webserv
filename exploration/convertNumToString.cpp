#include <iostream>
#include <sstream> 
#include <string>
std::string numberToString(size_t &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string
}

std::string numberToString(int &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string

}

int main(){
  int num = 123;
  size_t numSize = 42;
  std::string numberSize = numberToString(numSize);
  std::string number = numberToString(num);
  std::cout << "NUM STR: " << number << std::endl;
}