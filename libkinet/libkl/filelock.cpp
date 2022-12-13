#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <string>
#include "filelock.h"

//
//
FileLock::FileLock (const char * dir, const char * pName)
{
	Lock(dir, pName);
}

FileLock::FileLock ()
{
}

FileLock::~FileLock ()
{
	UnLock();
}

bool
FileLock::Lock (const char * dir, const char * pName)
{
	UnLock();
	if (dir[0] != 0)
	{
		m_lock = dir;
		m_lock += "/";
	}
	m_lock += pName;
	m_lock += ".LCK";
	pid_t filepid;
	pid_t pid = getpid();
	int fd = -1;
	while((fd = open(m_lock.c_str(), O_WRONLY|O_CREAT|O_EXCL|O_EXLOCK, 0644)) < 0)
	{
		// file locked by another proces, check if process is still alived
		if((fd = open(m_lock.c_str(), O_RDONLY, 0644)) >= 0)
		{
			int n = read(fd, (char *)(&filepid), sizeof(pid_t));
			close(fd);
			if(n < sizeof(pid_t))
			{
				unlink(m_lock.c_str());
				continue;
			}

			if(filepid == pid)
				return false;			// locked by current process, done
			if(kill(filepid, 0) < 0)
			{
				// process that locked the file is no longer exist,
				//	unlink the file
				unlink(m_lock.c_str());
			}
		}
		sleep(2);				// File is locked by other process, wait
	}
	write(fd, (char *)(&pid), sizeof(pid_t));
	close(fd);
	return true;
}

bool
FileLock::UnLock ()
{
	if(m_lock.length() > 0)
	{
		pid_t pid;
		int fd;
		if((fd = open(m_lock.c_str(), O_RDONLY, 0644)) > 0)
		{
			read(fd, (char *)(&pid), sizeof(pid_t));
			close(fd);
			if(pid == getpid())
				unlink(m_lock.c_str());
		}
		m_lock.resize(0);
		return true;
	}

	return false;
}

