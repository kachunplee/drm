#include <sys/time.h>
#include <stdio.h>

#include "def.h"
#include "mailopt.h"
#include "mailbox.h"
#include "maillhtml.h"

void
MailListHTML::OutBegin (ostream& stm)
{
	const char * pBoxName = m_name;
	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp, &tzp);

	int c_sel = 23;
	int c_date = 80;
	int c_menu = 60;
	int c_who = 200;
	const char * pListFontSize = m_pOpt->GetListFontSize();
	if (strncasecmp(pListFontSize, "x-small", 7) == 0)
	{
		c_date = 90;
		c_menu = 60;
		c_who = 215;
	}
	else if (strncasecmp(pListFontSize, "small", 5) == 0)
	{
		c_date = 100;
		c_menu = 60;
		c_who = 220;
	}

	stm  <<
		"<script src=" << DIR_JS << "drmwin.js></script>\n"
		"<script language=javascript>\n"
		"<!--\n"
		"parent.listSelectAll = false;\n"
		"//-->\n"
		"</script>\n"
		"<style type=\"text/css\">\n"
		"<!--\n"
		"    td.clsColSel {font-family: Verdana,Arial,Helvetica; "
		<< "font-size: " << m_pOpt->GetListFontSize()
		<< "}\n"
		"    td.clsColDate {font-family: Verdana,Arial,Helvetica; "
		<< "font-size: " << m_pOpt->GetListFontSize()
		<< "}\n"
		"    td.clsColMenu {font-family: Verdana,Arial,Helvetica; "
		<< "font-size: " << m_pOpt->GetListFontSize()
		<< "}\n"
		"    td.clsColWho {font-family: Verdana,Arial,Helvetica; "
		<< "font-size: " << m_pOpt->GetListFontSize()
		<< "}\n"
		"    td.clsColSubj {font-family: Verdana,Arial,Helvetica; "
		<< "font-size: " << m_pOpt->GetListFontSize()
		<< "}\n\n"
		"    a:link, a:active, a:visited {color: #396BB5; "
		"text-decoration: none}\n"
		"    a:hover {color: #F7D600}\n"
		<< "//-->\n"
		"</style>\n"
	;

	stm
		<< "<table border=0 cellpedding=0 "
		<< "cellspacing=0 width=100%>" << endl
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

	stm
		<< "<tr bgcolor=#ffffff height=1>" << endl
		<< "<td bgcolor=#ffffff height=1 valign=bottom width="
		<< c_sel << " class=clsColSel>"
		<< "<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width="
		<< c_sel << " height=1></td>" << endl
		<< "<td bgcolor=#ffffff height=1 valign=bottom width="
		<< c_date << " class=clsColDate>"
		<< "<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width="
		<< c_date << " height=1></td>" << endl
		<< "<td bgcolor=#ffffff height=1 valign=bottom width="
		<< c_menu << " class=clsColMenu>"
		<< "<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width="
		<< c_menu << " height=1></td>" << endl
		<< "<td bgcolor=#ffffff height=1 valign=bottom width="
		<< c_who << " class=clsColWho>"
		<< "<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width="
		<< c_who << " height=1></td>" << endl
		<< "<td bgcolor=#ffffff height=1 valign=bottom>"
		<< "<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif "
		<< "width=10 height=1></td>" << endl
		<< "</tr>" << endl
	;
}

void
MailListHTML::NoMail (ostream &stm)
{
	stm << "<tr><td colspan=4>"
		"No mail in your " << m_boxName << " box<p></td></tr>" << endl;
	m_bError = TRUE;
}

void
MailListHTML::MBoxBusy (ostream &stm)
{
	stm << "<tr><td colspan=4>"
		"Another session is accessing your " << m_boxName
		<< " box, please retry later.<p><td></tr>" << endl;
	m_bError = TRUE;
}

void
MailListHTML::OutEnd (ostream& stm)
{
	const char * pBoxName = m_name;
	stm << "</form>\n"
		"</table>\n"
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
}

