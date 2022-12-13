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
	void CloseNNTP();

protected:
	static ClientNNTP	* m_pNNTP;
	static string		m_servers;
};

#endif // __NNTPART_H__
