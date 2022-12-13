#ifndef __NNTPART_H__
#define __NNTPART_H__

#include <string>

class CNNTP;
class NNTPArticle
{
public:
	NNTPArticle(const char * group, const char * artnum, bool hdronly = false);
	NNTPArticle(CNNTP * nntp, const char * group, const char * artnum, bool hdronly = false);
	~NNTPArticle();
	char * GetLine();

protected:
	void New(CNNTP * nntp, const char * group, const char * artnum, bool hdronly = false);

protected:
	string	m_group;
	CNNTP	* m_pNNTP;
	CNNTP	* m_pNew;

	char	* m_art;
	char	* m_end;
	char	* m_body;
	int	m_size;
};

#endif // __NNTPART_H__
