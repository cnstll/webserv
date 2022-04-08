#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv, char **env)
{
	int pid = -1;
	char *args[5] = {"/usr/bin/go", "run", "client.go", "../core/server_root", NULL};
	for (int i = 0; i < 10; i++)
	{
		pid = fork();
		if (!pid)
		{
			execve(args[0], args, env);
			printf("EXECVE FAIL\n");
			exit (1);
		}
	}
	wait(NULL);
	return 0;
}
