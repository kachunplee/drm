#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream.h>

#include "nntpart.h"

extern int debug;

const char * NNTPservers = "dex.pathlink.com:1119";
const char * NNTPauths = "teresa11/111111";

int
main(int ac, char ** av)
{
	debug = 1;

	extern char *optarg;
	extern int optind;

	int c;
	while((c = getopt(ac, av, "S:A:d")) != EOF)
	{
		switch(c)
		{
		case 'S':
			NNTPservers = optarg;
			break;

		case 'A':
			NNTPauths = optarg;
			break;

		case 'd':
			debug++;
			break;

		default:
		usage:
			cout << "usage: arttest [-S servers] [-A authinfo] [-d] group artnum\n";
			return(1);
		}
	}

	ac -= optind;
	av += optind;

	if(ac < 2)
		goto usage;

	char * p;
	try
	{
		NNTPArticle art(av[0], av[1]);

		while((p=art.GetLine()))
		{
			cout << p << endl;
			if(p[0] == '.' && p[1] == '\r')
				break;
		}
	}
	catch(const char * p)
	{
		cerr << "Error: " << p << endl;
		return(1);
	}
	catch(int err)
	{
		cerr << "Error: " << strerror(errno) << endl;
		return(1);
	}

	return(0);
}
