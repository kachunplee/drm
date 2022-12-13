#include <sys/types.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include "inlib.h"

//
static char * weekdays [] =
{
	"Sunday",
	"Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturaday",
};

static char * months [] =
{
	"Janurary", "Feburary", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "Decemeber",
};

struct TZTbl
{
	const char * tz_name;
	short	tz_zone;
	int	tz_min;
};

#define tZONE 0
#define tDAYZONE 60

#define HOUR(h) (h*60)

static struct TZTbl TimezoneTable[] = {
    { "gmt",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "ut",	tZONE,     HOUR( 0) },	/* Universal */
    { "utc",	tZONE,     HOUR( 0) },	/* Universal Coordinated */
    { "cut",	tZONE,     HOUR( 0) },	/* Coordinated Universal */
    { "z",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "wet",	tZONE,     HOUR( 0) },	/* Western European */
    { "bst",	tDAYZONE,  HOUR( 0) },	/* British Summer */
    { "nst",	tZONE,     HOUR(3)+30 }, /* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR(3)+30 }, /* Newfoundland Daylight */
    { "ast",	tZONE,     HOUR( 4) },	/* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR( 4) },	/* Atlantic Daylight */
    { "est",	tZONE,     HOUR( 5) },	/* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR( 5) },	/* Eastern Daylight */
    { "cst",	tZONE,     HOUR( 6) },	/* Central Standard */
    { "cdt",	tDAYZONE,  HOUR( 6) },	/* Central Daylight */
    { "mst",	tZONE,     HOUR( 7) },	/* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR( 7) },	/* Mountain Daylight */
    { "pst",	tZONE,     HOUR( 8) },	/* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR( 8) },	/* Pacific Daylight */
    { "yst",	tZONE,     HOUR( 9) },	/* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR( 9) },	/* Yukon Daylight */
    { "akst",	tZONE,     HOUR( 9) },	/* Alaska Standard */
    { "akdt",	tDAYZONE,  HOUR( 9) },	/* Alaska Daylight */
    { "hst",	tZONE,     HOUR(10) },	/* Hawaii Standard */
    { "hast",	tZONE,     HOUR(10) },	/* Hawaii-Aleutian Standard */
    { "hadt",	tDAYZONE,  HOUR(10) },	/* Hawaii-Aleutian Daylight */
    { "ces",	tDAYZONE,  -HOUR(1) },	/* Central European Summer */
    { "cest",	tDAYZONE,  -HOUR(1) },	/* Central European Summer */
    { "mez",	tZONE,     -HOUR(1) },	/* Middle European */
    { "mezt",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "cet",	tZONE,     -HOUR(1) },	/* Central European */
    { "met",	tZONE,     -HOUR(1) },	/* Middle European */
    { "eet",	tZONE,     -HOUR(2) },	/* Eastern Europe */
    { "msk",	tZONE,     -HOUR(3) },	/* Moscow Winter */
    { "msd",	tDAYZONE,  -HOUR(3) },	/* Moscow Summer */
    { "wast",	tZONE,     -HOUR(8) },	/* West Australian Standard */
    { "wadt",	tDAYZONE,  -HOUR(8) },	/* West Australian Daylight */
    { "hkt",	tZONE,     -HOUR(8) },	/* Hong Kong */
    { "cct",	tZONE,     -HOUR(8) },	/* China Coast */
    { "jst",	tZONE,     -HOUR(9) },	/* Japan Standard */
    { "kst",	tZONE,     -HOUR(9) },	/* Korean Standard */
    { "kdt",	tZONE,     -HOUR(9) },	/* Korean Daylight */
    { "cast",	tZONE,     -(HOUR(9)+30) }, /* Central Australian Standard */
    { "cadt",	tDAYZONE,  -(HOUR(9)+30) }, /* Central Australian Daylight */
    { "east",	tZONE,     -HOUR(10) },	/* Eastern Australian Standard */
    { "eadt",	tDAYZONE,  -HOUR(10) },	/* Eastern Australian Daylight */
    { "nzst",	tZONE,     -HOUR(12) },	/* New Zealand Standard */
    { "nzdt",	tDAYZONE,  -HOUR(12) },	/* New Zealand Daylight */

