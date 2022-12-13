#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <dirent.h>

#include "def.h"

#include "mimetext.h"
#include "mailsettings.h"
#include "mailopt.h"
#include "mailbox.h"
#include "mailmsg.h"

extern "C" int wildmat(const char *, const char *);

static char * WeekDays [] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

static char * Months [] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

ostream&
operator << (ostream& stm, MailMessage& msg)
{
	return msg(stm);
}

ostream& MailMessage::operator () (ostream& stm)
{
	string BoxName = m_foldername;
	BoxName[0] = toupper(BoxName[0]);

	char pCheckCmd[16];
	strcpy(pCheckCmd, "/in?");

	if (strcasecmp(m_foldername, "out") == 0)
		m_hdrWho = "To";
	else
		m_hdrWho = "From";

	switch(m_pMailbox->GetMailStatus())
	{
	case MS_NOMAIL:
		OutBegin(stm, BoxName.c_str(), pCheckCmd, NULL, FALSE);
		stm << "No mail in your " << BoxName << " box" << endl;
		OutEnd(stm, FALSE);
		return stm;

	case MS_BUSY:
		OutBegin(stm, BoxName.c_str(), pCheckCmd, NULL, FALSE);
		stm << "Another session is accessing your " << BoxName
			<< " box, please retry later." << endl;
		OutEnd(stm, FALSE);
		return stm;

	default:
		if(m_pMailbox->GetCurMailInfo(m_opt.GetMailIdx(),
			m_opt.GetMailID()))
		{
			struct MsgInfo * pInfo = m_pMailbox->GetInfo();
			m_szReply = pInfo->msgMsg->GetHeader("reply-to");
			if (m_szReply.length() == 0)
				m_szReply = pInfo->msgMsg->GetHeader("from");
			m_szReplyAll = pInfo->msgMsg->GetHeader("to");
			if (strcasecmp(m_foldername, "out") == 0)
			{
				m_szWho = pInfo->msgMsg->GetHeader("to");
			}
			else
			{
				m_szWho = pInfo->msgMsg->GetHeader("from");
			}
			m_szCC = pInfo->msgMsg->GetHeader("cc");
			m_szNewsgrp = pInfo->msgMsg->GetHeader("newsgroups");
			m_szDate = pInfo->msgMsg->GetHeader("date");

			if(m_szNewsgrp.length() > 0)
			{
				MailSettings set(m_pUser);
				const char * s = set.GetModerate();
				if ((strcasecmp(m_foldername, "in") == 0) && s && *s)
					m_bModerate = wildmat(m_szNewsgrp, s);
			}

			const char * pSubj =
				pInfo->msgMsg->GetHeader("subject");
			if(pSubj == NULL) pSubj = "";
			if(m_bDspHdrs)
			{
				OutBegin(stm, BoxName.c_str(), pCheckCmd,
					pSubj, TRUE);
				caddr_t pMap =
					m_pMailbox->GetMsg(pInfo->msgOffset);
				PrintHdr(stm, pInfo, pMap);
				OutEnd(stm, TRUE);
			}
			else if(m_opt.GetAttNum())
			{
				if(*(m_opt.GetAttNum()) == 0)
				{
				    if (m_opt.IsPrnMsg())
				    {
					OutBegin(stm, BoxName.c_str(),
						pCheckCmd, pSubj, TRUE);
					caddr_t pMap =
						m_pMailbox->GetMsg(pInfo->msgOffset);
					stm << "Content-type: text/plain"
				    		<< endl << endl;
					stm.write(pMap, pInfo->msgLength);
				    }
				    else
				    {
					if (m_opt.IsHeader())
					{
						OutBegin(stm, BoxName.c_str(),
							pCheckCmd, pSubj, TRUE);
						OutEnd(stm, TRUE);
					}
					else
					{
						caddr_t pMap =
					   		m_pMailbox->GetMsg(pInfo->msgOffset);
						stm << "Content-type: text/plain"
					    		<< endl << endl;
						stm.write(pMap, pInfo->msgLength);
					}
				    }
				}
				else
					PrintAttach(stm, *this);
			}
			else
			{
				OutBegin(stm, BoxName.c_str(), pCheckCmd,
					pSubj, TRUE);
				PrintMsg(stm, pInfo, pSubj);
				OutEnd(stm, TRUE);
			}
		}
		else
		{
			OutBegin(stm, BoxName.c_str(), pCheckCmd,
				NULL, FALSE);
			stm << "Cannot locate the email message. It might be deleted or transferred to another mailbox.<p>" << endl;
			stm << "idx=" << m_opt.GetMailIdx() << ", ID="
				<< HTMLText(m_opt.GetMailID()) << "<p>" << endl;
			OutEnd(stm, FALSE);
			return stm;
		}
	}

	return stm;
}

BOOL MailMessage::IsStatus (char cStatus)
{
	return (m_Status.contains(cStatus) == 1);
}

