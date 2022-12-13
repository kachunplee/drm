#include <stdio.h>
#include "zstg.h"
#include "newslib.h"

int
main (int ac, char ** av)
{
	++av;
	while(--ac > 0)
	{
		const char * pSubj = *av++;
		ZString subjroot, filenum;

		POST_DEF def = ParsePostDef (pSubj, "", true,
                        subjroot, filenum);

		printf("flag=%02x exttype=%d %d/%d byte=%d\n%s\n%s\n",
                	def.flag, def.exttype,
                	def.part_no, def.part_total,
                	def.byte,
                	filenum.c_str(),
                	subjroot.c_str());
	}
}
