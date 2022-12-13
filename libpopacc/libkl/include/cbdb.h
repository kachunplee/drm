#ifndef __CBDB_H__
#define __CBDB_H__

#include <string>
#include <db_cxx.h>

template <class T>
class CBDB : public DbEnv
{
public:
	CBDB(const char * dbfile, int env_args = 0)
	: DbEnv (env_args)
	{
		m_file = dbfile;
		m_db = NULL;
		m_dbc = NULL;
	}

	~CBDB()
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

	void
	SetFile (const char * dbfile)		{ m_file = dbfile; }

	int
	open (const char *dbdir,
		const char * prog = NULL, int flags = 0, int mode = 0644,
		int env_mode = DB_CREATE|DB_INIT_CDB|DB_INIT_MPOOL,
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
		set_errpfx(prog ? prog : "CBDB");
		if (dbdir)
		{
			DbEnv::open(dbdir, env_mode, 0);
		}

		m_db = new Db(dbdir ? this : NULL, db_open);
#ifdef DB41plus
		return m_db->open(NULL, m_file, NULL, db_type, dbflags, mode);
#else
		return m_db->open(m_file, NULL, db_type, dbflags, mode);
#endif
	}

	int
	put (const char * k, T n, int s = sizeof(T))
	{
		Dbt key ((void*) k, strlen(k));
		Dbt data ((void*) &n, s);
		return m_db->put (NULL, &key, &data, 0);
	}

	int
	del (const char * k)
	{
		Dbt key ((void*) k, strlen(k));
		return m_db->del (NULL, &key, 0);
	}

	int
	get (const char * k, T * p, unsigned * pn = NULL, int partial = 0)
	{
		Dbt key ((void*) k, strlen(k));
		Dbt data ((void*) p, sizeof(T));
		data.set_flags(DB_DBT_USERMEM|partial);
		data.set_ulen(sizeof(T));
		if (partial&DB_DBT_PARTIAL)
		{
			data.set_dlen(sizeof(T));
			data.set_doff(0);
		}
		int ret = m_db->get(NULL, &key, &data, 0);
		if ((ret == 0) && pn)
		{
			*pn = data.get_size();
		}

		return ret;
	}

	int
	set (string &s, T *p, int partial = 0)
	{
		Dbt key ((void*) s.c_str(), s.length());
		Dbt data ((void*) p, sizeof(T));
		data.set_flags(DB_DBT_USERMEM|partial);
		data.set_ulen(sizeof(T));
		if (partial&DB_DBT_PARTIAL)
		{
			data.set_dlen(sizeof(T));
			data.set_doff(0);
		}

		if (m_dbc == NULL)
			m_db->cursor(NULL, &m_dbc, 0);

		int ret  = m_dbc->get(&key, &data, DB_SET_RANGE);
		if (ret != 0)
			return ret;

		s = string((const char *)key.get_data(), key.get_size());
		return 0;
	}

	int
	next (string * s, T *p, bool begin = false, int partial = 0)
	{
		Dbt key;
		Dbt data ((void*) p, sizeof(T));
		data.set_flags(DB_DBT_USERMEM|partial);
		data.set_ulen(sizeof(T));
		if (partial&DB_DBT_PARTIAL)
		{
			data.set_dlen(sizeof(T));
			data.set_doff(0);
		}

		if (m_dbc == NULL)
			m_db->cursor(NULL, &m_dbc, 0);

		int ret  = m_dbc->get(&key, &data, begin ? DB_FIRST : DB_NEXT);
		if (ret != 0)
			return ret;

		if(s) *s = string((const char *)key.get_data(), key.get_size());
		return 0;
	}

	int
	sync (const char * k)
	{
		return m_db->sync (0);
	}

	int
	cursor (Dbc **pDbc, u_int32_t flags)
	{
		return m_db->cursor(NULL, pDbc, flags);
	}

protected:
	Db    * m_db;
	Dbc   * m_dbc;
	const char * m_file;
};

#endif // __CBDB_H__
