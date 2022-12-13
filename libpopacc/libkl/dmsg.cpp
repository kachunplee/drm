#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>

int debug = 0;
int dmsg_daemon = 0;

void
DMSG (int priority, char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if(priority <= debug)
	{
		if (dmsg_daemon)
			vsyslog(LOG_DEBUG, fmt, args);
		else
		{
			time_t clock = time(NULL);
			char * ct = ctime(&clock);
			int n = strlen(ct) - 1;
			if (ct[n] == '\n') ct[n] = 0;

			fprintf(stderr, "%s: ", ct);
			vfprintf(stderr, fmt, args);
			putc('\n',stderr);
		}
	}
}
