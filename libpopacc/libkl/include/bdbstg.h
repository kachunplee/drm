#ifndef __BDBSTG_H__
#define __BDBSTG_H__

#include <string>
#include <db_cxx.h>

typedef char * CPTR;

class DBStg : public DbEnv
{
public:
	DBStg (const char * dbfile = NULL, int env_args = 0)
	: DbEnv (env_args)
	{
		m_file = dbfile;
		m_db = NULL;
		m_dbc = NULL;
	}

	~DBStg()
	{
		if(m_dbc)
			m_dbc->close();
		
		if (m_db)
		{
			m_db->close(0);
			delete m_db;
		}

		close(0);
	}

	int
	Open (const char * dbdir, const char * dbfile)
	{
		SetFile(dbfile);
		return open(dbdir, "DBStg", O_CREAT);
	}

	void
	SetFile (const char * dbfile)		{ m_file = dbfile; }

	int
	open (const char *dbdir,
		const char * prog = NULL, int flags = 0, int mode = 0644,
		int env_mode = DB_CREATE|DB_INIT_MPOOL /*|DB_INIT_CDB*/,
		DBTYPE db_type = DB_HASH, int db_open = 0)
	{
		if (m_db)
			return -1;

		int dbflags = 0;
		if (flags == 0)
			flags = O_RDONLY;

		if (flags & O_CREAT)
			dbflags |= DB_CREATE;

		if (flags & O_EXCL)
			dbflags |= DB_EXCL;

		if (flags & O_RDONLY)
			dbflags |= DB_RDONLY;

		if (flags & O_TRUNC)
			dbflags |= DB_TRUNCATE;

		set_error_stream(&cerr);
		set_errpfx(prog ? prog : "DBStg");
		if (dbdir)
		{
			DbEnv::open(dbdir, env_mode, 0);
		}

		m_db = new Db(dbdir ? this : NULL, db_open);
		return m_db->open(NULL, m_file, NULL, db_type, dbflags, mode);
	}

	int
	put (const char * k, CPTR n)
	{
		Dbt key ((void*) k, strlen(k));
		Dbt data ((void*) n, strlen(n)+1);
		return m_db->put (NULL, &key, &data, 0);
	}

	int
	del (const char * k)
	{
		Dbt key ((void*) k, strlen(k));
		return m_db->del (NULL, &key, 0);
	}

	int
	get (const char * k, CPTR *p)
	{
		Dbt key ((void*) k, strlen(k));
		Dbt data;
		int ret = m_db->get(NULL, &key, &data, 0);
		if (ret == 0)
			*p = (CPTR) data.get_data();
		return ret;
	}

	int
	next (string * s, CPTR *p, bool begin = false)
	{
		Dbt key;
		Dbt data;

		if (m_dbc == NULL)
			m_db->cursor(NULL, &m_dbc, 0);

		int ret  = m_dbc->get(&key, &data, begin ? DB_FIRST : DB_NEXT);
		if (ret != 0)
			return ret;

		if(s) *s = string((const char *)key.get_data(), key.get_size());
		*p = (CPTR)data.get_data();
		return 0;
	}

	int
	sync (const char * k)
	{
		return m_db->sync (0);
	}

protected:
	Db    * m_db;
	Dbc   * m_dbc;
	const char * m_file;
};

#endif // __BDBSTG_H__
