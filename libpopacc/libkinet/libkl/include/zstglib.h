#ifndef __ZSTGLIB_H__
#define __ZSTGLIB_H__

#include <string>
#include "zregex.h"

extern int stg_index(string & s, const Regex & ex, size_t n = 0);
extern int stg_indexr(string & s, const Regex & ex, size_t n = 0);
extern void stg_downcase(string & s);
extern int stg_gsub(string &s, const char * old, const char * that);
extern int stg_gsub(string &s, char old, char that);

#endif // __ZSTGLIB_H__
