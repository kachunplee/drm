#include <map>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dmsg.h"
#include "zstg.h"
#include "newslib.h"

//
static const char * ext_text [] = {
				"txt",
				"nfo",
				NULL
};

static const char * ext_binaries [] = {
				"exe",
				"zip",
				"pdf",
				NULL
};

static const char * ext_comb [] = {
				"rar",
				"par",
				"par2",
				"sfv",
				NULL
};

static const char * ext_photo [] = {
				"gif",
				"gpf",
				"png",
				"jpg",
				"jpeg",	
				"bmp",
				"dib",
				"tif",
				"tiff",
				"tga",
				"ppm",
				"pgm",
				"pbm",
				NULL
};

static const char * ext_video [] = {
				"avi",
				"asf",
				"mov",
				"mpg",
				"mpeg",
				"mpe",
				"ra",
				"ram",
				"rm",
				"ts",
				"wmv",
				NULL
};

static const char * ext_sound [] = {
				"ape",
				"wav",
				"mid",
				"midi",
				"snd",
				"swa",
				"aif",
				"aiff",
				"aifc",
				"afc",
				"mp2",
				"mp3",
				"mpu",
				"m1a",
				"au",
				"flac",
				NULL
};

static const char * thext_sound [] = {
				"mp3",
				"mp2",
				"mpu",
				"wav",
				"mid",
				"midi",
				"snd",
				"swa",
				"aif",
				"aiff",
				"aifc",
				"afc",
				"m1a",
				"au",
				NULL
};

static const char * thext_wmv [] = {
				"wmv",
				"avi",
				"asf",
				NULL
};

static const char * thext_movie [] = {
				"mov",
				NULL
};

static const char * thext_mpg [] = {
				"mpg",
				"mpeg",
				"mpe",
				NULL
};

static const char * thext_real [] = {
				"ra",
				"ram",
				"rm",
				NULL
};

typedef const char * CPTR;
struct ConstCharPtrCaseLess : binary_function<CPTR, CPTR, bool>
{
	bool operator()(const CPTR& x, const CPTR& y) const
		{ return strcasecmp(x, y) < 0; }
};

typedef map<CPTR, int, ConstCharPtrCaseLess> MapCPTR;

static int MinExt = 2;
static int MaxExt = 4;

struct ext_def {
	const char ** ext;
	int type;
};

static struct ext_def
ExtDef [] = {
	{ ext_text, EXT_TXTENC },
	{ ext_binaries, EXT_BINARIES },
	{ ext_comb, EXT_COMB },
	{ ext_video, EXT_VIDEO },
	{ ext_photo, EXT_PHOTO },
	{ ext_sound, EXT_SOUND },
	{ NULL, EXT_UNKNOWN },
};

static struct ext_def
ThExtDef [] = {
	{ thext_sound, THEXT_SOUND },
	{ thext_wmv, THEXT_WMV },
	{ thext_movie, THEXT_MOVIE },
	{ thext_mpg, THEXT_MPG },
	{ thext_real, THEXT_REAL },
	{ NULL, EXT_UNKNOWN },
};

//
static void
MakeExtMap (MapCPTR & ExtMap, struct ext_def * def)
{
	for(unsigned i = 0; def[i].ext != NULL; i++)
	{
		int type = def[i].type;
		for(const char ** pp = def[i].ext; *pp; ++pp)
			ExtMap[*pp] = type;
	}

	if(debug >= 2)
	{
		for(MapCPTR::iterator iter = ExtMap.begin();
			iter != ExtMap.end(); iter++)
		{
			DMSG(3, "%s = %d", (*iter).first, (*iter).second);
		}
	}
}

static int
LookupMap (const char * p, MapCPTR & ExtMap, struct ext_def * def)
{
	char ext[MaxExt+1];
	int i;
	for(i = 0; i <= MaxExt; i++)
	{
		if(p[i] == 0 || (!isdigit(p[i]) && !isalpha(p[i])))
		{
			if(i < MinExt)
				return EXT_UNKNOWN;

			ext[i] = 0;

			if(ExtMap.size() == 0)
				MakeExtMap(ExtMap, def);

			MapCPTR::iterator iter = ExtMap.find(ext);
			if(iter == ExtMap.end())
				return EXT_UNKNOWN;

			return (*iter).second;
		}

		ext[i] = tolower(p[i]);
	}

	return EXT_UNKNOWN;
}

