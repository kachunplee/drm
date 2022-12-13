#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>

#include "def.h"

#include "mailsettings.h"

MailSettings::MailSettings (const char* pUser)
{
	m_user = pUser;
	char * pName = getenv("SERVER_NAME");
	pName = getenv("HTTP_HOST");
	m_stgDftSound = getenv("SERVER_NAME");
	if(m_stgDftSound.length() > 0)
	{
		BOOL bHTTPS = FALSE;
		char * pHTTPS = getenv("HTTPS");
		if(pHTTPS && (strcmp(pHTTPS, "on") == 0))
			bHTTPS = TRUE;
		m_stgDftSound = (bHTTPS?"https://":"http://") + m_stgDftSound;
	}
	m_stgDftSound += "/drm/newmail.wav";
	
	m_pFrom = NULL;
	m_pSoundSrc = (char *)m_stgDftSound.chars();
	m_nCheckTime = 0;
	m_pBuffer = NULL;

	ZString stgTmp = DIR_LOCAL;
	stgTmp += pUser;
	stgTmp += "/.fromrc";
	int fd = open(stgTmp, O_RDONLY|O_EXLOCK, 0660);
	if(fd >= 0)
	{
		struct stat sb;
		if(fstat(fd, &sb) == 0)
		{
			m_pBuffer = new char[sb.st_size+1];
			read(fd, m_pBuffer, sb.st_size);
			m_pBuffer[sb.st_size] = 0;
			char * pNext = m_pBuffer;
			char * pBuf;
			while(pNext && *pNext)
			{
				pBuf = pNext;
				pNext = strchr(pBuf, '\n');
				if(pNext) *pNext++ = 0;
				if(strncmp(pBuf, "from", 4) == 0)
				{
					m_pFrom = strchr(pBuf, '\t');
					if(m_pFrom) m_pFrom++;
				}
				else if(strncmp(pBuf, "soundsrc", 8) == 0)
				{
					m_pSoundSrc = strchr(pBuf, '\t');
					if(m_pSoundSrc) m_pSoundSrc++;
					if(*m_pSoundSrc == 0)
						m_pSoundSrc = NULL;
				}
				else if(strncmp(pBuf, "checktime", 9) == 0)
				{
					pBuf = strchr(pBuf, '\t');
					if(pBuf)
					{
						pBuf++;
						m_nCheckTime = atoi(pBuf);
					}
				}
				else if(strncmp(pBuf, "moderate", 8) == 0)
				{
					m_pModerate = strchr(pBuf, '\t');
					if(m_pModerate) m_pModerate++;
				}
			}
		}
		close(fd);
	}
}

MailSettings::~MailSettings ()
{
	if(m_pBuffer)
		delete [] m_pBuffer;
}

const char *
MailSettings::GetAlias ()
{
	const char * p = GetFrom();
	if (p) return p;

#ifdef CMDGETALIAS
	int olduid = getuid();
	setuid(geteuid());	// getalias is a perl program
				//make uid = euid, so not to trigger taint mode

	string cmd = CMDGETALIAS;
	cmd += ' ';
	cmd += m_user;
	FILE * pp = popen(cmd.c_str(), "r");
	if (pp == NULL)
	{
		if (m_alias.find("@") < m_alias.length())
			return m_alias.c_str();

		return m_user.c_str();
	}

	char buf[1024];
	p =  fgets(buf, sizeof(buf), pp);
	if (p)
	{
		m_alias = p;

		pclose(pp);
		setuid(olduid);

		m_alias.gsub("\\", "");
		m_alias.gsub("\n", "");
		if (m_alias.find('@') < m_alias.length())
			return m_alias.c_str();
	}
	else
	{
		pclose(pp);
		setuid(olduid);
	}
#endif
	return m_user.c_str();
}
