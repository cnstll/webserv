#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv, char **env)
{
	int pid = -1;
	int status;
	int wpid;
	char *args[5] = {"/usr/bin/go", "run", "client.go", "server_root_test", NULL};
	char *argsForUploadTest[5] = {"/usr/bin/go", "run", "client.go", "test", NULL};

	for (int i = 0; i < 8; i++)
	{
		pid = fork();
		if (!pid)
		{
			execve(args[0], args, env);
			printf("EXECVE FAIL\n");
			exit (1);
		}
	}
	while ((wpid = wait(&status)) > 0);
	execve(args[0], argsForUploadTest, env);
	return 0;
}
