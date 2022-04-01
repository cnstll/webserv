#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<wait.h>
#include "env.hpp"


int main(int argc, char **argv, char **envp)
{
	env env_to_send;
    int pid;
    char *args[3] = {"php-cgi", "/mnt/nfs/homes/jescully/Documents/webserv/exploration/cgi/php_script_test.php", NULL};


    int i = 0;
    std::cout << std::system("php-cgi php_script_test.php");
    return 0;
}