    /* For completeness we include the following entries. */
#if	0

    /* Duplicate names.  Either they conflict with a zone listed above
     * (which is either more likely to be seen or just been in circulation
     * longer), or they conflict with another zone in this section and
     * we could not reasonably choose one over the other. */
    { "fst",	tZONE,     HOUR( 2) },	/* Fernando De Noronha Standard */
    { "fdt",	tDAYZONE,  HOUR( 2) },	/* Fernando De Noronha Daylight */
    { "bst",	tZONE,     HOUR( 3) },	/* Brazil Standard */
    { "est",	tZONE,     HOUR( 3) },	/* Eastern Standard (Brazil) */
    { "edt",	tDAYZONE,  HOUR( 3) },	/* Eastern Daylight (Brazil) */
    { "wst",	tZONE,     HOUR( 4) },	/* Western Standard (Brazil) */
    { "wdt",	tDAYZONE,  HOUR( 4) },	/* Western Daylight (Brazil) */
    { "cst",	tZONE,     HOUR( 5) },	/* Chile Standard */
    { "cdt",	tDAYZONE,  HOUR( 5) },	/* Chile Daylight */
    { "ast",	tZONE,     HOUR( 5) },	/* Acre Standard */
    { "adt",	tDAYZONE,  HOUR( 5) },	/* Acre Daylight */
    { "cst",	tZONE,     HOUR( 5) },	/* Cuba Standard */
    { "cdt",	tDAYZONE,  HOUR( 5) },	/* Cuba Daylight */
    { "est",	tZONE,     HOUR( 6) },	/* Easter Island Standard */
    { "edt",	tDAYZONE,  HOUR( 6) },	/* Easter Island Daylight */
    { "sst",	tZONE,     HOUR(11) },	/* Samoa Standard */
    { "ist",	tZONE,     -HOUR(2) },	/* Israel Standard */
    { "idt",	tDAYZONE,  -HOUR(2) },	/* Israel Daylight */
    { "idt",	tDAYZONE,  -(HOUR(3)+30) }, /* Iran Daylight */
    { "ist",	tZONE,     -(HOUR(3)+30) }, /* Iran Standard */
    { "cst",	 tZONE,     -HOUR(8) },	/* China Standard */
    { "cdt",	 tDAYZONE,  -HOUR(8) },	/* China Daylight */
    { "sst",	 tZONE,     -HOUR(8) },	/* Singapore Standard */

    /* Dubious (e.g., not in Olson's TIMEZONE package) or obsolete. */
    { "gst",	tZONE,     HOUR( 3) },	/* Greenland Standard */
    { "wat",	tZONE,     -HOUR(1) },	/* West Africa */
    { "at",	tZONE,     HOUR( 2) },	/* Azores */
    { "gst",	tZONE,     -HOUR(10) },	/* Guam Standard */
    { "nft",	tZONE,     HOUR(3)+30 }, /* Newfoundland */
    { "idlw",	tZONE,     HOUR(12) },	/* International Date Line West */
    { "mewt",	tZONE,     -HOUR(1) },	/* Middle European Winter */
    { "mest",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "swt",	tZONE,     -HOUR(1) },	/* Swedish Winter */
    { "sst",	tDAYZONE,  -HOUR(1) },	/* Swedish Summer */
    { "fwt",	tZONE,     -HOUR(1) },	/* French Winter */
    { "fst",	tDAYZONE,  -HOUR(1) },	/* French Summer */
    { "bt",	tZONE,     -HOUR(3) },	/* Baghdad */
    { "it",	tZONE,     -(HOUR(3)+30) }, /* Iran */
    { "zp4",	tZONE,     -HOUR(4) },	/* USSR Zone 3 */
    { "zp5",	tZONE,     -HOUR(5) },	/* USSR Zone 4 */
    { "ist",	tZONE,     -(HOUR(5)+30) }, /* Indian Standard */
    { "zp6",	tZONE,     -HOUR(6) },	/* USSR Zone 5 */
    { "nst",	tZONE,     -HOUR(7) },	/* North Sumatra */
    { "sst",	tZONE,     -HOUR(7) },	/* South Sumatra */
    { "jt",	tZONE,     -(HOUR(7)+30) }, /* Java (3pm in Cronusland!) */
    { "nzt",	tZONE,     -HOUR(12) },	/* New Zealand */
    { "idle",	tZONE,     -HOUR(12) },	/* International Date Line East */
    { "cat",	tZONE,     HOUR(10) },	/* -- expired 1967 */
    { "nt",	tZONE,     HOUR(11) },	/* -- expired 1967 */
    { "ahst",	tZONE,     HOUR(10) },	/* -- expired 1983 */
    { "hdt",	tDAYZONE,  HOUR(10) },	/* -- expired 1986 */
#endif	/* 0 */
};

