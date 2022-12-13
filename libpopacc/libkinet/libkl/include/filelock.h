#ifndef __FILELOCK_H__
#define __FILELOCK_H__

class FileLock
{
public:
	FileLock(const char * dir, const char * name);
	FileLock();
	~FileLock();

	bool Lock(const char * dir, const char * name);
	bool UnLock();

protected:
	string	m_lock;
};
#endif // __FILELOCK_H__
