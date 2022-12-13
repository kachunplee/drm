#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#include <string>
#include "mboxlock.h"

//
//
MBoxLock::MBoxLock (const char * lockf)
{
	Lock(lockf);
}

MBoxLock::MBoxLock ()
{
}

MBoxLock::~MBoxLock ()
{
	UnLock();
}

bool
MBoxLock::Lock (const char * lockf)
{
	UnLock();
	m_lock = lockf;
	m_lock += ".lock";
	return Lock();
}

bool
MBoxLock::Lock ()
{
	pid_t filepid;
	pid_t pid = getpid();
	int fd = -1;
	int timeout = 30*60;
	while((fd = open(m_lock.c_str(), O_WRONLY|O_CREAT|O_EXCL|O_EXLOCK, 0664)) < 0)
	{
		// file locked by another proces, check if process is still alived
		if((fd = open(m_lock.c_str(), O_RDONLY, 0644)) >= 0)
		{
			char buf[1024];
			read(fd, buf, sizeof(buf));
			close(fd);

			filepid = atoi(buf);
			if(filepid == pid)
				return true;	// locked by current process, done
			if(kill(filepid, 0) < 0)
			{
				// process that locked the file is no longer exist,
				//	unlink the file
				unlink(m_lock.c_str());
			}
		}

		if (timeout <= 0)
			return false;

		timeout -= 2;
		sleep(2);		// File is locked by other process, wait
	}

	char buf[1024];
	int n = sprintf(buf, "%d@", pid) ;
	write(fd, buf, n);
	close(fd);
	return true;
}

bool
MBoxLock::UnLock ()
{
	if(m_lock.length() > 0)
	{
		pid_t pid;
		int fd;
		if((fd = open(m_lock.c_str(), O_RDONLY, 0644)) > 0)
		{
			char buf[1024];
			read(fd, buf, sizeof(buf));
			close(fd);

			pid = atoi(buf);
			if(pid == getpid())
				unlink(m_lock.c_str());
		}
		m_lock.resize(0);
		return true;
	}

	return false;
}

