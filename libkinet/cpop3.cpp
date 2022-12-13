#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "dmsg.h"
#include "cpop3.h"

CPOP3::CPOP3 (const char * servers, const char * auths)
	: ClientTCP(servers, 110)
{
		// Get Header
		char * p = SendCmd();
		if(p == NULL)
			throw("Cannot connect to POP server");

		DMSG(1, "%s", p);

		if(strncmp(p, "+OK", 3) != 0) throw(p);

		int n = GetCServer();
		if(n >= 0 && auths)
		{
			string userid;
			string passwd;
			for(int i = 0; (i < n) && *auths; auths++)
			{
				if(*auths == ';') i++;
			}

			if(*auths)
			{
				const char * q = auths;
				while(*auths && *auths != '/' && *auths != ';') auths++;
				userid = string(q, auths-q);
				if(*auths == '/')
				{
					q = ++auths;
					while(*auths && *auths != ';') auths++;
					passwd = string(q, auths-q);
				}
			}

			if(userid.length())
			{
				SetCmd("user %s\r\n", userid.c_str());
				p = SendCmd();
				if(strncmp(p, "+OK", 3) != 0) throw(p);

				SetCmd("pass %s\r\n", passwd.c_str());
				p = SendCmd();
				if(strncmp(p, "+OK", 3) != 0) throw(p);
			}
		}
}

CPOP3::~CPOP3 ()
{
		char * p = SendCmd("QUIT\r\n");
		DMSG(1, "%s", p);
}

char * CPOP3::GetLine ()
{
	int n;
	char * p = GetLine(&n);
	if(p[n-1] == '\r') p[--n] = 0;
	if(p[0] == '.')
	{
		switch(p[1])
		{
		case '.':
			return p+1;

		case 0:
			return NULL;
		}
	}

	return p;
}
