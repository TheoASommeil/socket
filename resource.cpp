#include <sys/resource.h>
#include <unistd.h>
#include <cstdio>

int main()
{
	struct rusage myusage, childusage;

	pid_t child_pid;

	if ( (child_pid = fork()) == 0 ) {
		sleep(2);
		_exit(0);
	}
	sleep(5);
	getrusage(RUSAGE_SELF, &myusage);
	getrusage(RUSAGE_CHILDREN, &childusage);

	double user = (double)myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec / 1000000.0;
	user += (double)childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / 1000000.0;
	
	double sys = (double)myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec / 1000000.0;
	sys += (double)childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec / 1000000.0;

	printf("\nuser time : %g, sys time : %g\n", user, sys);

	return 0;
}
