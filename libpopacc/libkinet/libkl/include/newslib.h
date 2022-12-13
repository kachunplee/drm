#ifndef __NEWSLIB_H__
#define __NEWSLIB_H__

#include "zstg.h"

const int EXT_UNDEF = -1;
const int EXT_UNKNOWN = 0;
const int EXT_PHOTO = 1;
const int EXT_VIDEO = 2;
const int EXT_SOUND = 3;
const int EXT_BINARIES = 4;
const int EXT_COMB = 5;
const int EXT_TXTENC = 6;

const int THEXT_UNKNOWN = EXT_UNKNOWN;
const int THEXT_SOUND = 1;
const int THEXT_WMV = 2;
const int THEXT_MOVIE = 3;
const int THEXT_MPG = 4;
const int THEXT_REAL = 5;

class NewsSubject
{
protected:
	ZString	m_stg;
	const char * m_pExpIcon;
	const char * m_pIconTip;
	ZString m_ExpStg;

public:
	NewsSubject ()				{ m_pExpIcon = NULL; }
	NewsSubject (const char * p)		{ m_stg = p; m_pExpIcon = NULL; }

	ZString & stg ()			{ return m_stg; }
	const char* chars() const		{ return m_stg; }
	operator const char *() const		{ return chars(); }
	const char * GetIconTip ()		{ return m_pIconTip; }
	const char * GetExpStg ()		{ return m_ExpStg; }

	bool IsMultiPart(int * pnNum = NULL, int * pnTotal = NULL,
		const char ** ppNum = NULL, const char ** ppTotal = NULL,
			int min = 0, int minTotal = 2,
			int iNum = 0, int iTotal = 0,
			int exttype = EXT_UNDEF, int extpos = -1);
	bool GetMultiSortSubj(ZString & sortsubj,
			int * pnTotal = NULL, int * pnNum = NULL,
			int iTotal = 0, int iNum = 0,
			int minTotal = 2,
			int exttype = EXT_UNDEF, int extpos = -1,
			ZString * pextstg = NULL);

	int GetFileExt(int * = NULL);
	int GetBinArcSubj(char *, string * = NULL, bool bCOMB = false,
			int exttype = EXT_UNDEF, int extpos = -1);
	const char * GetExpIcon();

	int GetFileSpec(string &fname, const char **ppExt, int * pnPart, int * pnTotal);
};

extern int CheckCombPart (const char *f, int m);
extern const char * SepFileRoot(string * pf);
extern int LookupExtMap(const char * ext);
extern int LookupThumbExtMap(const char * ext);

extern const char * GetFileSpec(const char * pSubj, int extpos, string &fname);

#endif // __NEWSLIB_H__
