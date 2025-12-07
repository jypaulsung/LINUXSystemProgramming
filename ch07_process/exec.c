#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
	pid_t	pid;
	char	*argv[] = { "mycp", "exec.c", "exec2.c", NULL }; // for arguments of execv

	// execl
	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		if (execl("/home/cjs/lsp/file-io/mycp",
				"mycp", "exec.c", "exec1.c", NULL) < 0)  { // replace the child process with mycp, passing "exec.c" and "exec1.c" as arguments
			perror("execl");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0)  { // wait for the child process to terminate
		perror("waitpid");
		exit(1);
	}

	// execv
	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		if (execv("/home/cjs/lsp/file-io/mycp", argv) < 0)  { // replace the child process with mycp, passing the argv array as arguments
			perror("execv");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0)  {
		perror("waitpid");
		exit(1);
	}

	// execlp
	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		if (execlp("env", "env", NULL) < 0)  { // replace the child process with env, passing "env" as an argument
			perror("execlp");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0)  {
		perror("waitpid");
		exit(1);
	}
}