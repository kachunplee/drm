#ifndef __POP3ACT_H__
#define __POP3ACT_H__

class CPOP3;
class POP3Active
{
public:
	POP3Active();
	~POP3Active();
	char * GetLine();

protected:
	CPOP3	* m_pPOP3;
};

#endif // __POP3ACT_H__
