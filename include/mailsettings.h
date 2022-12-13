#ifndef __MAILSETTINGS_H__
#define __MAILSETTINGS_H__

class MailSettings
{
protected:
	ZString		m_user;
	ZString		m_alias;
	ZString		m_stgDftSound;
	char *		m_pBuffer;
	char *	m_pFrom;
	const char *	m_pSoundSrc;
	const char *	m_pModerate;
	int		m_nCheckTime;

public:
	MailSettings (const char *);
	~MailSettings();

	char * GetFrom ()			{ return m_pFrom; }
	const char * GetSoundSrc ()		{ return m_pSoundSrc; }
	const char * GetModerate ()		{ return m_pModerate; }
	int GetCheckTime ()			{ return m_nCheckTime; }

	const char * GetAlias();
};

#endif //__MAILSETTINGS_H__
