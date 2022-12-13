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
#include "mailopt.h"
#include "mailbox.h"
#include "mailsettings.h"
#include "mailto.h"

static char * WeekDays [] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

static char * Months [] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

MailTo::MailTo (MailOption * pOpt, const char * pUser, MsgInfo * pMsgInfo ,
	caddr_t pMap, char * pFoldername, bool bHeader)
	: m_pOpt(pOpt), m_szUser(pUser), m_pFoldername(pFoldername),
	  m_bHeader(bHeader)
{
	m_pMail = NULL;
	m_pMsgInfo = NULL;
	m_pDate = NULL;
	m_pFrom = NULL;
	m_pReturn = NULL;
	m_pSubject = NULL;
	m_pStatus = NULL;
	m_pTo = NULL;
	m_bHTML = false;

	if(pMsgInfo && pMap)
	{
		m_pMsgInfo = pMsgInfo;

		//
		// The old way...
		//	m_pMail is setup for the scanning for headers again!
		//
		// Actually, the headers should already parsed in
		//	pMsgInfo->msgMsg.
		// However, there is a planned rewrite... do this later.
		//
		int nLen = pMsgInfo->msgLength;
		m_pMail = new char[nLen+1];
		if(m_pMail)
		{
			strncpy(m_pMail, pMap, nLen);
			m_pMail[nLen] = 0;
		}
	}

	MailSettings settings(pUser);
	const char * p = settings.GetAlias();
	if(p && *p)
	{
		char * q = strchr(p, '\t');
		if(q) *q++ = 0;
		m_szUser = p;
		if(q && *q)
		{
			if(m_szUser.length() > 0)
			{
				m_szUser += " <";
				m_szUser += q;
				m_szUser += ">";
			}
			else
				m_szUser = q;
		}
	}

	if (strchr(m_szUser.c_str(), '@') == NULL)
	{
#ifdef MAILHOST
		m_szUser += '@';
		m_szUser += MAILHOST;
#else
		char buf[1024];
		gethostname(buf+1, sizeof(buf)-2);
		char * p = strchr(buf+1, '.');
		if (p == NULL)
			p = buf;
		p[0] = '@';
		m_szUser += p;
#endif
	}
}

MailTo::~MailTo ()
{
	delete [] m_pMail;
}

ostream& operator << (ostream& stm, MailTo& mailto)
{
	mailto.Parse();

	if (mailto.m_bHeader)
		return mailto.OutHeader(stm);
	else
		return mailto.OutBody(stm);
}

