#include "utils.hpp"
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '"
             << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

const char *string_to_c_str(const std::string& str)
{
    const char *ret_str = str.c_str();
    return (ret_str);
}

const char *file_to_c_string(const std::string& path)
{
    std::string str = readFileIntoString(path);
    const char *c_str = string_to_c_str(str);
    return (c_str);
}


bool doesFileExist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r+")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

void printErrorAndExit(const std::string &errorMessage){
    std::cerr << errorMessage;
    exit(EXIT_FAILURE);
}

size_t getLineFromPosition(const std::string &str, size_t pos){
  size_t lineNum = 0, tail = 0, head = 0;

  while (tail != std::string::npos){
    tail = str.find("\n", head);
    ++lineNum;
    if (pos < tail)
      break;
    head = tail + 1;
  }
  return lineNum;
}

std::string numberToString(int &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string
}

std::string numberToString(size_t &Number){
    std::ostringstream convert;   // stream used for the conversion
    convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
    return (convert.str()); // convert stream to string
}