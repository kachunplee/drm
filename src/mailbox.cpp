#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#include "def.h"

#include "mboxlock.h"
#include "mailbox.h"

static bool
MMakeDir (const char * Name, mode_t mode)
{
    struct stat		Sb;

    /* See if it failed because it already exists. */
    if(stat(Name, &Sb) >= 0 && S_ISDIR(Sb.st_mode))
	return true;

    int ret = mkdir(Name, mode);
    if (ret >= 0)
	chown(Name, HttpID, (unsigned) -1);

    return (ret >= 0);
}

/*
*/
static bool
MMakeDirP (const char * Name, mode_t mode)
{
    // Optimize common case -- parent almost always exists.
    const char *p = strrchr(Name, '/');
    ZString * pDir = new ZString(Name, p-Name);
    if (MMakeDir(pDir->chars(), mode))
	return true;

    // Well, try to create parent directories
    char buf [strlen(Name)+1];
    p = Name;
    char * q = buf;
    if (*p == '/') *q++ = *p++;

    for (; *p; p++, q++)
    {
	if (*p == '/') {
	    *q = '\0';
	    if(!MMakeDir(buf, mode))
		return false;
	    else
	    {
		if (*(p+1) && strchr(p+1, '/') == NULL)
			return true;
	    }
	}
	*q = *p;
    }

    return MMakeDir(Name, mode);
}

MailBox::MailBox (const char * pUser, const char * pBoxName)
{
	m_pUser = pUser;
	if (strcasecmp(pBoxName, "in") == 0)
	{
		m_BoxFile = DIR_MAIL;
		m_BoxFile += m_pUser;
	}
	else
	{
		m_BoxFile = DIR_LOCAL;
		m_BoxFile += m_pUser;
		struct stat sb;
		if(stat(m_BoxFile, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				// Create the local mailbox dir
				MMakeDirP(m_BoxFile, 0755);
			}
		}
		m_BoxFile += "/mail";
		if(stat(m_BoxFile, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				MMakeDirP(m_BoxFile, 0755);
			}
		}
		m_BoxFile += "/";
		m_BoxFile += pBoxName;
		if(stat(m_BoxFile, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				MMakeDirP(m_BoxFile, 0755);
			}
		}
	}

	m_MailStatus = MS_NOMAIL;
	m_pMMap = NULL;
	m_nMMap = 0;
	m_fdMBox = -1;

	m_pCurMMap = NULL;
	m_nCurMail = 0;

	m_Info.msgLength = 0;
	m_Info.msgMsg = NULL;

	m_bNewMail = FALSE;

	m_lockMBox = NULL;
}

MailBox::~MailBox ()
{
	UnMMap();
	UnlockFile(m_fdMBox);
	delete m_Info.msgMsg;

	delete m_lockMBox;
	m_lockMBox = NULL;
}


bool
MailBox::MMap ()
{
	if (m_pMMap)
		return true;
#if 0
	m_lockMBox = new MBoxLock();
	if (!m_lockMBox->Lock(m_BoxFile))
	{
		m_MailStatus = MS_BUSY;
		return false;
	}
#endif

	if(!LockMailFile(m_fdMBox))
		return false;		// can't lock the mailbox file

	m_MailStatus = MapMailbox(m_fdMBox) ? MS_MAIL : MS_MAPERR;

	m_pCurMMap = m_pMMap;
	m_nCurMail = 0;

	return m_MailStatus == MS_MAIL;
}

void MailBox::UnMMap ()
{
	if(m_pMMap != NULL)
	{
		munmap(m_pMMap, m_nMMap);
		m_pMMap = NULL;
		m_nMMap = 0;
		m_pCurMMap = NULL;
		m_nCurMail = 0;

		UnlockFile(m_fdMBox);

		delete m_lockMBox;
		m_lockMBox = NULL;
	}
}

