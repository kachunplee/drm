#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <dirent.h>

#include <map>

#include "def.h"

#include "mimetext.h"
#include "mailopt.h"
#include "mailbox.h"
#include "maillist.h"

extern "C"
{
#include "mime_priv.h"
}

extern char * strtohex(char *, char *);

ostream& operator << (ostream& stm, MailList& list)
{
	list.OutBegin(stm);

	list.m_nOut = 0;

	switch(list.m_pMailbox->GetMailStatus())
	{
	case MS_NOMAIL:
		list.NoMail(stm);
		break;

	case MS_BUSY:
		list.MBoxBusy(stm);
		break;

	default:
		list.PrintList(stm);
	}

	list.OutEnd(stm);
	return stm;
}

MailList::MailList (const char * name, MailOption * pOpt, MailBox* pBox, const char* pUser, int sortopt)
	: m_name(name), m_pOpt(pOpt), m_pMailbox(pBox), m_pUser(pUser)
{
	m_bError = FALSE;
	m_mail = 0;
	m_read = 0;
	m_boxName = m_name;
	m_boxName[0] = toupper(m_boxName[0]);
}

MailList::~MailList()
{
}

void
MailList::PrintList (ostream& stm)
{
	struct MsgInfo mInfo;
	mInfo.msgLength = 0;
	mInfo.msgMsg = NULL;
	char buf[LINESIZE+1];
	int nIdx = 1;

	map<string, MailEntry> maplist;
	bool rev = false;

	unsigned int nBuf = 1024;
	char * pBuf = new char[nBuf];

	m_mail = m_read = 0;

	INDateTime().SetTZ(m_pOpt->GetTZName());

	// memory map available, use it to display the mail list
	for(m_pMailbox->SetFirstMail();
		m_pMailbox->GetNextMail(mInfo, buf);
		nIdx++)
	{
		if(mInfo.msgStatus == -1)
			mInfo.msgStatus = MSG_NEW;

		m_mail++;
		if (mInfo.msgStatus & MSG_READ) m_read++;

		MailEntry ent;
		ent.m_idx = nIdx;
		ent.m_status = mInfo.msgStatus;
		ent.m_size =  mInfo.msgLength;

		ent.m_who = (strcasecmp(m_name, "out") == 0)
			? gMimeText.GetText(mInfo.msgMsg->GetHeaderC("to"))
			: gMimeText.GetText(mInfo.msgMsg->GetHeaderC("from"));
		char * p = (char *)mInfo.msgMsg->GetHeaderC("message-id");
		while ((strlen(p) * 2) > nBuf)
		{
			delete [] pBuf;
			nBuf += 1024;
			pBuf = new char[nBuf];
		}
		ent.m_msgid = strtohex(p, pBuf);
		ent.m_date = mInfo.msgMsg->GetHeaderC("date");
		ent.m_subject = gMimeText.GetText(mInfo.msgMsg->GetHeaderC("subject"));
		ent.m_hasATT = mInfo.msgMsg->HasAttachment();
		
		time_t t;
		switch (m_pOpt->GetSortOpt())
		{
		case SORT_DATE_R:
			rev = true;
		case SORT_DATE_A:
			t = INDateTime(ent.m_date.c_str()).GetTime();
			snprintf(buf, sizeof(buf),
				"%010lu\t%019d", (unsigned long) t, nIdx);
			maplist[buf] = ent;
			break;

		case SORT_SUBJ_R:
			rev = true;
		case SORT_SUBJ_A:
			snprintf(buf, sizeof(buf),
				"%s\t%019d", ent.m_subject.c_str(), nIdx);
			for (char * p = buf; *p; p++)
				*p = tolower(*p);	
			maplist[buf] = ent;
			break;

		case SORT_WHO_R:
			rev = true;
		case SORT_WHO_A:
		    {
			ZString szWho;
			INMailName(ent.m_who.c_str()).RealName(szWho);
			snprintf(buf, sizeof(buf),
				"%s\t%019d", szWho.c_str(), nIdx);
			for (char * p = buf; *p; p++)
				*p = tolower(*p);	
			maplist[buf] = ent;
		   }
			break;

		default:
			PrintLine(stm, ent);
		}

	}
	delete [] pBuf;

	if (m_pOpt->GetSortOpt())
	{
		if (rev)
		{
			map<string, MailEntry>::reverse_iterator iter;
			map<string, MailEntry>::reverse_iterator listend
				= maplist.rend();
			for (iter = maplist.rbegin(); iter != listend; iter++)
				PrintLine(stm, (*iter).second);
		}
		else
		{
			map<string, MailEntry>::iterator iter;
			map<string, MailEntry>::iterator listend
				= maplist.end();
			for (iter = maplist.begin(); iter != listend; iter++)
				PrintLine(stm, (*iter).second);
		}
	}
}

ostream &
MailList::OutDate (ostream& stm, const char * p)
{
	tm t = INDateTime(p).localtime();
	if (strcmp(m_pOpt->GetDateFormat(), "dd/mm") == 0)
		stm.form("%02d/%02d/%02d", t.tm_mday, t.tm_mon+1,
			(t.tm_year+1900)%100);
	else
		stm.form("%02d/%02d/%02d", t.tm_mon+1, t.tm_mday,
			(t.tm_year+1900)%100);
	stm.form(" %02d:%02d", t.tm_hour, t.tm_min);
	return stm;
}
