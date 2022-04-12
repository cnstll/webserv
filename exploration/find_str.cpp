#include <string>
#include <iostream>

int main(){
  std::string h = "Il ne faut pas melanger les torchons et les serviettes";
  size_t ret;
  if ((ret = h.substr(5, 40).find("torchons")) == std::string::npos)
    std::cout << "NOT FOUND\n";
  else
    std::cout << "FOUND at: " << ret << "in: " << h.substr(5, 40) <<  "\n";
}