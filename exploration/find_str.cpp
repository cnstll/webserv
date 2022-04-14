#include <string>
#include <iostream>

int main(){
  {
    // std::string h = "Il ne faut pas melanger les torchons et les serviettes";
    // size_t ret;
    // if ((ret = h.substr(5, 40).find("torchons")) == std::string::npos)
      // std::cout << "NOT FOUND\n";
    // else
      // std::cout << "FOUND at: " << ret << "in: " << h.substr(5, 40) <<  "\n";
  }
  {
    std::string h = "Il ne faut pas melanger les torchons et les serviettes\n\nOh, ca non ma bonne dame!";
    std::string line;
    size_t startOfLine =0, endOfLine =0;
    size_t hLen = h.length();
    std::cout << "HLEN: " << hLen << std::endl;
    while (startOfLine < hLen){
      endOfLine = h.find("\n", startOfLine);
      line = h.substr(startOfLine, endOfLine - startOfLine);
      std::cout << "SL: " << startOfLine << " - EL: " << endOfLine << " - LL: " << line.length() << " - LINE: " << line << std::endl;
      if (endOfLine == std::string::npos){
        std::cout << "BREAKKKKKKKK\n";
        break;
      }
      startOfLine = endOfLine + 1;
    }
  }
}