ostream& MailTo::OutHeader (ostream& stm)
{
	m_bHTML = (m_pMsgInfo && CheckIfHTML(*m_pMsgInfo->msgMsg));

	OutBegin(stm);

	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp, &tzp);

	stm
		<< "<form enctype=multipart/form-data method=POST name=PostForm action="
		<< MAILBIN << "mailsend>" << endl
	;

	stm <<
		"<tr>\n"
		"<td bgcolor=#EAEFF8 width=80 height=20 valign=top>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;<a href=\"javascript:showSuggestion(DIR_HT, '*', "
		"'Tofield')\">To</a>:</b></font></td>\n"
		"<td bgcolor=#EAEFF8 height=20 valign=top>"
		"<input type=text name=mailto id=\"Tofield\" autocomplete=off "
		"onkeyup=\"showSuggestion(DIR_HT, this.value, 'Tofield')\" "
		"onfocus=\"hideSuggestion()\" "
		"onfocusout=\"return false;\" size=45 value=\""
	;
	if(m_pOpt->GetMailType() == M_REPLY ||
	   m_pOpt->GetMailType() == M_REPLYALL)
		stm << HTMLText(m_pOpt->GetToAddr());
	stm <<
		"\">&nbsp;<input type=checkbox name=apprasl><font face=Arial "
		"size=2 color=#396BB5>Add to ASL</td>\n"
		"</tr>\n"
		"<tr>\n"
		"<td bgcolor=#EAEFF8 width=80 height=20>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;From:</b></font></td>\n"
		"<td bgcolor=#EAEFF8 height=20>"
		"<input type=text name=from size=45 value=\""
		<< HTMLText(m_szUser)
		<< "\" onfocus=\"hideSuggestion()\"></td>\n"
		"</tr>\n"
		"<tr>\n"
		"<td bgcolor=#EAEFF8 width=80 height=20>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;Subject:</b></font></td>\n"
		"<td bgcolor=#EAEFF8 height=20>"
		"<input type=text name=subject size=45"
	;
	const char * pCharset = gMimeText.GetCharset();
	if(m_pSubject != NULL)
	{
		stm << " value=\"";
		if(strncasecmp(gMimeText.GetText(m_pSubject), "Re:", 3) != 0)
			stm << "Re: ";
		stm << HTMLText(gMimeText.GetText(m_pSubject)) << "\"";
		const char * pSubjCharset = gMimeText.GetCharset();
		if (pSubjCharset && *pSubjCharset)
			pCharset = pSubjCharset;
	}
	stm <<
		" onfocus=\"hideSuggestion()\"></td>\n"
		"</tr>\n";
	if (pCharset && *pCharset)
		stm << "<meta http-equiv=\"Content-Type\" "
			<< "content=\"text/html; charset="
			<< pCharset << "\">" << endl;

	stm <<
		"<tr>\n"
		"<td bgcolor=#EAEFF8 width=80 height=20>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;<a href=\"javascript:showSuggestion(DIR_HT, '*', "
		"'Ccfield')\">Cc</a>:</b></font></td>\n"
		"<td bgcolor=#EAEFF8 height=20>"
		"<input type=text name=cc id=\"Ccfield\" autocomplete=off "
		"onkeyup=\"showSuggestion(DIR_HT, this.value, 'Ccfield')\" "
		"onfocus=\"hideSuggestion()\" "
		"onfocusout=\"return false;\"></td>\n"
		"</tr>\n"
		"<tr>\n"
		"<td bgcolor=#EAEFF8 width=80 height=20>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;<a href=\"javascript:showSuggestion(DIR_HT, '*', "
		"'Bccfield')\">Bcc</a>:</b></font></td>\n"
		"<td bgcolor=#EAEFF8 height=20>"
		"<input type=text name=bcc id=\"Bccfield\" autocomplete=off "
		"onkeyup=\"showSuggestion(DIR_HT, this.value, 'Bccfield')\" "
		"size=45 onfocus=\"hideSuggestion()\" "
		"onfoucusout=\"return false;\">"
		"&nbsp;&nbsp;<span id=spellchkmsg class=spellchkmsg>&nbsp;</span>"
		"</td>\n"
		"</tr>\n"
		"<tr style=\"display:none\">\n"
		"<td bgcolor=#EAEFF8 width=80 height=20 valign=top>"
		"<span class=message>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;Attachment:</b></font></span></td>\n"
		"<td bgcolor=#EAEFF8 height=20 id=attFiles valign=top "
		"style=\"font-family:Arial; font-size:80%; color:#396BB5;\"></td>\n"
		"</tr>\n"
		"<tr id=attuploads style=\"display:;\">\n"
		"<td bgcolor=#EAEFF8 width=80 height=20>"
		"<span class=message>"
		"<font face=Arial size=2 color=#396BB5>"
		"<b>&nbsp;Attachment:</b></font></span></td>\n"
		"<td bgcolor=#EAEFF8 height=20>"
		"<input type=file name=filename size=45 onfocus=\"hideSuggestion()\" onchange=\"gotFile(this);\"></td>\n"
		"</tr>\n"
	;

	for (int i = 0; i < 20; i++)
	{
	    stm <<
		"<tr style=\"display:none;\">"
		"<td bgcolor=#EAEFF8 width=80 height=20>&nbsp;</td>\n"
		"<td bgcolor=#EAEFF8 height=20 valign=top>"
		"<input type=file name=filename size=45 onfocus=\"hideSuggestion()\" onchange=\"gotFile(this);\"> "
		"<font face=Arial size=1 color=#396BB5>"
		"Repeat for multiple attachments</font></td>\n"
		"</tr>\n"
	    ;
	}

	stm <<
		"<tr>\n"
		"<td bgcolor=#EAEFF8 height=7 colspan=2>"
		"<img border=0 src=" << DIR_IMG << "spacer_1pixel.gif width=1 height=1>"
		"</td>\n"
		"</tr>\n"
	;
	stm << "<input type=hidden name=foldername value=\""
		<< HTMLText(m_pFoldername)
	;
	int nPOP = m_pOpt->GetPOPAccount();
	if(nPOP > 0 && nPOP <= MAXPOPS)
		stm << nPOP;
	stm << "\" onfocus=\"hideSuggestion()\">" << endl;
	if(m_pOpt->GetMailIdx() > 0)
	{
		char buf[20];
		sprintf(buf, "%d", m_pOpt->GetMailIdx());
		stm << "<input type=hidden name=mailidx value=\""
			<< HTMLText(buf) << "\">" << endl
			<< "<input type=hidden name=mailid value=\""
			<< HTMLText(m_pOpt->GetHexMailID()) << "\">"
			<< "<input type=hidden name=mailstat value=\""
			<< m_pOpt->GetUpdStatus() << "\">"
			<< endl
		;
	}
	stm <<
		"<input type=hidden name=MailHTML value=\"\">\n"
		"<input type=hidden name=MailBody>\n"
		"</form>\n"
		"</table>\n"
		"<div id=\"suggestDiv\" onmouseout=\"this.style.cursor="
		"'default'\" onmouseover=\"this.style.cursor='pointer'\" "
		"style=\"z-index:99;\">"
		"</div>\n"
		"<div id=\"TextControl\">"
		"<div id=\"TextSpell\"></div>\n"
	;

	OutBody (stm) <<
		"</div>\n"
		"<script>initSpell();</script>\n";
	;

	return stm;
}