BOOL MailMessage::IsDspImage ()
{
	if(m_DspImage == NTRUE)
	{
		m_DspImage = TRUE;

		// need to read the DspImage from the option
		ZString stgTmp = DIR_LOCAL;
		stgTmp += m_pUser;
		
		struct stat sb;
		if(stat(stgTmp, &sb) == -1)
		{
			// no local dir, set DspImage to true
			return m_DspImage;
		}

		stgTmp += "/.fromrc";
		if(stat(stgTmp, &sb) == -1)
		{
			// no setting file, set DspImage to true
			return m_DspImage;
		}
		Zifstream rcFile(stgTmp, ios::nocreate|ios::in);
		if(!rcFile || !rcFile.good())
		{
			// setting file is bad, set DspImage to true
			return m_DspImage;
		}

		char * p;
		do
		{
			p = rcFile.GetLine();
			if(!p) break;
			if(strncmp(p, "dspimage", 8) == 0)
			{
				p = p+9;
				if(strncmp(p, "off", 3) == 0)
					m_DspImage = FALSE;
				break;
			}
		} while(rcFile.NextLine());
	}
	return m_DspImage;
}

void MailMessage::OutBegin (ostream& stm, const char * pBoxName,
	const char * pCheckCmd, const char * pSubject, BOOL bValidMsg)
{
	HTMLBeg(stm) << "<head>" << endl << "<title>";

	if(pSubject)
		stm << HTMLText(gMimeText.GetText(pSubject));

	const char * pMsgID = HTMLText(m_opt.GetHexMailID()).GetText();

	stm << " - Direct Read Email - WebMail Interface</title>" << endl;

	BOOL bOutCharset = FALSE;
	if (m_opt.IsHeader())
	{
		const char * pCharset = gMimeText.GetCharset();
		if (pCharset && *pCharset)
		{
			stm << "<meta http-equiv=\"Content-Type\" "
				<< "content=\"text/html; charset="
				<< pCharset << "\">" << endl;
			bOutCharset = TRUE;
		}
	}

	stm
		<< "<script src=" << DIR_JS << "drmwin.js></script>" << endl
		<< "</head>" << endl
	;

	ZString MailAddr;
	INMailName(m_szWho).EMailAddress(MailAddr);

	ZString MailCCAddr;
	const char * pCC = m_szCC.chars();
	while(pCC && *pCC)
	{
		char * pp = (char *)strchr(pCC, ',');
		if (pp) *pp = 0;
		ZString addr;
		INMailName(pCC).EMailAddress(addr);
		if (MailCCAddr.length() > 0)
			MailCCAddr += "," + addr;
		else
			MailCCAddr = addr;
		if (pp)
		{
			*pp++ = ',';
			while(*pp == ' ') pp++;
		}
		pCC  = pp;
		
	}

	ZString MailReplyAllAddr;
	const char * pReplyAll = m_szReplyAll.chars();
	while(pReplyAll && *pReplyAll)
	{
		char * pp = (char *)strchr(pReplyAll, ',');
		if (pp) *pp = 0;
		ZString addr;
		INMailName(pReplyAll).EMailAddress(addr);
		if (MailReplyAllAddr.length() > 0)
			MailReplyAllAddr += "," + addr;
		else
			MailReplyAllAddr = addr;
		if (pp)
		{
			*pp++ = ',';
			while(*pp == ' ') pp++;
		}
		pReplyAll  = pp;
		
	}

	int nCol = 0;
	const char * pCol = "";
	const char * pBodyColor = "";

	if (m_opt.IsPreview())
	{
		if (m_opt.IsXML())
		{
			ZString prevWho = gMimeText.GetText(m_szWho);
			stm <<	"<script language=javascript>\n"
				"  parent.setPrevwTitle('"
			    <<	HTMLText(gMimeText.GetText(pSubject), true) << "', '"
			    <<	HTMLText(prevWho, true) << "', '"
			    <<  ((MailAddr.contains('@')
				    && MailAddr.contains('.'))
					? HTMLText(URLText(MailAddr).GetText())
					: "" ) << "', '"
		            <<	HTMLText(m_foldername) << "', '"
			    <<	m_opt.GetMailIdx() << "+"
			    <<	pMsgID << "')\n"
				"</script>"
			    << endl;
			
			return;
		}

		nCol = 3;
		pCol = " colspan=2";
		pBodyColor = " bgcolor=#EAEFF8";
		stm <<
			"<script language=javascript>\n"
			"<!--\n"
			"var d = parent.DRMLstPrevHeader.document;\n"
			"d.open(\"text/html\");\n"
			"d.writeln('<head>');\n"
			"d.writeln('<script src=" << DIR_JS << "drmwin.js></script>');\n"
			"d.writeln('</head>');\n"
			"d.writeln('<body" << pBodyColor << ">');\n"
			"d.writeln('<table border=0 cellpadding=0 "
			"cellspacing=0 width=100%>');\n"
			"d.writeln('<tr height=7>');\n"
			"d.writeln('<td bgcolor=#21599C colspan=" << nCol << ">"
			"<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width=1 "
			"hegiht=1></td>');\n"
			"d.writeln('</tr>');\n"
			"d.writeln('<tr>');\n"
			"d.writeln('<td height=17 valign=top bgcolor=#EAEFF8 "
			"width=69><font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;Subject:</b></font></td>');\n"
			"d.writeln('<td height=17 valign=top bgcolor=#EAEFF8"
			<< pCol
			<< "><font face=Arial size=2 color=#396BB5>"
			<< HTMLText(gMimeText.GetText(pSubject), true)
			<< "</font></td>');\n"
			"d.writeln('</tr>');\n"
			"d.writeln('<tr>');\n"
			"d.writeln('<td height=17 valign=top bgcolor=#EAEFF8 "
			"width=69><font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;" << m_hdrWho << ":</b></font></td>');\n"
			"d.writeln('<td height=17 valign=top bgcolor=#EAEFF8>"
		;

		if( MailAddr.contains('@') && MailAddr.contains('.') )
			stm << "<a href=\"mailto:"
				<< HTMLText(URLText(MailAddr).GetText())
				<< "\">"
				<< "<font face=Arial size=2 color=#396BB5>"
				<< HTMLText(gMimeText.GetText(m_szWho), true)
				<< "</font></a></td>');\n"
			;
		else
			stm << "<font face=Arial size=2 color=#396BB5>"
				<< HTMLText(gMimeText.GetText(m_szWho), true)
				<< "</font></td>');\n"
			;

		stm <<
			"d.writeln('<td height=17 valign=top align=right "
			"bgcolor=#EAEFF8 width=213>"
			"<a href=\"javascript:OpenMsgWin(\\'"
			<< MAILBIN << "frmmessage?" << m_foldername
			<< "+" << m_opt.GetMailIdx() << "+"
			<< pMsgID
			<< "\\')\" title=\"Open Message\">"
			"<img src=" << DIR_IMG << "openmsg.gif border=0 width=87 "
			"height=16></a>&nbsp;"
			"<a href=\"javascript:OpenPrnMsgWin(\\'"
			<< MAILBIN << "frmprnmsg?" << m_foldername
			<< "+-" << M_PRNMSG << "+" << m_opt.GetMailIdx()
			<< "+" << pMsgID
			<< "\\')\" title=\"Open Printable Message\">"
			"<img src=" << DIR_IMG << "prnver.gif border=0 width=112 "
			"height=16></a>&nbsp;</td>');\n"
			"d.writeln('</tr>');\n"
			"d.writeln('</table>');\n"
			"d.writeln('</body>');\n"
			"d.close();\n"
			"//-->\n"
			"</script>\n"
		;
		return;
	}

	if (!m_opt.IsHeader() && !m_opt.IsPrnMsg())
	{
		stm
			<< DEF_BODY_TAG << endl
			<< endl
		;
		return;
	}

	nCol = 2;

	if (m_opt.IsPrnMsg())
	{
	    stm <<
		"<body" << pBodyColor << ">\n"
		"<table border=0 cellpadding=0 cellspacing=0 width=100%>\n"
		"<tr>\n"
		"<td colspan=2 height=5 bgcolor=#EAEFF8>"
		"<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width=1 height=5>"
		"</td>\n"
		"</tr>\n"
	    ;
	}
	else
	{
	    stm <<
		"<link rel=\"stylesheet\" type=\"text/css\" href=\""
		<< DIR_CSS << "drmmenu.css\" />\n"
		"<script language=javascript>\n"
		"<!--\n"
		"function ViewMessage (foldername, idx, msgid)\n"
		"{\n"
		"	parent.DRMMsgHeader.location = '" << MAILBIN
		<< "message?' + foldername + '+-H+' + idx + '+' + msgid;\n"
		"	parent.DRMMsgBody.location = '" << MAILBIN
		<< "message?' + foldername + '+-B+' + idx + '+' + msgid;\n"
		"}\n"
		"function ViewHeader (foldername, idx, msgid)\n"
		"{\n"
		"	parent.DRMMsgHeader.location = '" << MAILBIN
		<< "message?' + foldername + '+-H+-" << M_HEADER
		<< "+' + idx + '+' + msgid;\n"
		"	parent.DRMMsgBody.location = '" << MAILBIN
		<< "message?' + foldername + '+-B+-" << M_HEADER
		<< "+' + idx + '+' + msgid;\n"
		"}\n"
		"function ViewSource (foldername, idx, msgid)\n"
		"{\n"
		"	parent.DRMMsgHeader.location = '" << MAILBIN
		<< "message?' + foldername + '+-H+' + idx + '.+' + msgid;\n"
		"	parent.DRMMsgBody.location = '" << MAILBIN
		<< "message?' + foldername + '+-B+' + idx + '.+' + msgid;\n"
		"}\n"
		"\n"
		"//-->\n"
		"</script>\n"
		"<body" << pBodyColor << " style=\"font-family: Arial; "
		"font-size: 90%\">\n"
		"<div id=\"stitleBar\">"
		"<img src=\"" << DIR_IMG << "toolbartop.gif\" height=\"28\" "
		"width=\"100%\"></div>\n"
		"<div id=\"mainMenu\" class=\"mainMenu\" "
		"style=\"min-width: 720px; background-image:url('"
		"" << DIR_IMG << "menubg.gif');\">\n"
		"<div style=\"margin:0; padding: 4px 15px 0 80px;\">\n"
		"<ul id=\"menuList1\" class=\"menuList\">\n"
		"<li>\n"
		"<a class=\"selector\" href=\"javascript:OpenComposeWin('"
		<< MAILBIN << "compose')\"><b>New Message</b></a>\n"
		"</li>\n"
	;

	if (bValidMsg)
	{
	    ZString ReplyAddr;
	    INMailName(m_szReply).EMailAddress(ReplyAddr);
	    stm <<
		"| <li>\n"
		"<a class=\"selector\" href=\"javascript:OpenComposeWin('"
		<< MAILBIN << "compose?" << m_foldername
		<< "+-r" << HTMLText(URLText(ReplyAddr, true).GetText(), true)
		<< "+" << m_opt.GetMailIdx() << "+"
		<< HTMLText(m_opt.GetHexMailID())
		<< "')\"><b>Reply</b></a>\n"
		"</li>\n"
		"| <li>\n"
		"<a class=\"selector\" href=\"javascript:OpenComposeWin('"
		<< MAILBIN << "compose?" << m_foldername << "+-R"
		<< HTMLText(URLText(ReplyAddr, true).GetText(), true)
	    ;

	    if (MailReplyAllAddr.length() > 0)
		stm << HTMLText(",")
		    << HTMLText(URLText(MailReplyAllAddr, true).GetText(),
					true);

	    if (MailCCAddr.length() > 0)
		stm << HTMLText(",")
		    << HTMLText(URLText(MailCCAddr, true).GetText(), true);

	    stm <<
		"+" << m_opt.GetMailIdx() << "+"
		<< HTMLText(m_opt.GetHexMailID())
		<< "')\"><b>Reply All</b></a>\n"
		"</li>\n"
		"| <li>\n"
		"<a class=\"selector\" href=\"javascript:OpenComposeWin('"
		<< MAILBIN << "compose?" << m_foldername << "+-f"
		<< HTMLText(URLText(MailAddr, true).GetText(), true)
		<< "+" << m_opt.GetMailIdx() << "+" << pMsgID
		<< "')\"><b>Forward</b></a>\n"
		"</li>\n"
	    ;

	    if (m_bDspHdrs)
	    {
		stm <<
		    "| <li>\n"
		    "<a class=\"selector\" href=\"javascript:ViewMessage('" 
		    << m_foldername << "', '" << m_opt.GetMailIdx() << "', '"
		    << pMsgID << "')\"><b>View Message</b></a>\n"
		    "</li>\n"
		    "| <li>\n"
		    "<a class=\"selector\" href=\"javascript:ViewSource('"
		    << m_foldername << "', '" << m_opt.GetMailIdx() << "', '"
		    << pMsgID << "')\"><b>View Source</b></a>\n"
		    "</li>\n"
		    "| <li>\n"
		    "<a class=\"selector\" href=\"/cgi-bin/ulogout\">"
		    "<b>Logout</b></a>\n"
		;
	    }
	    else if(m_opt.GetAttNum() && (*(m_opt.GetAttNum()) == 0))
	    {

		stm <<
		    "| <li>\n"
		    "<a class=\"selector\" href=\"javascript:ViewMessage('" 
		    << m_foldername << "', '" << m_opt.GetMailIdx() << "', '"
		    << pMsgID << "')\"><b>View Message</b></a>\n"
		    "</li>\n"
		    "| <li>\n"
		    "<a class=\"selector\" href=\"javascript:ViewHeader('"
		    << m_foldername << "', '" << m_opt.GetMailIdx() << "', '"
		    << pMsgID << "')\"><b>View Header</b></a>\n"
		    "</li>\n"
		    "| <li>\n"
		    "<a class=\"selector\" href=\"/cgi-bin/ulogout\">"
		    "<b>Logout</b></a>\n"
	        ;
	    }
	    else
	    {
		stm <<
		    "| <li>\n"
		    "<a class=\"selector\" href=\"javascript:ViewHeader('"
		    << m_foldername << "', '" << m_opt.GetMailIdx() << "', '"
		    << pMsgID << "')\"><b>View Header</b></a>\n"
		    "</li>\n"
		    "| <li>\n"
		    "<a class=\"selector\" href=\"javascript:ViewSource('" 
		    << m_foldername << "', '" << m_opt.GetMailIdx() << "', '"
		    << pMsgID << "')\"><b>View Source</b></a>\n"
		    "</li>\n"
		;

		if (m_bModerate)
		{
		    stm <<
			"| <li>\n"
			"<a class=\"selector\" href=\"" << MAILBIN << "message?"
			<< m_foldername << "+-" << M_APPROVE << "+"
			<< m_opt.GetMailIdx() << "+" << pMsgID
			<< "\" target=DRMMsgBody><b>Approve</b></a>\n"
		    ;
		}
		else
		{
		    stm	<<
			"| <li>\n"
			"<a class=\"selector\" href=\"/cgi-bin/ulogout\">"
			"<b>Logout</b></a>\n"
		    ;
		}
	    }
	}
	else
	{
	    stm	<<
		"| <li>\n"
		"<a class=\"selector\" href=\"/cgi-bin/ulogout\">"
		"<b>Logout</b></a>\n"
	    ;
	}

	stm	<<
		"</ul>\n"
		"</div>\n"
		"</div>\n"
		"<div style=\"position:absolute;top:0;left:10px;\">"
		"<img id=\"stitleImage\" src=\"" << DIR_IMG <<
		"stitlelogo.gif\" border=0></div>\n"
		"<div style=\"position:absolute;top:3;left:535;\">"
		"<a href=\"javascript:OpenPrnMsgWin('"
		<< MAILBIN << "frmprnmsg?" << m_foldername
		<< "+-" << M_PRNMSG << "+" << m_opt.GetMailIdx()
		<< "+" << pMsgID
		<< "')\" title=\"Open Printable Message\"><img border=0 src="
		<< DIR_IMG << "prnver.gif width=112 height=16></a></div>\n"

		"<table border=0 cellpadding=0 cellspacing=0 width=100%>\n"
	    ;
	}

	if (!bValidMsg && !m_opt.IsPreview())
	{
		stm <<
			"<tr>\n"
			"<td height=17 bgcolor=#EAEFF8 colspan=2>"
			"<font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;Subject:</b></font></td>\n"
			"</tr>\n"
			"<tr>\n"
			"<td height=17 bgcolor=#EAEFF8 colspan=2>"
			"<font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;" << m_hdrWho << "</b></font></td>\n"
			"</tr>\n"
			"<tr>\n"
			"<td height=17 bgcolor=#EAEFF8 colspan=2>"
			"<font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;Date:</b></font></td>\n"
			"</tr>\n"
			"</table>\n"
		;
		return;
	}

	if (m_szNewsgrp.length() > 0 && !m_opt.IsPreview())
	{
		stm <<
			"<tr>\n"
			"<td height=17 bgcolor=#EAEFF8 width=75>"
			"<font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;Newsgroups:</b></font></td>\n"
			"<td height=17 bgcolor=#EAEFF8"
			<< pCol << ">"
			"<font face=Arial size=2 color=#396BB5>"
			<< HTMLText(m_szNewsgrp, true)
			<< "<font></td>\n"
			"</tr>\n"
		;
	}

	stm <<
		"<tr>\n"
		"<td height=17 valign=top bgcolor=#EAEFF8 width=75>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;Subject:</b></font></td>\n"
		"<td height=17 valign=top bgcolor=#EAEFF8" << pCol
		<< "><font face=Arial size=2 color=#396BB5>"
		<< HTMLText(gMimeText.GetText(pSubject))
		<< "</font></td>\n"
		"</tr>\n"
		"<tr>\n"
		"<td height=17 valign=top bgcolor=#EAEFF8 width=75>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;" << m_hdrWho << ":</b></font></td>\n"
		"<td height=17 valign=top bgcolor=#EAEFF8>"
	;

	if( MailAddr.contains('@') && MailAddr.contains('.') )
	{
		stm << "<a href=\"mailto:"
			<< HTMLText(URLText(MailAddr).GetText()) << "\">"
			<< "<font face=Arial size=2 color=#396BB5>"
			<< HTMLText(gMimeText.GetText(m_szWho), true) << "</font></a>"
		;
		if (!m_opt.IsPreview() && !m_opt.IsPrnMsg())
		{
		    ZString addbkurl = MAILBIN;
		    stm << "<input type=hidden value=\""
			<< HTMLText(MailAddr).GetText() << "\" id=\"fromEmail\">";
		    addbkurl += "drmaddbk/1";
		    stm << "&nbsp;<a href=\"javascript:OpenAddBookWin('"
			<< addbkurl << "')\"><img border=0 align=top "
			<< "src=" << DIR_IMG
			<< "blueaddaddress.gif width=128 height=16></a>";
		}
		stm << "</td>\n"
		;
	}
	else
		stm << "<font face=Arial size=2 color=#396BB5>"
			<< HTMLText(gMimeText.GetText(m_szWho), true) << "</font></td>\n"
		;

	if (m_opt.IsHeader() && !bOutCharset)
	{
		const char * pCharset = gMimeText.GetCharset();
		if (pCharset && *pCharset)
		{
			stm << "<meta http-equiv=\"Content-Type\" "
				<< "content=\"text/html; charset="
				<< pCharset << "\">" << endl;
			bOutCharset = TRUE;
		}
	}

	if (m_opt.IsPreview())
	{
		stm <<
			"<td height=17 valign=top align=right "
			"bgcolor=#EAEFF8 width=213>"
			"<a href=\"javascript:OpenMsgWin(\\'"
			<< MAILBIN << "frmmessage?" << m_foldername
			<< "+"
		;

		stm << m_opt.GetMailIdx() << "+"
			<< pMsgID
			<< "\\')\" title=\"Open Message\">"
			"<img src=" << DIR_IMG << "openmsg.gif border=0 width=87 "
			"height=16></a>&nbsp;"
			"<a href=\"javascript:OpenPrnMsgWin(\\'"
			<< MAILBIN << "frmprnmsg?" << m_foldername
			<< "+-" << M_PRNMSG << "+" << m_opt.GetMailIdx()
			<< "+" << pMsgID
			<< "\\')\" title=\"Open Printable Message\">"
			"<img src=" << DIR_IMG << "prnver.gif border=0 width=112 "
			"height=16></a>&nbsp;</td>\n"
			"</tr>\n"
		;
	}
	else
	{
		stm << "</tr>\n";

		if (m_szNewsgrp.length() == 0)
		{
		    INDateTime mdate(m_szDate);
		    mdate.SetTZ(m_opt.GetTZName());
		    tm t = mdate.localtime();
		    int tz_min = t.tm_gmtoff/60;
		    char s[2] = {0, 0};
		    if (tz_min < 0)
		    {
			s[0] = '-';
			tz_min = -tz_min;
		    }
		    char dateBuf[256];
		    sprintf(dateBuf,
			"%s, %2d %s %4d %02d:%02d:%02d %s%02d%02d (%s)\n",
			WeekDays[t.tm_wday], t.tm_mday,
			Months[t.tm_mon], t.tm_year+1900,
			t.tm_hour, t.tm_min, t.tm_sec,
			s, tz_min/60, tz_min%60, t.tm_zone); 

		    stm <<
			"<tr>\n"
			"<td height=17 valign=top bgcolor=#EAEFF8 "
			"width=75><font face=Arial size=2 color=#396BB5>"
			"<b>&nbsp;Date:</b></font></td>\n"
			"<td height=17 valign=top bgcolor=#EAEFF8>"
			"<font face=Arial size=2 color=#396BB5>"
			<< HTMLText(dateBuf, true)
			<< "</font></td>\n"
			"</tr>\n";
		}
		stm <<
			"<tr height=2>\n"
			"<td bgcolor=#396BB5 width=75>"
			"<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width=75 "
			"height=1></td>\n"
			"<td bgcolor=#396BB5>"
			"<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width=1 "
			"height=1></td>\n"
			"</tr>\n"
		;
	}

	stm <<
		"</table>\n"
	;
}

