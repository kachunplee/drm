#ifndef __MBOXLOCK_H__
#define __MBOXLOCK_H__

class MBoxLock
{
public:
	MBoxLock(const char *lockf);
	MBoxLock();
	~MBoxLock();

	bool Lock(const char *lockf);
	bool Lock();
	bool UnLock();

protected:

protected:
	string	m_lock;
};
#endif // __MBOXLOCK_H__
