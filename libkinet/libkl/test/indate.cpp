#include <stdio.h>
#include "zstg.h"
#include "inlib.h"

const int TZ_OFF = 4;	// eastern daylight saving
const char * TZNAME = "EST5EDT";


int
main (int ac, char ** av)
{
	if (ac > 1)
	{
		--ac;
		++av;
		TZNAME = av[0];
	}
	++av;
	while(--ac > 0)
	{
		INDateTime date(*av++);

		date.SetTZ(TZNAME);

		tm t = date.time();
		
		int tz_min = t.tm_gmtoff/60;
		printf("%04d-%02d-%02d %02d:%02d:%02d %02d%02d %s\n",
			t.tm_year+1900, t.tm_mon+1, t.tm_mday,
			t.tm_hour, t.tm_min, t.tm_sec,
			tz_min/60, tz_min%60, t.tm_zone);

		tz_min = - TZ_OFF*60;
		t = date.time(tz_min*60);
		printf("%04d-%02d-%02d %02d:%02d:%02d %02d%02d\n",
			t.tm_year+1900, t.tm_mon+1, t.tm_mday,
			t.tm_hour, t.tm_min, t.tm_sec,
			tz_min/60, tz_min%60);

		t = date.localtime();
		
		tz_min = t.tm_gmtoff/60;
		char s[2] = {0, 0};
		if (tz_min < 0)
		{
			s[0] = '-';
			tz_min = -tz_min;
		}
		printf("%04d-%02d-%02d %02d:%02d:%02d %s%02d%02d %s\n",
			t.tm_year+1900, t.tm_mon+1, t.tm_mday,
			t.tm_hour, t.tm_min, t.tm_sec,
			s, tz_min/60, tz_min%60, t.tm_zone);
	}
}