void MailMessage::OutEnd (ostream& stm, BOOL bValidMsg)
{
	stm << "</body>" << endl
		<< "</html>" << endl;
}

void MailMessage::PrintHdr (ostream& stm, struct MsgInfo * pInfo, caddr_t pMap)
{
	if (m_opt.IsHeader())
		return;

	//
	// Print all the headers
	//
	stm
		<< "<table border=\"0\" cellpadding=\"0\" "
		<< "cellspacing=\"0\" width=\"700\">" << endl
		<< "</tr>" << endl
		<< "<tr>" << endl
		<< "<td><font face=\"Arial\" size=\"2\">" << endl
	;

	ZString MailAddr;
	caddr_t pEnd = pMap + pInfo->msgLength;
	caddr_t p, q, r;
	for(; *pMap && pMap < pEnd; pMap = p+1)
	{
		p = strchr(pMap, '\n');
		if(pMap == p)
		{
			pMap++;
			break;
		}
		if(p) *p = 0;

		q = strchr(pMap, ' ');
		if(q && *q)
		{
			if(*(q-1) != ':')
			{
				stm << HTMLText(pMap) << "<br>" << endl;
			}
			else
			{
				r = q;
				*r++ = 0;
				while(r < p && *r && isspace(*r)) r++;

				stm << "<b>" << HTMLText(pMap) << " </b>"
					<< HTMLText(r) << "<br>" << endl;

				*q = ' ';
			}
		}
		else
			stm << HTMLText(pMap) << "<br>" << endl;

		if(p) *p = '\n';
	}

	stm
		<< "</font></td>" << endl
		<< "</tr>" << endl
		<< "</table>" << endl
	;
}

