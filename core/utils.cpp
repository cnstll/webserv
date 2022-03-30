#include "utils.hpp"
#include <cstdio>
#include <string>

bool doesFileExist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r+")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}