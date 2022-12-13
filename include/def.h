#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

typedef signed char SBOOL;
typedef int BOOL;
typedef unsigned char BYTE;

const int FALSE = 0;
const int TRUE = -1;
const int NTRUE = 1;

#include "conf.h"

#include "zio.h"
#include "htmllib.h"
#include "urllib.h"
#include "inlib.h"
#include "zmime.h"

#ifdef DEBUG
#define ASSERT assert
#else
void ASSERT (int);
void ASSERT (char *);
#endif

extern BOOL bLynx;
extern uid_t UserID;
extern gid_t UserGID;
extern uid_t HttpID;
extern gid_t MailGID;

#define LINESIZE 2048

extern "C"
{
	int wildmat(const char * text, const char * p);
};
