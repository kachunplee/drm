#ifndef __PARSEDATE_H__
#define __PARSEDATE_H__

#include <sys/types.h>
#include <time.h>

typedef struct _TIMEINFO {
         time_t           time;
         long             usec;
         long             tzone;
} TIMEINFO;

#ifdef __cplusplus
extern "C" {
#endif                                                                      
 
extern time_t parsedate(char *text, TIMEINFO *now);
extern int str2tm (struct tm * otm, char * p);

#ifdef __cplusplus
}
#endif                                                                      

#endif
