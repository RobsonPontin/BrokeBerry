#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <float.h>
#include <math.h>

#include "daemon.h"

#define LOG_PATH "/dev/null"
#define WORKING_DIR "/brokeberry/bin"

int daemonize()
{
	int fd, fdlimit;

	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Already a daemon */
	if (getppid() == 1)
		exit(EXIT_FAILURE);

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);

	/* If we got a good PID, then
	we can exit the parent process. */
	if (pid > 0)
		exit(EXIT_SUCCESS);

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0)
	{
		printf("daemonize() Error on setting child process\n");
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir(WORKING_DIR)) < 0)
	{
		printf("daemonize() Error on working directory\n");
		exit(EXIT_FAILURE);
	}

	/* Close all the standard file descriptors */
	fdlimit = sysconf(_SC_OPEN_MAX);
	fd = 0;

	while (fd < fdlimit)
		close(fd++);

	/* Set STDIN, STDOUT, STDERR */
	open(LOG_PATH, O_RDWR);
	dup(0);
	dup(0);

	return EXIT_SUCCESS;
}