#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "dmsg.h"
#include "cnntp.h"
#include "nntpart.h"

extern const char * NNTPservers;
extern const char * NNTPauths;

NNTPArticle::NNTPArticle (const char * group, const char * artnum, bool hdronly)
{
	New(new CNNTP(NNTPservers, NNTPauths), group, artnum, hdronly);
	m_pNew = m_pNNTP;
}

NNTPArticle::NNTPArticle (CNNTP * nntp, const char * group, const char * artnum, bool hdronly)
{
	New(nntp, group, artnum, hdronly);
}

void
NNTPArticle::New (CNNTP * nntp, const char * group, const char * artnum, bool hdronly)
{
	m_pNew = NULL;
	m_art = NULL;
	m_size = 0;

	m_pNNTP = nntp;

	char * p;
	if(group[0] != '<' || group[strlen(group)-1] != '>')
	{
		if (m_group == group)
		{
			m_pNNTP->SetCmd("group %s\r\n", group);
			p = m_pNNTP->SendCmd();
			DMSG(1, "%s", p);
			if(strncmp(p, "211 ", 4) != 0) throw(p);
			m_group = group;
		}
	}
	else
		artnum = group;

	m_pNNTP->SetCmd("%s %s\r\n", hdronly?"HEAD":"ARTICLE", artnum);
	p = m_pNNTP->SendCmd();
	DMSG(1, "%s", p);
	if(strncmp(p, "220 ", 4) != 0) throw(p);
}

NNTPArticle::~NNTPArticle ()
{
	delete m_pNew;
	delete m_art;
}

char * NNTPArticle::GetLine ()
{
	if(m_pNNTP == NULL)
		return NULL;

	return m_pNNTP->GetLine();
}
