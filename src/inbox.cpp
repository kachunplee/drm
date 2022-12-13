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

#include "inbox.h"

InBox::InBox (const char * pUser)
	: MailBox(pUser, "in")
{
	m_fdPOP = -1;
}

InBox::~InBox ()
{
}

BOOL
InBox::LockMailFile (int& fd)
{
	if(m_fdPOP > -1)
		return TRUE;	// mailbox file is already locked

	ZString stgTmp = DIR_MAIL;
	stgTmp += ".";
	stgTmp += m_pUser;
	stgTmp += ".pop";
	m_fdPOP = open(stgTmp, O_RDWR|O_EXCL, 0600);
	if(m_fdPOP == -1)
	{
		if(errno == ENOENT)	// pop file doesn't exits, create one
		{
			if((m_fdPOP = open(stgTmp, O_RDWR|O_CREAT|O_EXCL, 0660))>-1)
			{
				chown(stgTmp, UserID, MailGID);
				chmod(stgTmp, 0660);
			}
		}
		if(m_fdPOP == -1)
		{
			m_MailStatus = MS_BUSY;
			return FALSE;
		}
	}
	if(flock(m_fdPOP, LOCK_EX) == -1)
	{
		// file is locked by another process
		close(m_fdPOP);
		m_fdPOP = -1;
		m_MailStatus = MS_BUSY;
		return FALSE;
	}

	return MailBox::LockMailFile(fd);
}

void
InBox::UnlockFile (int &fd)
{
	MailBox::UnlockFile(fd);
	MailBox::UnlockFile(m_fdPOP);
}

void InBox::DeleteBox ()
{
	ZString stgTmp = DIR_LOCAL;
	stgTmp += m_pUser;
	stgTmp += "/";
	stgTmp += m_pUser;
	stgTmp += ".old";
	RenameBox(stgTmp);
	remove(m_BoxFile);
}

void InBox::UpdStatus (struct MsgInfo* pInfo, char cStatus, BOOL bPreview)
{
	MailBox::UpdStatus(pInfo, cStatus, bPreview);
	chown(m_BoxFile, UserID, MailGID);
	chmod(m_BoxFile, 0660);
}
