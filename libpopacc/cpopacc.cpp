#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <fstream>
#include <vector>

#include "dmsg.h"
#include "cpopacc.h"

extern int debug;

const char PrxMail [] = "/var/local/bin/procmail";

const int MAXERRMSG = 2048;
static char ErrMsg [MAXERRMSG];

const int INITMSGALLOC = 2048;

POPAccount::POPAccount (const char * servers, const char * auths,
	ostream * pstm)
	: m_status(pstm), CPOP3(servers, auths)
{
	DMSG(1, "Connected to POP server: %s", servers);
	m_status("Connected to POP server: %s", servers);
}

POPAccount::~POPAccount ()
{
}

int
POPAccount::Pull (const char * pUser, const char * pMbox)
{
	struct passwd *pw = getpwnam(pUser);
	if(pw == NULL)
	{
		snprintf(ErrMsg, MAXERRMSG, "No such user: %s", pUser);
		throw(ErrMsg);
	}

	char * p = SendCmd("stat\r\n");
	if(strncmp(p, "+OK", 3) != 0)
	{
		snprintf(ErrMsg, MAXERRMSG, "POP STAT Error: %s", p);
		throw(ErrMsg);
	}

	int noMsg = atoi(p+4);
	if(noMsg == 0)
	{
		m_status("No mail");
		return noMsg;
	}

	DMSG(1, "Got %d mails to %s", noMsg, pMbox?pMbox:"inbox");
	m_status("Got %d mails", noMsg);

	char user [strlen(pUser)+1];
	strcpy(user, pUser);

	if (pMbox)
	{
		struct stat sb;
		if((stat(pMbox, &sb) == -1) && (errno == ENOENT))
                {
			int fd = open(pMbox, O_CREAT|O_WRONLY, 0660);
			if (fd >= 0)
			{
				close(fd);
				struct group * pGroup = getgrnam("mail");
				int MailGID = (pGroup == NULL) ?
					pw->pw_gid : pGroup->gr_gid;
				chown(pMbox, pw->pw_uid, MailGID);
			}
                }
	}

	for(int i = 1; i <= noMsg; i++)
	{
		DMSG(1, "Retrieving %d of %d mails", i, noMsg);
		m_status("Retrieving %d of %d mails", i, noMsg);

		SetCmd("retr %d\r\n", i);
		p = SendCmd();
		if(strncmp(p, "+OK", 3) != 0)
		{
			snprintf(ErrMsg, MAXERRMSG, "POP RETR Error: %s", p);
			throw(ErrMsg);
		}


		//
		// Get Messages
		//
		vector<string> msg;
		while((p=GetLine()))
			msg.push_back(p);

		// Open PIPE
		int pipe1[2], pipe2[2];
		if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
		{
			snprintf(ErrMsg, MAXERRMSG, "Pipe error: %s",
				strerror(errno));
			throw(ErrMsg);
		}

		int child = vfork();
		if (child < 0)
		{
			snprintf(ErrMsg, MAXERRMSG, "Fork error: %s",
				strerror(errno));
			throw(ErrMsg);
		}

		if (child == 0)
		{
			close(pipe1[1]);
			close(pipe2[0]);
			close(0);
			close(1);
			dup(pipe1[0]);
			dup(pipe2[1]);

			const char * argv [6];
			int n = 0;
			if (pMbox)
			{
				argv[n++] = "-a";	// [0]
				argv[n++] = pMbox;	// [1]
			}

			argv[n++] = "-Y";		// [2]
			argv[n++] = "-d";		// [3]
			argv[n++] = user;		// [4]
			argv[n] = NULL;			// [5]

			// Try using execv,
			//	but it requires a char *const [] !!
			execl(PrxMail, "procmail",
				argv[0], argv[1], argv[2],
				argv[3], argv[4], argv[5]);
			fprintf(stderr, "Exec error: %s\n",
				strerror(errno));

			_exit(EXIT_FAILURE);
		}

		close(pipe1[0]);
		close(pipe2[1]);

		FILE * fp = fdopen(pipe1[1], "w");
		if (fp == NULL)
		{
			snprintf(ErrMsg, MAXERRMSG, "fdopen error: %s",
				strerror(errno));
			throw(ErrMsg);
		}

		int n = msg.size();
		for(int j = 0; j < n; j++)
		{
			if ((fputs(msg[j].c_str(), fp) == EOF) ||
				(fputc('\n', fp) == EOF))
			{
				snprintf(ErrMsg, MAXERRMSG, "pipe write error: %s",
					strerror(errno));
				throw(ErrMsg);
			}
		}

		fclose(fp);

		ifstream in;
		in.attach(pipe2[0]);
		string error;
		getline(in, error);
		in.close();
		
		int status;
		waitpid(child, &status ,NULL);
		if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0))
		{
			snprintf(ErrMsg, MAXERRMSG, "PrxMail error: %d %s",
				status, error.c_str());
			throw(ErrMsg);
		}

		// Delete mail
		SetCmd("dele %d\r\n", i);
		p = SendCmd();
		if(strncmp(p, "+OK", 3) != 0)
		{
			snprintf(ErrMsg, MAXERRMSG, "POP DELE Error: %s", p);
			throw(ErrMsg);
		}
	}

	return noMsg;
}