int
LookupExtMap (const char * p)
{
	static MapCPTR ExtMap;
	return LookupMap(p, ExtMap, ExtDef);
}

int
LookupThumbExtMap (const char * p)
{
	static MapCPTR ThExtMap;
	return LookupMap(p, ThExtMap, ThExtDef);
}

//
//
static const char *
MultiPartNum (const char* p, const char** ppNum, const char** ppTotal)
{
	while( *p && !isdigit(*p) ) p++;
	*ppNum = p;

	while( *p && isdigit(*p) ) p++;
	while( *p && !isdigit(*p) ) p++;
	*ppTotal = p;

	while( *p && isdigit(*p) ) p++;
	return p;
}

//
//
static const char *
IndexMulti (const char * p)
{
	bool bSkip;
	for(;*p;p++)
	{
		if(*p == '(' || *p == '[' || strncasecmp(p, "day", 3) == 0
			|| strncasecmp(p, "file", 4) == 0)
		{
			const char * q;
			if (*p == '(' || *p == '[')
			{
				q = p+1;
				bSkip = false;
			}
			else
			{
				if(strncasecmp(p, "day", 3) == 0)
					q = p + 3;	// day xx of xx
				else
					q = p + 4;	// file xx of xx
				while(isspace(*q)) ++q;
				bSkip = true;
			}

			if(!isdigit(*q))
				continue;
			while(isdigit(*++q)) ;

			if(*q == '/')
			{
				++q;
			}
			else
			{
				while(isspace(*q)) ++q;
				if(strncasecmp(q, "of", 2) != 0)
					continue;
				q += 2;
				while(isspace(*q)) ++q;
			}

			if(!isdigit(*q))
				continue;
			while(isdigit(*++q)) ;

			if (*q == 0 || bSkip)
				return p;

			if(*q == ')' || *q == ']')
				return p;
		}
	}

	return NULL;
}

bool
NewsSubject::GetMultiSortSubj (ZString & sortsubj,
	int * pnTotal, int * pnNum,
	int iTotal, int iNum, int minTotal,
	int exttype, int extpos,
	ZString * pextstg)
{
	const char * pNum;
	int nTotal;
	bool ret = IsMultiPart(pnNum, &nTotal,
				&pNum, NULL,
				1, 1,
				iNum, iTotal,
				exttype, extpos);
	if(ret == false || nTotal < minTotal)
		return false;

	if(pnTotal) *pnTotal = nTotal;

	// Find file ext
	if (exttype == EXT_UNDEF)
	{
		extpos = 0;
		exttype = GetFileExt(&extpos);
	}

	if ((pextstg) && (exttype != EXT_UNKNOWN) && (extpos != 0))
	{
		int begextstg = CheckCombPart(m_stg.chars(), extpos)+1;
		int endextstg;
		for(endextstg = extpos+1;
			isalpha(m_stg[endextstg]) || isdigit(m_stg[endextstg]);
			endextstg++) ;
		*pextstg = m_stg.substr(begextstg, endextstg-begextstg);

		// Strip file extension
		// 'abc file.part001.rar [1/11] -> 'abc file. [1/11]'
		sortsubj = m_stg.substr(0, begextstg);
		sortsubj += (m_stg.chars()+endextstg);
	}
	else
	{
		sortsubj = m_stg;
	}

#if 0
	// ??
	// Remove the part numbers
	sortsubj = m_stg.substr(0, pNum - m_stg.chars());
	while(isdigit(*pNum)) pNum++;
	sortsubj += pNum;
#endif

	// Remove all part numbers!!
	ZString newsubj;
	for(;;)
	{
		pNum = IndexMulti(sortsubj.chars());
		if (pNum == NULL)
			break;
		while(!isdigit(*pNum)) pNum++;
		newsubj = sortsubj.substr(0, pNum - sortsubj.chars());
		while(isdigit(*pNum)) pNum++;
		while(isspace(*pNum) || (*pNum == '/')) pNum++; 
		while(isdigit(*pNum)) pNum++;
		newsubj += pNum;
		sortsubj = newsubj;
	}
	return true;
}