//
// format: hh[mm[ss]]
//
static unsigned int
TZ2Int (const char * tz)
{
	unsigned t = 0;
	int k = strlen(tz);
	if (k%1)
		return 0;
	k /= 2;
	for (int n = 0; n < 6; n+=2)
	{
		t *= 60;
		if (k-- > 0)
		{
			if (!isdigit(tz[n]) || !isdigit(tz[n+1]))
				return 0;
			t += (tz[n]-'0')*10 + (tz[n+1]-'0');
		}
	}

	return t;
}

//
INDateTime::INDateTime ()
{
	::time(&m_epoch);
}

//
// format: eg. Wed, 17 May 2001 15:40:44 -0700
//
INDateTime::INDateTime (const char * p)
{
	m_tm.tm_isdst = 0;
	m_tm.tm_zone = "GMT";
	m_tm.tm_gmtoff = 0;
	m_tm.tm_wday = -1;
	m_tm.tm_hour = 0;
	m_tm.tm_min = 0;
	m_tm.tm_sec = 0;

	int i,n;

	//
	// Day of week
	//
	const char *q;
	for(q = p; *q && !isdigit(*q); q++)
	{
		if(*q == ' ' || *q == ',')
		{
			n = q-p;
			for(i = 0; i < 7; i++)
				if(strncasecmp(p, weekdays[i], n) == 0)
				{
					m_tm.tm_wday = i;
					break;
				}
			q++;
			break;
		}
	}

	for(p = q; *p && !isdigit(*p); p++);		// skip over to...

	//
	// Day of month
	//
	m_tm.tm_mday = atoi(p);
	while(*p && (isdigit(*p) || *p == ' ')) p++;	// skip over to...

	//
	// Month
	//
	m_tm.tm_mon = 0;
	for(q = p; *q; q++)
	{
		if(!isalpha(*q))
		{
			n = q-p;
			for(i = 0; i < 12; i++)
				if(strncasecmp(p, months[i], n) == 0)
				{
					m_tm.tm_mon = i;
					break;
				}
			break;
		}
	}
	for(p = q; *p && !isdigit(*p); p++);		// skip over to...

	//
	// Year
	//
	m_tm.tm_year = atoi(p);
	if(m_tm.tm_year < 70)
		m_tm.tm_year += 100;
	else if(m_tm.tm_year > 1900)
		m_tm.tm_year -= 1900;
	while(*p && isdigit(*p)) p++;
	while(*p && *p == ' ') p++;			// skip over to...

	if(*p == 0)
	{
		m_epoch = ::timegm(&m_tm);
		return;
	}

	//
	// Time
	//
	//
	// Parse time
	//
	char tzdiff[128];
	i = sscanf(p, "%2d:%2d:%2d %s\t", 
		&m_tm.tm_hour, &m_tm.tm_min, &m_tm.tm_sec, tzdiff);

	m_epoch = ::timegm(&m_tm);

	if(i == 4)
	{
		//
		// Got time zone
		//
		time_t t=0;
		if (tzdiff[0] == '-')
			t -= TZ2Int(tzdiff+1);
		else if (tzdiff[0] == '+')
			t += TZ2Int(tzdiff+1);
		else if(isdigit(tzdiff[0]))
			t += TZ2Int(tzdiff);
		else {
			int ntz = sizeof(TimezoneTable)/sizeof(TimezoneTable[0]);
			for(int n = 0; n < ntz; n++)
			{
			  if (strcasecmp(tzdiff, TimezoneTable[n].tz_name) == 0)
			  {
				t = - (TimezoneTable[n].tz_min-TimezoneTable[n].tz_zone)*60;
				break;
			  }
			}
		}

		m_tm.tm_gmtoff = t;
		m_epoch -= t;
	}
}


