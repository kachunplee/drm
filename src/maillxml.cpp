#include <sys/time.h>
#include <stdio.h>

#include "def.h"
#include "mailopt.h"
#include "mailbox.h"
#include "maillxml.h"

void
MailListXML::NoMail (ostream &stm)
{
#if 0
	stm << "No mail in your " << m_boxName << " box<p>" << endl;
#endif
	m_bError = TRUE;
}

void
MailListXML::MBoxBusy (ostream &stm)
{
#if 0
	stm << "Another session is accessing your " << m_boxName
		<< " box, please retry later.<p>" << endl;
#endif
	m_bError = TRUE;
}

void
MailListXML::OutBegin (ostream& stm)
{

#if 0
	const char * pBoxName = m_name;

	const char * pListFontSize = m_pOpt->GetListFontSize();
	if (strncasecmp(pListFontSize, "x-small", 7) == 0)
	{
	}
	else if (strncasecmp(pListFontSize, "small", 5) == 0)
	{
	}

	stm
		<< "<form method=POST name=maillist action=\""
		//<< "/cgi-bin" << "/test-env\">" << endl
		<< MAILBIN << "mailxfer\">" << endl
		<< "<input type=hidden name=actiontype value=\"\">" << endl
	;

	if(pBoxName)
	{
		stm
			<< "<input type=hidden name=mboxname value="
			<< pBoxName << ">" << endl;
	}
#endif
}

void
MailListXML::OutEnd (ostream& stm)
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
MailListXML::PrintLine (ostream& stm, MailEntry &ent)
{
	//
	// MsgID
	//
	const char * pMsgID = ent.m_msgid.c_str();
	if(pMsgID) pMsgID = URLText(pMsgID).GetText();

	//
	// Icon image
	//
	const char * pMailImg = "mailclosed.gif";
	if(ent.m_status & MSG_SEND)
		pMailImg = "mailsend.gif";
	else if(ent.m_status & MSG_REPLY)
		pMailImg = "mailreply.gif";
	else if(ent.m_status & MSG_FORWARD)
		pMailImg = "mailforward.gif";
	else if(ent.m_status & MSG_READ)
		pMailImg = "mailopened.gif";

	//
	// size
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
	//	<rows><items msgid=" " icon=" " size=" ">
	//
	stm <<	"  <row>\n"
		"    <items msg=\"" << ent.m_idx << '+';
			if (pMsgID != NULL)
				stm << HTMLText(pMsgID);
	stm <<	        "\" icon=\"" << pMailImg
	    <<		"\" size=\"" << sizebuf
	    <<		"\" att=\"" << (ent.m_hasATT ?  "yes" : "no")
	    <<		"\">\n";

	//
	// Date
	//
	stm <<  "      <item>";
	const char * p = ent.m_date.c_str();
	if(p)
		OutDate(stm, p);
	stm <<  "</item>\n";

	//
	// From
	//
	stm <<  "      <item><![CDATA[";

	ZString szWho;
	p = ent.m_who.c_str();
	if(p)
	{
		szWho = p;
		INMailName(szWho.chars()).RealName(szWho);
		szWho.gsub("&", "+");
		szWho.gsub("<", "(");
		szWho.gsub(">", ")");
		stm << szWho;
	}
	stm <<        "]]></item>\n";

	//
	// subject
	//
	stm <<  "      <item><![CDATA[";

	const char * pSubj = ent.m_subject.c_str();
	if(pSubj == NULL)
		pSubj = "";
	int l = strlen(pSubj);
	if(l == 0)
		stm << "[No Subject]";
	else
		stm << HTMLText(pSubj);

	stm <<        "]]></item>\n"
		"    </items>\n"
		"  </row>\n";
}