void MailMessage::PrintMsg (ostream& stm, struct MsgInfo * pInfo,
	const char * pSubj)
{
	if (m_opt.IsHeader() && !m_opt.IsPrnMsg())
		return;

	if (m_opt.IsPrnMsg())
	{
		stm << "<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width=1 "
			"height=10><br>\n";
	}

	//
	// Print headers
	//
	stm << "<font face=\"Arial\" size=\"2\">" << endl;

	// Save the current status header
	if(m_Status.length() == 0)
		m_Status = pInfo->msgMsg->GetHeader("status");

	PrintBody(stm, pInfo, *pInfo->msgMsg, "");

	stm << endl
		<< "</font>" << endl;

	if (m_opt.IsPreview())
	{
		stm
			<< "<form method=POST name=maillist action=\""
			<< MAILBIN << "mailxfer?-p\">" << endl
			<< "<input type=hidden name=mboxname value="
			<< m_foldername
		;
		stm
			<< ">" << endl
			<< "<input type=hidden name=\"" << m_opt.GetMailIdx()
			<< ","
		;

		const char * pMsgID = HTMLText(m_opt.GetHexMailID()).GetText();
		if(pMsgID != NULL)
			stm << pMsgID;

		stm
			<< "\" value=yes>" << endl
			<< "<input type=hidden name=actiontype value=\"\">"
			<< endl
			<< "</form>" << endl
		;
	}
}

