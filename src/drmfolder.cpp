#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <ndbm.h>
#include <dirent.h>
#include <string>

#include "linein.h"
#include "def.h"

char FOLDER_IN[] = "in";
char * BFolders[] = {"in", "out", "trash"};

struct folderInfo
{
	ZString		name;
	char		state;
	folderInfo *	next;
};

bool
Folderstat (const char * folder, char * pUser, int * pmail, int *pread)
{
	ZString stgFolder;
	if (strcasecmp(folder, "in") == 0)
	{
		stgFolder = DIR_MAIL;
		stgFolder += pUser;
	}
	else
	{
		stgFolder = DIR_LOCAL;
		stgFolder += pUser;
		stgFolder += "/mail/";
		stgFolder += folder;
	}

	int nummail = 0;
	int numread = 0;

	// Open DB  pUser/mailstat.db
        //    ($date, $mail, $read) = split(DB{folder})
	//    if ($date >= date(stgFolder))
	//	*pmail = $mail; *pread= $read; return true
	ZString stgDB = DIR_LOCAL;
	stgDB += pUser;
	stgDB += "/mailstat";
	datum key, data;
	DBM * pDB = dbm_open(stgDB.chars(), O_RDONLY, 0644);
	if (pDB)
	{
		key.dptr = (char *)folder;
		key.dsize = strlen(folder);

		data = dbm_fetch(pDB, key);
		const char * p = data.dptr;
		time_t d_time = 0;
		int nMail = -1;
		int nRead = -1;
		if (p)
		{
			ZString tmpstg(p, data.dsize);
			p = tmpstg.c_str();
			d_time = atoi(p);
			p = strchr(p, '\t');
			if (p)
			{
				p++;
				nMail = atoi(p);
				p = strchr(p, '\t');
				if (p)
				{
					p++;
					nRead = atoi(p);
				}
			}
		}
		dbm_close(pDB);

		time_t f_time = 0;
		struct stat sb;
		if(stat(stgFolder.chars(), &sb) == 0)
		{
			f_time = sb.st_mtime;
		}

		if (f_time > 0 && d_time >= f_time && nMail >= 0 && nRead >= 0)
		{
			*pmail = nMail;
			*pread = nRead;
			return true;
		}
	}

	LineIn in(stgFolder.chars());
	if(!in.IsOpen())
	{
		*pmail = nummail;
		*pread = numread;
		return false;
	}

	time_t now = time(NULL);
	bool bHeader = false;
	char * p;
	while((p = in.GetLine()))
	{
		if(!p) break;		// end of box, done

		if(strncmp(p, "From ", 5) == 0)
		{
			bHeader = true;
			++nummail;
			continue;
		}

		if (bHeader)
		{
		    if(strncmp(p, "Status: ", 8) == 0)
		    {
			if (strchr(p+8, 'R') != NULL)
				++numread;
		    }
		    else if (!*p)
		    {
			bHeader = false;
		    }
		}
	}

	*pmail = nummail;
	*pread = numread;
	char buf[PATH_MAX];
	sprintf(buf, "%ld\t%d\t%d", now, nummail, numread);
	pDB = dbm_open(stgDB.chars(), O_RDWR|O_CREAT|O_EXLOCK, 0644);
	if (pDB)
	{
		key.dptr = (char *)folder;
		key.dsize = strlen(folder);

		data.dptr = buf;
		data.dsize = strlen(buf);

		dbm_store(pDB, key, data, DBM_REPLACE);
		dbm_close(pDB);
	}
	return true; 
}

