#include <fcntl.h>

void
LockFile (const char * dir, const char * pName)
{
	string filename(dir);
	filename += pName;
	pid_t filepid;
	pid_t pid = getpid();
	int fd, n;
	while((fd = open(filename.c_str(), O_RDWR|O_CREAT|O_EXCL, 0644)) < 0)
	{
		// file locked by another proces, check if the process is
		//	still alived
		if((fd = open(filename.c_str(), O_RDONLY, 0644)) >= 0)
		{
			n = read(fd, (char *)(&filepid), sizeof(pid_t));
			close(fd);
			if(n == 0)
			{
				// File is empty
				if((fd = open(filename.c_str(),
					O_RDWR|O_TRUNC|O_EXCL, 0644)) >= 0)
					break;
				unlink(filename.c_str());
			}
			else
			{
				if(filepid == pid)
					return;		// locked by current
							//	process, done
				if(kill(filepid, 0) < 0)
				{
					// process that locked the file is no
					//	longer exist, overwrite the file
					if((fd = open(filename.c_str(),
						O_RDWR|O_TRUNC|O_EXCL,
						0644)) >= 0)
						break;
					unlink(filename.c_str());
				}
			}
		}
		sleep(10);	// File is locked by other process, wait
	}
	write(fd, (char *)(&pid), sizeof(pid_t));
	close(fd);
}

void
UnLockFile (const char * dir, const char * pName)
{
	string filename(dir);
	filename += pName;
	pid_t pid;
	int fd;
	if((fd = open(filename.c_str(), O_RDONLY, 0644)) > 0)
	{
		read(fd, (char *)(&pid), sizeof(pid_t));
		close(fd);
		if(pid == getpid())
			unlink(filename.c_str());
		else
			DMSG(0, "Cannot unlock %s%s", dir, pName);
	}
}
