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

NNTPArticle::NNTPArticle (const char * group, const char * artnum, bool hdronly)
{
	char * p;
	m_pNNTP = new ClientNNTP(NNTPservers, NNTPauths);

	if(group[0] != '<' || group[strlen(group)-1] != '>')
	{
		p = m_pNNTP->command("GROUP", group);
		DMSG(1, "%s", p);
		if(strncmp(p, "211 ", 4) != 0) throw(p);
	}
	else
		artnum = group;

	const char * cmd = (hdronly ? "HEAD" : "ARTICLE");
	p = m_pNNTP->command(cmd, artnum);

	DMSG(1, "%s", p);
	if(strncmp(p, hdronly?"221 ":"220 ", 4) != 0) throw(p);
}

NNTPArticle::~NNTPArticle ()
{
	delete m_pNNTP;
}

char * NNTPArticle::GetLine ()
{
	if(m_pNNTP == NULL)
		return NULL;

	return m_pNNTP->GetLine();
}