int
main (int ac, char **av)
{
	char * currentFolder = FOLDER_IN;
	if (ac == 2)
		currentFolder = av[1];

	char * pUser = getenv("REMOTE_USER");

	ZString FolderFile = DIR_LOCAL;
	FolderFile += pUser;
	FolderFile += "/Folders";

	folderInfo * pFolderInfo = NULL;
	folderInfo * pCurFI = NULL;
	int userfolder = 0;
	int foldercnt = 0;
	LineIn in(FolderFile.chars());
	if(in.IsOpen())
	{
		char * p;
		folderInfo * pFI;
		while((p = in.GetLine()))
		{
			//printf("%s\n", p);

			pFI = new folderInfo;
			pFI->state = 0;
			pFI->next = NULL;
			int l = strlen(p);
			if (l > 0)
				l--;
			if (p[l] == '+' || p[l] == '-')
			{
				pFI->state = p[l];
				p[l--] = 0;
			}
			pFI->name = p;
			if (p[l] != '/')
				foldercnt++;

			if(pFolderInfo == NULL)
			{
				pFolderInfo = pFI;
				pCurFI = pFI;
			}
			else
			{
				pCurFI->next = pFI;
				pCurFI = pFI;
			}
			userfolder++;
		}
	}

	ZString stgFolders;
	ZString stgFolderState;
	for (pCurFI = pFolderInfo; pCurFI; pCurFI = pCurFI->next)
	{
		if (stgFolders.length() > 0)
		{
			stgFolders += ", ";
			stgFolderState += ", ";
		}

		stgFolders += "'";
		stgFolders += pCurFI->name;
		stgFolders += "'";
		stgFolderState += "\"";
		if (pCurFI->state) stgFolderState += pCurFI->state;
		stgFolderState += "\"";
	}

	ZString stgBFolders;
	int i;
	int s = (size_t)sizeof(BFolders)/sizeof*(BFolders);
	for(i = 0; i < s; i++)
	{
		if (i > 0)
			stgBFolders += ", ";
		stgBFolders += "'";
		stgBFolders += BFolders[i];
		stgBFolders += "'";
	}

	float storageuse = 0;
	float storageleft = 0;
	int maxsize = 50000;

	FILE *fp;
	char path[PATH_MAX];
	sprintf(path, "/usr/local/bin/dspaccntquota -u %s", pUser);
	fp = popen(path, "r");
	if (fp)
	{
		char * pPath;
		while(fgets(path, PATH_MAX, fp) != NULL)
		{
			pPath = path;
			while(isspace(*pPath)) pPath++;
			if (*pPath)
			{
				if (strncmp(pPath, "/v0", 3) != 0)
					continue;
				while(!isspace(*pPath)) pPath++;
				while(isspace(*pPath)) pPath++;
				storageuse = atoi(pPath);
				while(!isspace(*pPath)) pPath++;
				while(isspace(*pPath)) pPath++;
				maxsize = atoi(pPath);
			}
		}
		pclose(fp);
	}
	if (maxsize > storageuse)
	{
		storageleft = (maxsize - storageuse) / 1000;
	}
	if (maxsize == 0) maxsize = 1;
	maxsize *= 1024;
	if (storageuse > 0)
	{
		storageuse = int((storageuse * 1024 * 100 / maxsize) + 0.5);
		storageuse = int((storageuse * 132 / 100) + 0.5);
	}
	if (storageuse > 132)
		storageuse = 132;
	int storageremain = 132 - int(storageuse);

	printf("var listSelectAll = false;\n\n");
	printf("var foldercount = %d;\n", foldercnt);
	printf("var maxcount = %d;\n\n", MAXFOLDER);
	printf("var currentFolder = '%s';\n", currentFolder);
	printf("var BFolders = new Array(%s);\n", stgBFolders.chars());
	printf("var Folders = new Array(%s);\n", stgFolders.chars());
	printf("var FDiropens = new Array(%s);\n\n", stgFolderState.chars());
	printf("var fd_read = new Array();\n");
	printf("var fd_unread = new Array();\n\n");
	printf("var storage_left = %.2f;\n", storageleft);
	printf("var storage_use = %d;\n", int(storageuse));
	printf("var storage_remain = %d;\n", storageremain);

	int nummail, numread;
	const char * p;
	for(i = 0; i < s; i++)
	{
		p = BFolders[i];
		Folderstat(p, pUser, &nummail, &numread);
		printf("fd_read['%s'] = %d\nfd_unread['%s'] = %d;\n", p, numread, p,
			(nummail-numread));
	}
	for (pCurFI = pFolderInfo; pCurFI; pCurFI = pCurFI->next)
	{
		p = pCurFI->name;
		Folderstat(p, pUser, &nummail, &numread);
		printf("fd_read['%s'] = %d\nfd_unread['%s'] = %d;\n", p, numread, p,
			(nummail-numread));
	}

	exit(0);
}
