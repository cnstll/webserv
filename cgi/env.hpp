# include <iostream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<iostream>
#include"./libft/libft.h"

class env
{
    private:
    public:
        char **_environment;
        env();
        ~env();
        env &operator=(const env &newenv);
        char **str_add(std::string  str_to_add);
        int strlen_list(char **str_list);
        char **calloc_str_list(size_t size);
};