static char *
memmem (const char *big, int blen, const char *lit, int llen)
{
	const char *pos;

	for (pos = big; pos - big <= blen - llen; pos++) {
		if (*pos == *lit && memcmp(pos, lit, llen) == 0)
			return (char *)pos;
	}

	return NULL;
}

void
MailMessage::PrintBody (ostream& stm, struct MsgInfo * pInfo, ZMIME & msg, const char * pAttNumR)
{
	string attnum;

	//
	// Prelog !!
	//

	//
	// Print body
	//
	if(msg.IsPlainAndHTML())
	{
		PrintBody(stm, pInfo, msg.GetAtt(1));
		return;
	}

	msg.DecodeBody();
	const char * pBody = msg.GetBody();

	if(pBody)
	{
		const char * pEnd = pBody + msg.GetBodyLen();

		//
		// print the body
		//
		if(msg.IsPlain())
		{
			PrintPlain(stm, pBody, pEnd);
		}
		else if(msg.IsHTML())
		{
			const char *attnum, *begid, *endid;
			string id;
			for(endid = pBody; pBody < pEnd; )
			{
				begid = memmem(endid, pEnd-endid, "cid:", 4);
				if(begid == NULL)
					break;

				begid += 4;
				for(endid = begid; endid < pEnd; endid++)
					if(isspace(*endid)
						|| *endid == '"'
						|| *endid == '>')
						break;

				id = string(begid, endid-begid);
				attnum =
					pInfo->msgMsg->MapContentID(id.c_str());
				if(attnum == NULL)
					continue;

				pInfo->msgMsg->GetAtt(attnum)->SetCID();
				stm.write(pBody, begid-4-pBody);
				stm << MAILBIN
				    << m_opt.GetProgName()
				    << "/" << m_opt.GetMailIdx()
					<< '.' << attnum << "/"
				    << HTMLText(m_opt.GetHexMailID())
				;

				pBody = endid;
			}

			if(pEnd-pBody) stm.write(pBody, pEnd-pBody);
			stm << "<p>";
		}
#if 0
		else if(msg.IsRFC822())
		{
		}
#endif
		else if(msg.IsUUEncoded())
		{
			if(msg.GetProlog())
				PrintPlain(stm,
					msg.GetProlog(),
					msg.GetProlog()+msg.GetPrologLen());
			attnum = pAttNumR;
			attnum += ".0";
			PrintAttLink(stm, msg.GetUUFileName(),
				msg, attnum.c_str());

			if(msg.GetEpilog())
				PrintPlain(stm,
					msg.GetEpilog(),
					msg.GetEpilog()+msg.GetEpilogLen());
		}
		else
			PrintAttLink(stm, msg.GetAttFileName(), msg, pAttNumR);
	}

	//
	// Prolog !!
	//

	//
	// Print Attachments
	//
	int n = msg.GetNumAtt();
	if(n)
	{
		for(int i = 0; i < n; i++)
		{
			char buf[20];
			sprintf(buf, ".%d", i+1);
			attnum = pAttNumR;
			attnum += buf;
			PrintBody(stm, pInfo, msg.GetAtt(i), attnum.c_str());
		}
	}
}

