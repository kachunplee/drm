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

NNTPArticle::NNTPArticle (const char * group, const char * artnum, bool hdronly)
{
	m_pNNTP = new ClientNNTP(NNTPservers, NNTPauths, NNTPgrpchkf);

	if((group[0] == '<') && (group[strlen(group)-1] == '>'))
		artnum = group;
	else
	{
		grpinfo gi = m_pNNTP->group(group);
		if (gi.numart < 0)
			throw("Group command error");
	}

	const char * p = m_pNNTP->article(artnum, hdronly);
	if (p == NULL)
		throw("Cannot get article");
}

NNTPArticle::~NNTPArticle ()
{
	delete m_pNNTP;
}

char * NNTPArticle::GetLine ()
{
	if(m_pNNTP == NULL)
		return NULL;

	return m_pNNTP->GetDataLine();
}