bool
NewsSubject::IsMultiPart (int *pnNum, int* pnTotal,
	const char** ppNum, const char **ppTotal,
	int min, int minTotal,
	int iNum, int iTotal,
	int exttype, int extpos)
{
	const char *pNum = NULL;
	const char *pTotal = NULL;
	int nNum = 0;
	int nTotal = 0;

	if ((nNum == 0) || (nTotal == 0)
	    || (ppTotal != NULL) || (ppNum != NULL))
	{
		const char *ptNum, *ptTotal;
		int ntNum, ntTotal;

		bool bR = false;
		const char *p = m_stg.chars();

		const char * ext = NULL;
		if (exttype == EXT_UNDEF)
			exttype = GetFileExt(&extpos);
		
		if (exttype != EXT_UNKNOWN)
		{
			p += extpos;
			ext = p;
		}
		else
			extpos = 0;

		const char *q = p;
		for(const char *r;;)
		{
			r = IndexMulti(q);
			if(r == NULL)
			{
				if (extpos == 0)
					break;

				q = p-extpos;
				extpos = 0;
				continue;
			}

			if ((*r != '[') && (*r != '('))
			{
				q = r+1;
				continue;
			}

			q = MultiPartNum(r, &ptNum, &ptTotal);
			ntNum = atoi(ptNum);
			ntTotal = atoi(ptTotal);
			if (iNum && (iNum != ntNum))
				continue;
			if (iTotal && (iTotal != ntTotal))
				continue;
				
			if (ntNum <= ntTotal)
			{
				bR = (ntNum >= min && ntTotal >= minTotal);
				if(bR)
				{
					nNum = ntNum;
					nTotal = ntTotal;
					pNum = ptNum;
					pTotal = ptTotal;
				}

				if (ext)
					break;
			}
		}

		if(!bR)
			return false;
	}

	if(pnNum)
		*pnNum = nNum;
	if(pnTotal)
		*pnTotal = nTotal;
	if(ppNum)
		*ppNum = pNum;
	if(ppTotal)
		*ppTotal = pTotal;
	return true;
}

int
NewsSubject::GetFileExt (int * pidx)
{
	int i;
	if(pidx == NULL)
		pidx = &i;

	int r = EXT_UNKNOWN;

	int e;
	const char * p = stg().chars();
	const char * q;
	for(unsigned n = 0; n < stg().length(); n++)
	{
		if(p[n] == '.' && *(q = p+n+1))
		{
			if(q[1] && q[2] &&
				isdigit(q[1]) && isdigit(q[2]) &&
				(q[3] == 0 ||
				  (!isdigit(q[3]) && !isalpha(q[3])
					&& (q[3] != '.'))))
			{
				r = EXT_COMB;
				*pidx = n;
				n += 2;		// 3 char ext
			}
			else
			{
				e = LookupExtMap(q);
				if(e != EXT_UNKNOWN)
				{
					r = e;
					*pidx = n++;
						// ++ -> ext at least 2 char
				}
			}
		}
	}

	return r;
}

const char *
NewsSubject::GetExpIcon ()
{
	m_pExpIcon = NULL;
	m_pIconTip = NULL;
	
	const char * p = stg().chars();

	if(p == NULL || *p == '\0')
		return NULL;

	const char * q = p;
	if(strncmp(q, "Re:", 3) == 0)
	{
		// skip the "Re:" and spaces
		q = q+3;
		while(*q && isspace(*q)) q++;
	}

	int nIconLen = 3;
	switch(*q)
	{
	case ':':
		if(*(q+1) == '-')
		{
			switch(*(q+2))
			{
			case ')':
				m_pExpIcon = "smile.gif";
				m_pIconTip = "Smile";
				break;
			case '>':
				m_pExpIcon = "sarcastic.gif";
				m_pIconTip = "Sarcastic";
				break;
			case 'D':
				m_pExpIcon = "laugh.gif";
				m_pIconTip = "Laughing";
				break;
			case 'P':
				m_pExpIcon = "sticktongue.gif";
				m_pIconTip = "Sticking tongue out";
				break;
			case '<':
				m_pExpIcon = "sad.gif";
				m_pIconTip = "Sad";
				break;
			case '@':
				m_pExpIcon = "angryyell.gif";
				m_pIconTip = "Angry and yelling";
				break;
			case 'O':
				m_pExpIcon = "surprise.gif";
				m_pIconTip = "Surprised or shocked";
				break;
			case '/':
				m_pExpIcon = "skeptical.gif";
				m_pIconTip = "Skeptical";
				break;
			case 'Z':
				m_pExpIcon = "sleep.gif";
				m_pIconTip = "Sleeping";
				break;
			case 'I':
				if(*(q+3) == 'I')
				{
					nIconLen = 4;
					m_pExpIcon = "angry.gif";
					m_pIconTip = "Angry";
				}
				break;
			}
		}
		break;
	case ';':
		if(*(q+1) == '-' && *(q+2) == ')')
		{
			m_pExpIcon = "wink.gif";
			m_pIconTip = "Wink";
		}
		break;
	case '}':
		if(*(q+1) == '-' && *(q+2) == ')')
		{
			m_pExpIcon = "evil.gif";
			m_pIconTip = "Evil";
		}
		break;
	case '?':
		if(*(q+1) == '-' && *(q+2) == '(')
		{
			m_pExpIcon = "dontknow.gif";
			m_pIconTip = "Sorry, I don't know what went wrong";
		}
		break;
	}

	if(m_pExpIcon == NULL)
		return NULL;

	// build a new subject with the expression characteres
	if(q > p)
		m_ExpStg = ZString(p, q-p);
	q += nIconLen;
	while(*q && isspace(*q)) q++;
	m_ExpStg += q;
	return m_pExpIcon;
}