void MailMessage::PrintLine (ostream& stm, const char * p, const char * end)
{
	int n;
	for(const char * q = p; p < end && (*p != '\n'); p+=n)
	{
		n = strcspn(p, "&<>:\n");
		switch(p[n])
		{
		case '&':
			stm.write(q, n);
			stm << "&amp;";
			n++;
			q = p + n;
			continue;
		case '<':
			stm.write(q, n);
			stm << "&lt;";
			n++;
			q = p + n;
			continue;
		case '>':
			stm.write(q, n);
			stm << "&gt;";
			n++;
			q = p + n;
			continue;
		case ':':
			//
			// no longer scanning for mailto, news, newspost, nntp, telnet,
			//	gopher, tn3270 and whois
			//
			if(strncmp(p+n-4, "http", 4) == 0)
			{
				if(n > 4)
					stm.write(q, n-4);
				p = p + n - 4;
				n = PrintLink(stm, p, TRUE);
				q = p + n;
			}
			else if(strncmp(p+n-3, "ftp", 3) == 0)
			{
				if(n > 3)
					stm.write(q, n-3);
				p = p + n - 3;
				n = PrintLink(stm, p, FALSE);
				q = p + n;
			}
			else
			{
				stm.write(q, ++n);
				q = p+n;
			}
			continue;

		case '\n':
			stm.write(q, n);
			break;

		default:
			stm << q;
		}

		break;
	}
	stm << "<br>" << endl;
}