tm&
INDateTime::time (int tz_sec)
{
	time_t t = m_epoch + tz_sec;
	::gmtime_r(&t, &m_tm);
	return m_tm;
}

void
INDateTime::SetTZ (const char * tzname)
{
	setenv("TZ", tzname, 1);
	tzset();
}

tm&
INDateTime::localtime (int tz_sec)
{
	time_t t = m_epoch + tz_sec;
	::localtime_r(&t, &m_tm);
	return m_tm;
}

//----------------------------------------------------------------------------//
// Mail Name Formats:
//	(Real Name) Email Name
//	Real Name <Email Name>
//
string &
INMailName::EMailAddress(string & stg)
{
	if(!m_stg || m_stg == "")
		return stg = "";

	const char *p,*q;

	//
	// Real Name <Email Mail>
	//
	p = strchr(m_stg, '<');
	if(p && (q=strchr(++p, '>')))
	{
		stg = string(p, q-p);
		return stg;
	}

	//
	// (Real Name) Email Name
	//
	if(m_stg[0] == '(' && (p = strchr(m_stg, ')')))
	{
		p++;
		q = m_stg+strlen(m_stg)-1;
	}
	//
	// Email Name (Real Name)
	//
	else if((q = strchr(m_stg, '(')) && strchr(q, ')'))
	{
		q--;
		p = m_stg;
	}
	else
	{
		//
		// Last resort - take the whole string
		p = m_stg;
		q = m_stg+strlen(m_stg)-1;
	}

	for(; q>m_stg && *q==' '; q--);				// strip trailing space
	for(p = m_stg; p<q && *p==' '; p++);		// strip leading space
	if(*p == '"' && *q == '"') { p++; q--; }		// strip quote
	if(*p == '<' && *q == '>') { p++; q--; }		// strip <>
	stg = string(p, q-p+1);

	return stg;
}

string & INMailName::RealName(string & stg)
{
	if(!m_stg || m_stg == "")
		return stg = "";

	const char *p,*q;

	//
	// (Real Name) ...
	//
	p = strchr(m_stg, '(');
	if(p && (q=strchr(++p, ')')))
	{
 		if(*p == '"' && ((q-1) > p) && q[-1] == '"') { p++; q--; }
		stg = string(p, q-p);
		return stg;
	}

	//
	// Real Name <Email Mail>
	//
	p = strrchr(m_stg, '<');
	if(p && p > m_stg && strchr(p, '>'))
	{
		q = p-1;
	}
	else
	{
		q = m_stg+strlen(m_stg)-1;
	}

	for(; q>m_stg && *q==' '; q--);				// strip space
	for(p = m_stg; p<q && *p==' '; p++);		// skip space
	if(p != q && *p == '"' && *q == '"') { p++; q--; }		// strip quote
	if(*p == '<' && *q == '>') { p++; q--; }		// strip <>
	stg = string(p, q-p+1);

	return stg;
}