void
MailListHTML::PrintLine (ostream& stm, MailEntry &ent)
{
	int nCellPedding = 2;
	int c_sel = 23;
	int c_date = 80;
	int c_menu = 60;
	int c_who = 200;
	int fontsize = 1;

	const char * pListFontSize = m_pOpt->GetListFontSize();
	if (strncasecmp(pListFontSize, "x-small", 7) == 0)
	{
		c_date = 90;
		c_menu = 60;
		c_who = 215;
		fontsize = 2;
	}
	else if (strncasecmp(pListFontSize, "small", 5) == 0)
	{
		c_date = 100;
		c_menu = 60;
		c_who = 220;
		nCellPedding = 3;
		fontsize = 3;
	}

	if ((m_nOut % 2) == 0)
		stm << "<tr bgcolor=#ffffff>" << endl;
	else
		stm << "<tr bgcolor=#F5F8FC>" << endl;

	ZString szWho;
	const char * p = ent.m_who.c_str();
	if(p)
	{
		szWho = p;
		INMailName(szWho.chars()).RealName(szWho);
		szWho.gsub("&", "+");
		szWho.gsub("<", "(");
		szWho.gsub(">", ")");
	}

	const char * pMailImg = "mailclosed.gif";
	if(ent.m_status & MSG_SEND)
		pMailImg = "mailsend.gif";
	else if(ent.m_status & MSG_REPLY)
		pMailImg = "mailreply.gif";
	else if(ent.m_status & MSG_FORWARD)
		pMailImg = "mailforward.gif";
	else if(ent.m_status & MSG_READ)
		pMailImg = "mailopened.gif";

	const char * pMsgID = ent.m_msgid.c_str();
	if(pMsgID) pMsgID = URLText(pMsgID).GetText();

	stm << "<td valign=top width=" << c_sel << " class=clsColSel>"
		<< "<input type=\"checkbox\" name=\""
		<< ent.m_idx << ","
	;
	if(pMsgID != NULL)
		stm << HTMLText(pMsgID);

	stm
		<< "\" value=\"yes\">"
		<< "</td>" << endl
	;

	p = ent.m_date.c_str();
	stm << "<td valign=top width=" << c_date
		<< " nowrap=true class=clsColDate>";
	if(p)
		OutDate(stm, p);
	else
		stm << "&nbsp;</td>" << endl;

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

	stm << "<td valign=top align=left width=" << c_menu << " nowrap=ture class=clsColMenu>"
		<< "&nbsp;<img border=\"0\" src=\"" << DIR_IMG << pMailImg
		<< "\" width=\"24\" height=\"12\" title='"
		<< HTMLText(sizebuf) << "'>";

	pMailImg = ent.m_hasATT ?  "mailattach.gif" : "mailattachblank.gif";
	stm << "<img src=" << DIR_IMG << pMailImg << " border=0 width=10 "
		<< "height=12></td>" << endl;

	stm.form("<td valign=top width=%d nowrap=true class=clsColWho>&nbsp;%-20.20s</td>\n",
		c_who, szWho.chars());

	stm << "<td valign=top class=clsColSubj>"
		<< "<font color=\"#396BB5\">"
		<< "<a href=\""
	;
	if (!m_pOpt->IsPreviewFrame())
		stm << "javascript:OpenMsgWin('";
	stm << MAILBIN ;
	if (!m_pOpt->IsPreviewFrame())
		stm << "frm";
	stm << "message?" << m_name << "+";
	if (m_pOpt->IsPreviewFrame())
		stm << "-P+";
	stm << ent.m_idx << "+";

	if(pMsgID != NULL)
	{
		if (m_pOpt->IsPreviewFrame())
			stm << HTMLText(pMsgID);
		else
			stm << HTMLText(pMsgID);
	}

	if (!m_pOpt->IsPreviewFrame())
		stm << "')\">";
	else
		stm << "\" target=DRMLstPrevBody>";

	const char * pSubj = ent.m_subject.c_str();
	if(pSubj == NULL)
		pSubj = "";
	int l = strlen(pSubj);
	if(l == 0)
		stm << "[No Subject]";
	else
		stm << HTMLText(pSubj);

	stm << "</a></font></td>" << endl
		<< "</tr>" << endl;

	m_nOut++;
}
