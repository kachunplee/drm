#ifndef __DBSTG_H__
#define __DBSTG_H__

#include <unistd.h>
#include <string>
#include <errno.h>

#ifdef LINUX
#include <sys/file.h>
#include <db_185.h>
#else
#include <fcntl.h>
#include <db.h>
#endif

typedef char * CPTR;

//
// Not multi-thread safe...
//	see kdb.h, which is more generalize
//
class StgDB
{
public:
	StgDB (const char * dbf, int flag = O_RDONLY, int lock = 0)
	{
#ifdef LINUX
		m_db = NULL;

		if (lock)
		{
		retrylock:
			m_fd = open(dbf, flag&~O_CREAT);
			if (m_fd < 0)
			{
				if ((flag & O_CREAT) && (errno == ENOENT))
				{
					// if file not exist and
					// 	flag & O_CREAT
					// create the db
					m_db = dbopen(dbf, flag, 0644, DB_HASH, NULL);
					if (m_db)
					{
						m_db->close(m_db);
						goto retrylock;
					}
				}

				return;
			}

			flock(m_fd, lock);	
		}
#else
		if (lock & LOCK_EX)
			flag |= O_EXLOCK;
		if (lock & LOCK_SH)
			flag |= O_SHLOCK;
#endif
		m_db = dbopen(dbf, flag, 0644, DB_HASH, NULL);
	}

	bool
	IsOK ()
	{
		return m_db != NULL;
	}

	~StgDB ()
	{
		if (m_db) m_db->close(m_db);
#ifdef LINUX
		if (m_fd >= 0) close(m_fd);
#endif
	}

	int
	fd ()
	{
		return IsOK() ? m_db->fd(m_db) : -1;
	}

	char *
	get (const char * key, int klen = 0, int * plen = NULL)
	{
		m_key.data = (void *) key;
		m_key.size = klen ? klen : strlen(key);

		switch (m_db->get(m_db, &m_key, &m_data, 0))
		{
		case -1:
			return (char *) -1;

		case 1:
			return NULL;
		}

		if (plen != NULL)
			*plen = m_data.size;

		char * p = (char*)m_data.data;
		p[m_data.size] = 0;	// terminate string - yike! hope the buffer has that extra byte 
		return p;
	}

	int
	seq (char ** pkey, int *pnkey, char ** pdata, int *pndata,
		int flags = R_NEXT)
	{
		int res = m_db->seq(m_db, &m_key, &m_data, flags);
		if (res != 0)
			return res;

		*pkey = (char*)m_key.data;
		*pnkey = m_key.size;
		*pdata = (char*)m_data.data;
		*pndata = m_data.size;
		return res;
	}

	int
	put (const char * key, const char * data, int klen = 0, int dlen = 0)
	{
		m_key.data = (void *) key;
		m_key.size = klen ? klen : strlen(key);
		m_data.data = (void *) data;
		m_data.size = dlen ? dlen : strlen(data);
		return m_db->put(m_db, &m_key, &m_data, 0);
	}

	int
	del (const char * key, int keylen = 0)
	{
		m_key.data = (void*)key;
		m_key.size = keylen?keylen:strlen(key);
		return m_db->del(m_db, &m_key, 0);
	}

protected:
#ifdef LINUX
	int	m_fd;
#endif
	DB *	m_db;
	DBT	m_key, m_data;
};

#endif // __DBSTG_H__
