#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>

int main() {
  std::string str("Hello");
  char buf[] = " \0World";
  str.append(buf, 7);
  // std::ifstream in(str, std::ios::in | std::ios::binary);
  // std::ostringstream contents;
  // contents << in.rdbuf();
  // in.close();
  std::cout << str << std::endl;
}