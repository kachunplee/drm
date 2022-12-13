#include <stdio.h>
#include "zstg.h"
#include "newslib.h"

int
main (int ac, char ** av)
{
	++av;
	while(--ac > 0)
	{
		NewsSubject subj(*av++);
		ZString s;
		int total, num;

		if (subj.GetMultiSortSubj(s, &total, &num, 0, 0, 1))
		{
			printf ("%s sort=%s part=%d total=%d\n",
				subj.stg().c_str(), s.c_str(), total, num);
		}
		else
		{
			printf ("%s not multi\n", subj.stg().c_str());
		}
	}
}
