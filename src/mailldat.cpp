#include <sys/time.h>
#include <stdio.h>

#include "def.h"
#include "mailopt.h"
#include "mailbox.h"
#include "mailldat.h"

void
MailListData::NoMail (ostream &stm)
{
#if 0
	stm << "No mail in your " << m_boxName << " box<p>" << endl;
#endif
	m_bError = TRUE;
}

void
MailListData::MBoxBusy (ostream &stm)
{
#if 0
	stm << "Another session is accessing your " << m_boxName
		<< " box, please retry later.<p>" << endl;
#endif
	m_bError = TRUE;
}

void
MailListData::OutBegin (ostream& stm)
{
}

void
MailListData::OutEnd (ostream& stm)
{
#if 0
	const char * pBoxName = m_name;
	stm <<
		"<script language=javascript>\n"
		"<!--\n"
		"if (top.UpdFolder)\n"
		"{\n"
		"	top.UpdFolder('" << pBoxName << "', "
				<< m_read << ", " << m_mail - m_read << ");\n"
		"}\n"
		"//-->\n"
		"</script>\n"
	;
#endif
}

void
MailListData::PrintLine (ostream& stm, MailEntry &ent)
{
	//
	// MsgID
	//
	const char * pMsgID = ent.m_msgid.c_str();
	if(pMsgID) pMsgID = URLText(pMsgID).GetText();

	//
	// Mail status
	int status = 0;
	if(ent.m_status & MSG_SEND)
		status = 1;
	else if(ent.m_status & MSG_REPLY)
		status = 2;
	else if(ent.m_status & MSG_FORWARD)
		status = 3;
	else if(ent.m_status & MSG_READ)
		status = 4;

	//
	// Size
	//
	char sizebuf [20];
	int size = ent.m_size;
	if (size < 1024)
	{
		snprintf(sizebuf, sizeof(sizebuf), "%d byte", size);
	}
	else
	{
		size /= 1024;
		if (size < 1000)
		{
			snprintf(sizebuf, sizeof(sizebuf),
				"%dK byte", size);
		}
		else
		{
			snprintf(sizebuf, sizeof(sizebuf),
				"%d,%dK byte", size/1000, size&1000);
		}
	}
	
	//
	// Output
	//	msgid < status < size
	//
	stm <<	ent.m_idx << '+';
	if (pMsgID != NULL)
		stm << HTMLText(pMsgID);
	stm << '<' << status
	    << '<' << sizebuf
	    << '<' << (ent.m_hasATT ?  "yes" : "no");

	//
	// Date
	//
	stm <<  '<';

	const char * p = ent.m_date.c_str();
	if(p)
		OutDate(stm, p);

	//
	// From
	//
	stm <<  '<';

	ZString szWho;
	p = ent.m_who.c_str();
	if(p)
	{
		szWho = p;
		INMailName(szWho.chars()).RealName(szWho);
		stm << HTMLText(szWho);
	}

	//
	// subject
	//
	stm <<  '<';

	const char * pSubj = ent.m_subject.c_str();
	if(pSubj == NULL)
		pSubj = "";
	int l = strlen(pSubj);
	if(l == 0)
		stm << "[No Subject]";
	else
		stm << HTMLText(pSubj);

	stm << "<\n";
}