ostream& MailTo::OutBody (ostream& stm)
{
	m_bHTML = (m_pMsgInfo && CheckIfHTML(*m_pMsgInfo->msgMsg));
	stm <<
		"<textarea id=TextBody name=MailBody rows=20 cols=80 "
		"wrap=PHYSICAL"
	;
	if (m_bHTML)
		stm << " style=\"visibility: hidden\"";
	stm << ">\n";

	char dateBuf[256];
	if (m_pDate)
	{
		INDateTime mdate(m_pDate);
		mdate.SetTZ(m_pOpt->GetTZName());
		tm t = mdate.localtime();
		int tz_min = t.tm_gmtoff/60;
		char s[2] = {0, 0};
		if (tz_min < 0)
		{
			s[0] = '-';
			tz_min = -tz_min;
		}
		//sprintf(dateBuf, "%s, %2d %s %4d %02d:%02d:%02d %s%02d%02d (%s)\n",
		sprintf(dateBuf, "%s, %2d %s %4d %02d:%02d:%02d %s%02d%02d (%s)",
			WeekDays[t.tm_wday], t.tm_mday,
			Months[t.tm_mon], t.tm_year+1900,
			t.tm_hour, t.tm_min, t.tm_sec,
			s, tz_min/60, tz_min%60, t.tm_zone); 
	}
	else
		dateBuf[0] = 0;

	if(m_pOpt->GetMailType() == M_REPLY ||
	   m_pOpt->GetMailType() == M_REPLYALL)
	{
		if (m_pMsgInfo)
		{
			if (!m_bHTML)
				stm << "At " << HTMLText(dateBuf)
					<< ", you wrote\r";
			PrintBody(stm, *m_pMsgInfo->msgMsg);
		}
	}
	else if(m_pOpt->GetMailType() == M_FORWARD)
	{
		if (m_pMsgInfo)
		{
		    if (!m_bHTML)
		    {
			if(m_pReturn != NULL)
				stm << ">Return-Path: "
					<< HTMLText(m_pReturn) << "\r"
				;
			stm << ">Date: " << HTMLText(dateBuf) << "\r";
			stm << ">From: " << HTMLText(m_pFrom) << "\r";
			stm << ">To: " << HTMLText(m_pTo) << "\r";
			stm << ">Subject: " << HTMLText(gMimeText.GetText(m_pSubject)) << "\r>\r";
		    }
		    PrintBody(stm, *m_pMsgInfo->msgMsg);
		}
	}
	stm <<
		"</textarea>\n"
	;

	if (m_bHTML)
	{
		stm 
		    << "<div style=\"margin: 0px; padding: 0px; position: absolute; top:0px; left:0px; width=660; height=350;\">\n"
		    << "<form name=myform onsubmit=\"return submitForm();\">\n"
		    << "<script>\n"
		    << "<!--\n"
		    << "function submitForm()\n"
		    << "{\n"
		    << "    updateRTE('rte1');\n"
   		    << "    return false;\n"
		    << "}\n"
		    << "initRTE(\"/images/\", \"/drm/css/\", \"\", \"/drm5/\");\n"
		    << "//-->\n"
		    << "</script>\n"
		;

		ZString tFrom = gMimeText.GetText(m_pFrom);
		stm << "<script>\n"
		       //"var ostg = \"<div><br>\\n<br>\\n</div><hr><!-- DRM QUOTE -->\\n\";\n"
		       "var ostg = \"<p><br>\\n<br>\\n</p><hr><!-- DRM QUOTE -->\\n\";\n"
		       "ostg += \"<b>From:</b> " << HTMLText(tFrom) << "<br>\\r\";\n"
		       "ostg += \"<b>Sent:</b> " << HTMLText(dateBuf) << "<br>\\r\";\n"
		       "ostg += \"<b>To:</b> " << HTMLText(m_pTo) << "<br>\\r\";\n"
		       "ostg += \"<b>Subject:</b> " << HTMLText(gMimeText.GetText(m_pSubject)) << "<br>\\r\";\n"
		       "ostg += \"<p>\\r\";\n"
		       "ostg += document.getElementById('TextBody').value;\n"
		       "writeRichText('rte1', ostg, 660, 340, false, false);\n"
		       "</script>\n"
		       "</form>\n"
		       "</div>\n"
		;

	}

	return stm;
}