BOOL
MailBox::LockMailFile (int& fd)
{
	if(fd > -1)
		return TRUE;	// mailbox file is already locked

	struct stat sb;
	if(stat(m_BoxFile, &sb) == -1)
	{
		m_MailStatus = MS_NOMAIL;
		return FALSE;
	}

	if(sb.st_size == 0)
	{
		m_MailStatus = MS_NOMAIL;
	}

	fd = open(m_BoxFile, O_RDWR|O_EXCL, 0660);
	if(fd == -1)
	{
		// can't open file, assume busy
		m_MailStatus = MS_BUSY;
		return FALSE;
	}

	if(flock(fd, LOCK_EX) == -1)
	{
		// file is locked by another process
		close(fd);
		fd = -1;
		m_MailStatus = MS_BUSY;
		return FALSE;
	}

	return TRUE;
}

void
MailBox::UnlockFile (int & fd)
{
	if(fd == -1)
		return;

	flock(fd, LOCK_UN);
	close(fd);
	fd = -1;
}


void MailBox::DeleteBox ()
{
	ZString stgTmp = m_BoxFile;
	stgTmp += ".old";
	RenameBox(stgTmp);
	remove(m_BoxFile);
}

void MailBox::UpdStatus (struct MsgInfo* pInfo, char cStatus, BOOL bPreview)
{
	caddr_t pMMap = GetMMap();
	// Update mail status
	ZString stgTmp = DIR_LOCAL;
	stgTmp += m_pUser;
	stgTmp += "/";
	stgTmp += m_pUser;
	int fdTemp = open(stgTmp, O_RDWR|O_TRUNC|O_EXLOCK, 0660);
	if(fdTemp == -1)
	{
		if((fdTemp=open(stgTmp, O_CREAT|O_RDWR|O_EXCL, 0660)) == -1)
			return;
		chown(stgTmp, UserID, MailGID);
		chmod(stgTmp, 0660);
		flock(fdTemp, LOCK_EX);
	}
	char buf[LINESIZE+1];
	caddr_t p, q;
	int nWrite, nStatLen;

	// Update the mail status
	if(pInfo->msgStatOffset)
	{
		p = strchr(pMMap + pInfo->msgStatOffset, '\n');
		*p = 0;
		q = strchr(pMMap + pInfo->msgStatOffset, cStatus);
		if(q)
		{
			// Status is already exists
			*p = '\n';
			flock(fdTemp, LOCK_UN);
			remove(stgTmp.chars());
			return;
		}
		nStatLen = strlen(pMMap+pInfo->msgStatOffset);
		write(fdTemp, pMMap, pInfo->msgStatOffset);
		q = strchr(pMMap + pInfo->msgStatOffset, ' ');
		if (bPreview)
			nWrite = 0;
		else
		{
			buf[0] = 'O';
			nWrite = 1;
		}
		switch(cStatus)
		{
		case 'R':
			buf[nWrite++] = 'R';
			if(q)
			{
				q++;
				if(Frequent(q, 'F') > 0)
					buf[nWrite++] = 'F';
				if(Frequent(q, 'P') > 0)
					buf[nWrite++] = 'P';
			}
			break;

		case 'F':
		case 'P':
		case 'S':
			if(q)
			{
				q++;
				if(Frequent(q, 'R') > 0)
					buf[nWrite++] = 'R';
			}
			buf[nWrite++] = cStatus;
			break;
		}
		while(nWrite < 3)
		{
			buf[nWrite++] = ' ';
		}
		buf[nWrite] = 0;
		write(fdTemp, "Status: ", 8); 
		write(fdTemp, buf, nWrite); 
cerr << "updstatus to " << buf << "\n";
		*p = '\n';
		nWrite = m_nMMap - pInfo->msgStatOffset - nStatLen;
		write(fdTemp, p, nWrite);
	}
	else
	{
		nWrite = pInfo->msgOffset + pInfo->msgHeaderLength;
		write(fdTemp, m_pMMap, nWrite);
		write(fdTemp, "Status: O", 9);
		if(cStatus != 'O')
		{
			write(fdTemp, &cStatus, 1);
			write(fdTemp, " \n", 2);
		}
		nWrite = m_nMMap - pInfo->msgOffset - pInfo->msgHeaderLength;
		write(fdTemp, pMMap + pInfo->msgOffset +
			pInfo->msgHeaderLength, nWrite);
	}

	rename(stgTmp.chars(), m_BoxFile.chars());
	UnMMap();
	m_fdMBox = fdTemp;
}

