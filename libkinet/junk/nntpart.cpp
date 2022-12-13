#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "dmsg.h"
#include "cnntp.h"
#include "clientnntp.h"
#include "nntpart.h"

extern const char * NNTPservers;
extern const char * NNTPauths;

const char * NNTPgrpchkf = NULL;

ClientNNTP * NNTPArticle::m_pNNTP = NULL;
string NNTPArticle::m_servers;

NNTPArticle::NNTPArticle (const char * group, const char * artnum, bool hdronly)
{
	if ((m_pNNTP != NULL) && (strcmp(m_servers.c_str(), NNTPservers) != 0))
		CloseNNTP();

	if (m_pNNTP == NULL)
	{
		m_servers = NNTPservers;
		m_pNNTP = new ClientNNTP(NNTPservers, NNTPauths, NNTPgrpchkf);
	}

	if (m_pNNTP == NULL)
		throw("NNTPArticle: Cannot connect to server");

	if(group[0] == '<' || group[strlen(group)-1] == '>')
	{
		// fetch by message-id
		artnum = group;
	}
	else
	{
		if (strcmp(m_pNNTP->GetCurrentGroup(), group) != 0)
		{
			grpinfo gi = m_pNNTP->group(group);
			if (gi.numart < 0)
				throw("Group command error");
		}
	}

	const char * cmd = (hdronly ? "HEAD" : "ARTICLE");
	char * p = m_pNNTP->command(cmd, artnum);

	DMSG(1, "%s", p);
	if(strncmp(p, hdronly?"221 ":"220 ", 4) != 0) throw(p);
}

NNTPArticle::~NNTPArticle ()
{
	CloseNNTP();
}

void
NNTPArticle::CloseNNTP ()
{
	m_servers = "";
	ClientNNTP * nntp = m_pNNTP;
	m_pNNTP = NULL;
	delete nntp;
}

char * NNTPArticle::GetLine ()
{
	if(m_pNNTP == NULL)
		return NULL;

	return m_pNNTP->GetLine();
}