BOOL MailTo::IsStatus (char cStatus)
{
	if(m_pStatus)
		return (strchr(m_pStatus, cStatus) != NULL);
	return FALSE;
}

void MailTo::OutBegin (ostream& stm)
{
	stm <<
		"<link rel=\"stylesheet\" type=\"text/css\" href=\""
			<< DIR_CSS << "drmmenu.css\" />\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\""
			<< DIR_CSS << "drmmailto.css\" />\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\""
			<< DIR_CSS << "ktedit.css\" />\n"
		"<script src=" << DIR_JS << "drmwin.js></script>\n"
		"<script src=" << DIR_JS << "drmmailto.js></script>\n"
		"<script src=" << DIR_JS << "postwinadj.js></script>\n"
		"<script src=" << DIR_JS << "ajax.js></script>\n"
		"<script src=" << DIR_JS << "ktedit.js></script>\n"
		"<script language=javascript>\n"
		"<!--\n"
		"var DIR_HT = '" << DIR_HT << "';\n"
		"var DIR_IMG = '" << DIR_IMG << "';\n"
		"var DIR_BIN = '" << MAILBIN << "';\n"
		"var isHTML = " << (m_bHTML?"true":"false") << ";\n"
		"//-->\n"
		"</script>\n"
		"<script src=" << DIR_JS << "drmedit.js></script>\n"
	;

	if (m_bHTML)
		stm << "<script src=" << DIR_JS << "richtext.js></script>\n";

	stm <<

		"</head>\n"
		"<body onload=setform style=\"font-family: Arial; font-size: 90%\">\n"
		"<div id=\"stitleBar\">"
		"<img src=\"" << DIR_IMG << "toolbartop.gif\" height=\"28\" "
		"width=\"100%\"></div>\n"
		"<div id=\"mainMenu\" class=\"mainMenu\" "
		"style=\"background-image:url('"
		"" << DIR_IMG << "menubg.gif');\">\n"
		"<div style=\"margin:0; padding: 4px 15px 0 80px;\">\n"
		"<ul id=\"menuList1\" class=\"menuList\">\n"
		"<li>\n"
		"<a class=\"selector\" href=\"javascript:SendMessage()\">"
		"<b><span id=sendmessage>Send Message</span></b></a>\n"
		"</li>\n"
		"| <li>\n"
		"<a class=\"selector\" href=\"javascript:startSpell()\">"
		"<b><span id=spellchklnk>Spell Check</span></b></a>\n"
		"</li>\n"
		"| <li>\n"
		"<a class=\"selector\" href=\"" << MAILBIN << "prefspell\">"
		"<b>Dictionary</b></a>\n"
		"</li>\n"
		"| <li>\n"
		"<a class=\"selector\" href=\"/cgi-bin/ulogout\">"
		"<b>Logout</b></a></li>\n"
		"</ul>\n"
		"</div>\n"
		"</div>\n"
		"<div style=\"position:absolute;top:0;left:10px;\">"
		"<img id=\"stitleImage\" "
		"src=\"" << DIR_IMG << "stitlelogo.gif\" border=0></div>\n"
		"<table border=0 cellpadding=0 cellspacing=0 width=100%>\n"
		"<tr>\n"
		"<td bgcolor=#EAEFF8 colspan=2 height=7><img border=0 "
		"src=" << DIR_IMG << "spacer_1pixel.gif width=1 height=1></td>\n"
	;
}

