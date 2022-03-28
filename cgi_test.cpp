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


int main(int argc, char **argv)
{

    if(argc != 2)
        return 1;


    std::string str = readFileIntoString("/mnt/nfs/homes/jescully/Documents/webserv/testfile.html");
    std::cout << str << std::endl;
    return 0;
}