static char emptystg [] = "";
BOOL MailBox::GetMailInfo (struct MsgInfo& mInfo, int nMailNo,
	const char * pID)
{
	if (pID == NULL) pID = emptystg;

	char buf[LINESIZE+1];

	// Use nMailNo to estimate search
	if ((nMailNo <= m_nCurMail)
	    || (m_pCurMMap == NULL)
	    || (strncmp(m_pCurMMap, "From ", 5) != 0)
			// Not pointing to the begin of a mail
			// Mail Box must have changed
	  )
	{
		SetFirstMail();
	}

	// Save searchEnd
	int searchEnd = m_nCurMail;

	for(;;)
	{
	    while(GetNextMail(mInfo, buf))
	    {
		++m_nCurMail;
		if (pID[0] == 0)
		{
			if (m_nCurMail == nMailNo)
				return true;

			continue;
		}

		const char * pMsgID =
				mInfo.msgMsg->GetHeader("message-id");
		if (pMsgID && (strcmp(pID, pMsgID) == 0))
			return true;

		if (m_nCurMail == searchEnd)
			return FALSE;
	    }

	    if ((searchEnd == 0) || (pID[0] == 0))
		  break;

	    SetFirstMail();
	}

	return FALSE;
}

BOOL MailBox::GetNextMail (struct MsgInfo& mInfo, char * pBuf)
{

	mInfo.msgStatus = -1;
	mInfo.msgLength = 0;
	mInfo.msgHeaderLength = 0;

	caddr_t pEndMap = GetMMap() + m_nMMap; // GetMMap with do mmap
	mInfo.msgOffset = m_pCurMMap - m_pMMap; // so from here on, m_pMMap is OK
	mInfo.msgStatOffset = 0;
	delete mInfo.msgMsg;
	mInfo.msgMsg = NULL;

	if(m_pCurMMap == pEndMap)
		return FALSE;		// At end of memory map

	int nMsg = 0;
	BOOL bMaybe = TRUE;
	BOOL bInHead = FALSE;
	caddr_t p, q;
	int nLen;
	char c;

	for (;;)
	{
		if((p = (char *)memchr(m_pCurMMap, '\n',
				pEndMap - m_pCurMMap)) == NULL)
			break;
		if((nLen = p - m_pCurMMap + 1) <= LINESIZE)
		{
			strncpy(pBuf, m_pCurMMap, nLen);
			pBuf[nLen-1] = 0;
			if(bMaybe && pBuf[0] == 'F')
			{
				if(IsHead(pBuf))
				{
					if(nMsg > 0)
					{
						mInfo.msgLength = m_pCurMMap -
							m_pMMap -
							mInfo.msgOffset;
						mInfo.msgMsg =
							new ZMIME(
								m_pMMap +
								mInfo.msgOffset,
								mInfo.msgLength
							     );
						return TRUE;
					}

					nMsg++;
					mInfo.msgHeaderLength = nLen;
					bInHead = TRUE;
				}
			}
			else if(pBuf[0] == 0)
			{
				bInHead = FALSE;
			}
			else if(bInHead)
			{
				switch(pBuf[0])
				{
				case 'S':
					if(strncasecmp(pBuf, "Status: ", 8) == 0)
					{
						// got subject header
						if(mInfo.msgStatus == -1)
						{
							mInfo.msgStatus = MSG_NEW;
							mInfo.msgStatOffset = m_pCurMMap - m_pMMap;
							q = pBuf + 6;
							while(isspace(*++q));
							while((c = *q++))
							{
								if(c == 'O')
									mInfo.msgStatus &= ~MSG_NEW;
								else if(c == 'R')
									mInfo.msgStatus |= MSG_READ;
								else if(c == 'F')
									mInfo.msgStatus = MSG_FORWARD;
								else if(c == 'P')
									mInfo.msgStatus = MSG_REPLY;
								else if(c == 'S')
									mInfo.msgStatus = MSG_SEND;
							}
						}
					}
				}
				mInfo.msgHeaderLength += nLen;
			}
			bMaybe = pBuf[0] == 0;
		}
		else
			bMaybe = *m_pCurMMap == '\n';
		m_pCurMMap = p + 1;
		if(m_pCurMMap == pEndMap)
		{
			// At end of memory map
			if(bInHead)
				return FALSE;

			mInfo.msgLength = m_pCurMMap - m_pMMap -
				mInfo.msgOffset;
			mInfo.msgMsg = new ZMIME(
				m_pMMap + mInfo.msgOffset, mInfo.msgLength);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL MailBox::GetCurMailInfo (int nMailNo, const char * pID)
{
	if(m_Info.msgMsg && (m_nCurMail == nMailNo))
	{
		if (pID[0] == 0)
			return TRUE;

		const char * pMsgID = m_Info.msgMsg->GetHeader("message-id");
		if ((pMsgID != NULL) && (strcmp(pID, pMsgID) == 0))
			return TRUE;

		delete m_Info.msgMsg;
		m_Info.msgLength = 0;
		m_Info.msgMsg = NULL;
	}

	return GetMailInfo(m_Info, nMailNo, pID);
}

//
//
//
bool
MailBox::AppendMail (const char * xfermail, int nLen)
{
	UnMMap();
	
	if(!LockMailFile(m_fdMBox))
	{
		if((m_fdMBox=open(m_BoxFile, O_CREAT|O_RDWR|O_EXCL, 0660))==-1)
		{
			cerr << m_pUser << " open " << m_BoxFile << " error: "
				<< strerror(errno) << endl;
			return false;
		}
		chown(m_BoxFile, UserID, MailGID);
		chmod(m_BoxFile, 0660);
		flock(m_fdMBox, LOCK_EX);
	}

	if (lseek(m_fdMBox, (off_t) 0, SEEK_END) < 0)
	{
		cerr << m_pUser << " seek " << m_BoxFile << " error: "
			<< strerror(errno) << endl;
		return false;
	}

	if (write(m_fdMBox, xfermail, nLen) < 0)
	{
		cerr << m_pUser << " write " << m_BoxFile << " error: "
			<< strerror(errno) << endl;
		return false;
	}

	fsync(m_fdMBox);
	return true;
}

void
MailBox::DeleteMail (Mails& dels)
{
	if(!LockMailFile(m_fdMBox))
	{
		cerr << m_pUser << " cannot lock " << m_BoxFile << " to delete: "
			<< strerror(errno) << endl;
		return;
	}
cerr << m_pUser << "delete a mail in " << m_BoxFile << endl;

	caddr_t delend = (caddr_t) -1;

	Mails::iterator iter = dels.begin();
	caddr_t mail_end = (*iter).first;
	for (Mails::iterator dels_end = dels.end();
		iter != dels_end; ++iter)
	{
		caddr_t mail = (*iter).first;
		int len = (*iter).second;
		if (delend == mail)
		{
			// combine region
			delend += len;
			continue;
		}

		if (delend != (caddr_t) -1)
		{
			// move region
			int reglen = mail - delend;
			memmove(mail_end, delend, reglen);
			mail_end += reglen;
		}

		delend = mail + len;
	}

	int reglen = m_pMMap + m_nMMap - delend;
	if (reglen > 0)
	{
		memmove(mail_end, delend, reglen);
		mail_end += reglen;
	}

cerr << m_pUser << " remove " << reglen << " bytes " << endl;

	reglen = mail_end - m_pMMap;
	UnMMap();
	truncate(m_BoxFile, reglen);

	SetFirstMail();
}

void MailBox::DeleteMail (caddr_t pMMap, int nLen)
{
	Mails dels;
	dels[pMMap] = nLen;
	DeleteMail(dels);
}

void MailBox::RenameBox (const char * pDestName)
{
	UnMMap();
	UnlockFile(m_fdMBox);

	struct stat sb;
	if(stat(m_BoxFile, &sb) != 0)
		return;		// the source box is not exist

	if(sb.st_size == 0)
		return;		// the source box is empty

	struct passwd * pPwdEnt = getpwnam("root");
	if(pPwdEnt == NULL)
		return;		// can't get the root passwd info

	// Rename the mailbox and chown to root
	if(rename(m_BoxFile, pDestName) == -1)
	{
		ZString stgTmp = DIR_LOCAL;
		stgTmp += m_pUser;
		struct stat sb;
		if(stat(stgTmp, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				// Create the local mailbox dir
				if (mkdir(stgTmp, 0755) < 0)
					return;
				chown(stgTmp, HttpID, (unsigned) -1);
			}
		}
		stgTmp += "/mail";
		if(stat(stgTmp, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				if (mkdir(stgTmp, 0755) < 0)
					return;
				chown(stgTmp, HttpID, (unsigned) -1);
			}
		}
		rename(m_BoxFile, pDestName);
	}
	chown(pDestName, pPwdEnt->pw_uid, (unsigned) -1);
}

BOOL MailBox::MapMailbox (int fd)
{
	// map mailbox into memory
	if(IsMMap())
		UnMMap();

	struct stat sb;
	if(fstat(fd, &sb) == -1)
	{
		m_MailStatus = MS_MAPERR;
		return FALSE;
	}

	if (sb.st_size == 0)
		return FALSE;

	m_nMMap = (size_t) sb.st_size;
	m_pMMap = (caddr_t) mmap(0, m_nMMap, PROT_READ|PROT_WRITE, MAP_SHARED,
		fd, 0);

	if(m_pMMap != (caddr_t)-1)
		return TRUE;

	m_pMMap = NULL;
	m_MailStatus = MS_MAPERR;
	return FALSE;
}

void MailBox::WriteBegin ()
{
	ZString stgTmp = DIR_LOCAL;
	stgTmp += m_pUser;
	struct stat sb;
	if(stat(stgTmp, &sb) == -1)
	{
		if(errno == ENOENT)
		{
			// Create the local mailbox dir
			if (mkdir(stgTmp, 0755) < 0)
				return;
			chown(stgTmp, HttpID, (unsigned) -1);
		}
	}

	stgTmp += "/mail";
	if(stat(stgTmp, &sb) == -1)
	{
		if(errno == ENOENT)
		{
			// Create the local mailbox dir
			if (mkdir(stgTmp, 0755) < 0)
				return;
			chown(stgTmp, HttpID, (unsigned) -1);
		}
	}

	if(!LockMailFile(m_fdMBox))
	{
		if((m_fdMBox=open(m_BoxFile, O_CREAT|O_RDWR|O_EXCL, 0660))==-1)
		{
			cerr << m_pUser << " open " << m_BoxFile << " error: "
				<< strerror(errno) << endl;
			return;
		}
		chown(m_BoxFile, UserID, MailGID);
		chmod(m_BoxFile, 0660);
		flock(m_fdMBox, LOCK_EX);
	}

	if (lseek(m_fdMBox, (off_t) 0, SEEK_END) < 0)
	{
		cerr << m_pUser << " seek " << m_BoxFile << " error: "
			<< strerror(errno) << endl;
		return;
	}
}

void MailBox::WriteData (const char * p, int nLen)
{
	if(m_fdMBox == -1)
		return;
	write(m_fdMBox, p, nLen);
}

void MailBox::WriteEnd ()
{
	UnlockFile(m_fdMBox);
}

BOOL MailBox::IsHead (char * pBuf)
{
	register char *cp;
	struct headline hl;
	char parbuf[LINESIZE];

	cp = pBuf;
	if (*cp++ != 'F' || *cp++ != 'r' || *cp++ != 'o' || *cp++ != 'm' ||
	    *cp++ != ' ')
		return FALSE;
	Parse(pBuf, &hl, parbuf);
	if (hl.l_from == NULL || hl.l_date == NULL) {
//		Fail(pBuf, "No from or date field");
		return FALSE;
	}
	if (!IsDate(hl.l_date)) {
//		Fail(pBuf, "Date field not legal date");
		return FALSE;
	}
	return TRUE;
}

void MailBox::Parse (char * pLine, struct headline * phl, char * pBuf)
{
	register char *cp;
	char *sp;
	char word[LINESIZE];

	phl->l_from = NULL;
	phl->l_tty = NULL;
	phl->l_date = NULL;
	cp = pLine;
	sp = pBuf;
	/*
	 * Skip over "From" first.
	 */
	cp = NextWord(cp, word);
	cp = NextWord(cp, word);
	if (*word)
		phl->l_from = CopyIn(word, &sp);
	if (cp != NULL && cp[0] == 't' && cp[1] == 't' && cp[2] == 'y') {
		cp = NextWord(cp, word);
		phl->l_tty = CopyIn(word, &sp);
	}
	if (cp != NULL)
		phl->l_date = CopyIn(cp, &sp);
}

char * MailBox::NextWord (char * wp, char *wbuf)
{
	register char c;

	if (wp == NULL) {
		*wbuf = 0;
		return NULL;
	}
	while ((c = *wp++) && c != ' ' && c != '\t') {
		*wbuf++ = c;
		if (c == '"') {
 			while ((c = *wp++) && c != '"')
 				*wbuf++ = c;
 			if (c == '"')
 				*wbuf++ = c;
			else
				wp--;
 		}
	}
	*wbuf = '\0';
	for (; c == ' ' || c == '\t'; c = *wp++)
		;
	if (c == 0)
		return NULL;
	return (wp - 1);
}

char * MailBox::CopyIn (char * src, char **space)
{
	register char *cp;
	char *top;

	top = cp = *space;
	while((*cp++ = *src++))
		;
	*space = cp;
	return (top);
}

/*
 * 'A'	An upper case char
 * 'a'	A lower case char
 * ' '	A space
 * '0'	A digit
 * 'O'	An optional digit or space
 * ':'	A colon
 * 'N'	A new line
 */
char ctype[] = "Aaa Aaa O0 00:00:00 0000";
char tmztype[] = "Aaa Aaa O0 00:00:00 AAA 0000";

int MailBox::IsDate (char * date)
{

	return Cmatch(date, ctype) || Cmatch(date, tmztype);
}

/*
 * Match the given string (cp) against the given template (tp).
 * Return 1 if they match, 0 if they don't
 */
int MailBox::Cmatch (char * cp, char *tp)
{

	while (*cp && *tp)
		switch (*tp++) {
		case 'a':
			if (!islower(*cp++))
				return 0;
			break;
		case 'A':
			if (!isupper(*cp++))
				return 0;
			break;
		case ' ':
			if (*cp++ != ' ')
				return 0;
			break;
		case '0':
			if (!isdigit(*cp++))
				return 0;
			break;
		case 'O':
			if (*cp != ' ' && !isdigit(*cp))
				return 0;
			cp++;
			break;
		case ':':
			if (*cp++ != ':')
				return 0;
			break;
		case 'N':
			if (*cp++ != '\n')
				return 0;
			break;
		}
	if (*cp || *tp)
		return 0;
	return (1);
}

int MailBox::Frequent (char * q, char c)
{
	int nCount = 0;
	while(*q)
	{
		if(*q++ == c)
			nCount++;
	}
	return nCount;
}