int MailMessage::PrintLink (ostream& stm, const char * p, BOOL bHTTP)
{
	char c;
	int n;
	for(n = 0; (c = p[n]); n++)
	{
		if(c == ' ' || c == '\t' || c == '"' || c == '<' || c == '>' ||
			c == '(' || c == ')' || c == '[' || c == ']' || c == '\n')
			break;
		if(!bHTTP && c == '=')
			break;
	}

	if(p[n-1] == '.' || p[n-1] == ',')
		c = p[--n];

	stm << "<a href=\"" << HTMLText(p, n)
		<< "\">" << HTMLText(p, n) << "</a>";

	return n;
}

void
MailMessage::PrintAttLink (ostream & stm, const char * filename, ZMIME & msg,
	const char * pAttNumR)
{
	if(msg.IsCID())
		return;

	if(*pAttNumR == 0)
		pAttNumR = ".0";

	const char * ext = NULL;
	if(filename)
	{
		ext = strrchr(filename, '.');
		if(ext)
		{
			++ext;
			if(strcmp(ext, "jpeg")
				&& strcmp(ext, "jpg")
				&& strcmp(ext, "gif")
				&& strcmp(ext, "png")
				&& strcmp(ext, "bmp")
			  )
				ext = NULL;
		}
	}
	if((ext != NULL || strncmp(msg.GetContentType(), "image/", 6) == 0) &&
		IsDspImage() && strcasecmp(m_foldername, "out") != 0)
	{
		stm << "<img src=\"" << MAILBIN << m_opt.GetProgName();
		stm << "/" << m_opt.GetMailIdx() << pAttNumR << "/"
		    << HTMLText(m_opt.GetHexMailID());
		if(filename)
		    stm << "/" << HTMLText(filename);
		stm << "\"><p>" << endl;
	}
	else
	{
		stm << "<b>Attachment: " << "<a href=\"" << MAILBIN
		    << m_opt.GetProgName();
		stm << "/" << m_opt.GetMailIdx() << pAttNumR
		    << "/"
		    << HTMLText(m_opt.GetHexMailID());
		if(filename)
		    stm << "/" << HTMLText(filename);
		stm << "\" title=\"Display Header\">"
		    << (filename?HTMLText(filename):"No_Name")
		    << "</a></b><p>" << endl;
	}
}

void
MailMessage::PrintAttach (ostream& stm, MailMessage& msg)
{
	struct MsgInfo * pInfo = m_pMailbox->GetInfo();
	const char * p = msg.m_opt.GetAttNum();
	ZMIME * pm = pInfo->msgMsg->GetAtt(p);
	if(pm == NULL)
		return;

	pm->DecodeBody();
	const char * pBody = pm->GetBody();
	int n = pm->GetBodyLen();
	if(pBody == NULL)
		return;

	p = pm->GetContentType();
	if(p == NULL)
		p = "text/plain";
	stm << "Content-type: " << p
	    << endl << endl;
	stm.write(pBody, n);
}

void
MailMessage::PrintPlain (ostream& stm, const char * pBeg, const char * pEnd)
{
	const char *p;
	for( ; *pBeg && pBeg < pEnd; pBeg = p+1)
	{
		p = strchr(pBeg, '\n');
		if(pBeg == p)
			stm << "<br>" << endl;
		else
			PrintLine(stm, pBeg, pEnd);

		if(p == NULL) break;
	}
	stm << "<p>";
}
