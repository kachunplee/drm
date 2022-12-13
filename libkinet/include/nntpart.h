#ifndef __NNTPART_H__
#define __NNTPART_H__

class ClientNNTP;
class NNTPArticle
{
public:
	NNTPArticle(const char * group, const char * artnum, bool hdronly = false);
	~NNTPArticle();
	char * GetLine();

protected:
	ClientNNTP	* m_pNNTP;
};

#endif // __NNTPART_H__