void MailTo::Parse ()
{
	if(m_pMail)
	{
		char * p, * q;
		p = m_pMail;
		while(*p)
		{
			q = strchr(p, '\n');
			if(q) *q = 0;
			if(*p == 0)
			{
				// reach the body section
				//m_pBody = q+1;
				return;
			}
			if(strncasecmp(p, "Date: ", 6) == 0)
			{
				if(m_pDate == NULL)
				{
					p += 4;
					while(isspace(*++p));
					m_pDate = p;
				}
			}
			else if(strncasecmp(p, "From: ", 6) == 0)
			{
				if(m_pFrom == NULL)
				{
					p += 4;
					while(isspace(*++p));
					m_pFrom = p;
				}
			}
			else if(strncasecmp(p, "Return-Path: ", 13) == 0)
			{
				if(m_pReturn == NULL)
				{
					p += 11;
					while(isspace(*++p));
					m_pReturn = p;
				}
			}
			else if(strncasecmp(p, "Subject: ", 9) == 0)
			{
				if(m_pSubject == NULL)
				{
					p += 7;
					while(isspace(*++p));
					m_pSubject = p;
				}
			}
			else if(strncasecmp(p, "Status: ", 8) == 0)
			{
				if(m_pStatus == NULL)
				{
					p += 6;
					while(isspace(*++p));
					m_pStatus = p; 
				}
			}
			else if(strncasecmp(p, "To: ", 4) == 0)
			{
				if(m_pTo == NULL)
				{
					p += 2;
					while(isspace(*++p));
					m_pTo = p;
				}
			}
			if(q == NULL)
				break;
			p = q + 1;
		}
	}
}

bool
MailTo::CheckIfHTML (ZMIME &msg)
{
	if(&msg == NULL)
		return false;
	
	if (msg.IsPlainAndHTML())
	{
		return true;
	}

	const char * pBody = msg.GetBody();

	if (pBody)
	{
		if (msg.IsHTML())
			return true;
	}

	int n = msg.GetNumAtt();
	if (n)
	{
		for(int i = 0; i < n; i++)
		{
			if (CheckIfHTML(msg.GetAtt(i)))
				return true;
		}
	}

	return false;
}

void
MailTo::PrintBody (ostream& stm, ZMIME &msg)
{
	if(&msg == NULL)
		return;
	
	if (msg.IsPlainAndHTML())
	{
		PrintBody(stm, msg.GetAtt(1));
		return;
	}

	msg.DecodeBody();
	const char * pBody = msg.GetBody();

	if (pBody)
	{
		if (msg.IsPlain() && m_bHTML)
			stm << "<pre>" << endl;

		if (msg.IsPlain() || msg.IsHTML())
		{
			const char * pEnd = pBody + msg.GetBodyLen();
			for(const char * p = pBody; p && *p && (pBody < pEnd);
				pBody = p+1)
			{
				if (!m_bHTML)
					stm << '>';
				p = strchr(pBody, '\n');
				if (p)
					stm << HTMLText(pBody, p-pBody);
				else
					stm << HTMLText(pBody);
				stm << "\r";
			}
		}

		if (msg.IsPlain() && m_bHTML)
			stm << "</pre>" << endl;
	}

	int n = msg.GetNumAtt();
	if (n)
	{
		for(int i = 0; i < n; i++)
		{
			PrintBody(stm, msg.GetAtt(i));
		}
	}
}
