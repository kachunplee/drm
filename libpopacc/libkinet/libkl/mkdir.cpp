#include <sys/stat.h>
#include <unistd.h>

#define _DIR_MODE 0775

bool
MakeDir (const char * Name)
{
    struct stat		Sb;

    /* See if it failed because it already exists. */
    if(stat(Name, &Sb) >= 0 && S_ISDIR(Sb.st_mode))
	return true;

    return (mkdir(Name, _DIR_MODE) >= 0);
}

/*
*/
bool
MakeDirP (const char * Name)
{
    // Optimize common case -- parent almost always exists.
    if (MakeDir(Name))
	return true;

    // Well, try to create parent directories
    char buf [strlen(Name)+1];
    const char *p = Name;
    char * q = buf;
    if (*p == '/') *q++ = *p++;

    for (; *p; p++, q++)
    {
	if (*p == '/') {
	    *q = '\0';
	    if(!MakeDir(buf))
		return false;
	}
	*q = *p;
    }

    return MakeDir(Name);
}