//
int
NewsSubject::GetBinArcSubj (char * buf, string * ps,
	bool bCOMB, int exttype, int extpos)
{
	if (exttype == EXT_UNDEF)
	{
		exttype = GetFileExt(&extpos);
		if (exttype == EXT_UNKNOWN)
			return exttype;
	}

	if (!bCOMB && (exttype == EXT_COMB))
		return exttype;

	const char * p = stg();
	const char * r;

	const char * q = p+extpos-1;			// before file ext
	for(; q >= p; q--)
	{
		if(isspace(*q)) break;
	}

	char * bp = buf;
	for(r = p; r < q; r++)
		if(isalpha(*r) || *r < 0) *bp++ = *r;

	r = p+extpos+1;
	while(isdigit(*r) || isalpha(*r)) r++;
	q++;

	if (ps) *ps = stg().substr(q-p, r-q);

	for(; *r; r++)
		if(isalpha(*r) || *r < 0) *bp++ = *r;

	*bp = 0;

	return exttype;
}

int 
CheckCombPart (const char *f, int m)
{
	for(int k = m-1; k > 0; k--)
	{
		if(f[k] == '.')
		{
			const char * q = f+k+1;

			// remove .volXXXX[+_]XXX., mainly par2
			if (strncasecmp(q, "vol", 3) == 0)
			{
				q += 3;
				while(isdigit(*q)
					|| (*q == '+') || (*q == '_'))
					q++;
				if (*q == '.')
					m = k;
				continue;
			}

			// remove .partXXXXX., mainly rar
			if (strncasecmp(q, "part", 4) == 0)
				q += 4;

			while(isdigit(*q)) q++;

			if (*q == '.')
				m = k;
			break;
		}
	}

	return m;
}

const char *
SepFileRoot (string * pf)
{
	int m;
	const char * f = pf->c_str();
	for(m = pf->length()-1; m >= 0; m--)
		if(f[m] == '.') break;

	if (m < 0)
		return NULL;

	m = CheckCombPart(f, m);

	(*pf)[m] = 0;
	return f+m+1;
}

int
NewsSubject::GetFileSpec (string &fname,
	const char **ppExt, int * pnPart, int * pnTotal)
{
	int extpos = -1;
	int exttype =  GetFileExt(&extpos);
	char atype = (exttype == EXT_UNKNOWN) ? '1' : '2';

	*pnPart = 0;
	*pnTotal = 0;
	if (IsMultiPart(pnPart, pnTotal, NULL, NULL, 1, 2))
	{
		if (*pnPart > 1)
			atype = '3';
		else if (*pnTotal > 1)
			atype = '2';
	}

	*ppExt = ::GetFileSpec(stg(), extpos, fname);

	return atype;
}

const char *
GetFileSpec (const char * pSubj, int extpos, string &fname)
{
	if (extpos < 0)
		return NULL;

	const char * idx = pSubj + extpos;
	const char * f;
	for(f = idx-1;
		    (f >= pSubj) && (isalpha(*f) ||
			isdigit(*f) || *f == '_' ||
				*f == '.' || *f == '+' ||
				*f == '-' || *f == ',' ||
				*f == '(' || *f == ')' ||
				*f == '[' || *f == ']' ||
				(unsigned char) *f >= 0xa0);
			    f--) ;
	fname = string(f+1, idx-(f+1));
	fname += '.';
	int n = fname.length();
	for (++idx; isalpha(*idx) || isdigit(*idx); ++idx) fname += *idx;

	return fname.c_str()+n;
}
