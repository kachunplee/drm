#ifndef __CDB_H__
#define __CDB_H__

#include <string>
#include <db_cxx.h>

template <class K, class T>
class CDb : public Db
{
public:
	CDb (DbEnv *env, u_int32_t flags)
	    : Db(env, flags)
	{
		m_dbc = NULL;
	}

	~CDb ()
	{
		close_cursor();
		close(0);
	}

	int
	insert (K &kdata, T &rdata, int flag = 0, int nr = sizeof(T), int nk = sizeof(K))
	{
		Dbt key ((void*) &kdata, nk);
		Dbt data ((void*) &rdata, nr);
		return put (NULL, &key, &data, flag);
	}

	int
	remove (K &kdata, int nk = sizeof(K))
	{
		Dbt key ((void*) &kdata, nk);
		return del (NULL, &key, 0);
	}

	int
	select (K &kdata, T **ppr, int *pnr = NULL, int nk = sizeof(K))
	{
		Dbt key ((void*) &kdata, nk);
		Dbt data;
		int ret = get(NULL, &key, &data, 0);
		if (ret == 0)
		{
			*ppr = (T *) data.get_data();
			if (pnr) *pnr = data.get_size();
		}
		return ret;
	}

	int
	find (K &kdata, T **ppr, int *pnr = NULL, int nk = sizeof(K))
	{
		Dbt key ((void*) &kdata, nk);
		Dbt data;

		if (m_dbc == NULL)
			cursor(NULL, &m_dbc, 0);

		int ret  = m_dbc->get(&key, &data, DB_SET);
		if (ret == 0)
		{
			*ppr = (T *) data.get_data();
			if (pnr) *pnr = data.get_size();
		}
		return ret;
	}

	int
	set_cursor (K **ppk, T **ppr, int curpos = DB_SET,
		int *pnk = NULL, int *pnr = NULL)
	{
		Dbt key;
		Dbt data;

		if (m_dbc == NULL)
			cursor(NULL, &m_dbc, 0);

		int ret  = m_dbc->get(&key, &data, curpos);
		if (ret != 0)
			return ret;

		if (ppk) *ppk = (K*) key.get_data();
		if (pnk) *pnk = key.get_size();
		if (ppr) *ppr = (T*) data.get_data();
		if (pnr) *pnr = data.get_size();
		return 0;
	}

	int
	next (K **ppk, T **ppr, bool begin = false,
		int *pnk = NULL, int *pnr = NULL)
	{
		return set_cursor(ppk, ppr, begin ? DB_FIRST : DB_NEXT, pnk, pnr);
			
	}

	int
	previous (K **ppk, T **ppr, bool last = false,
		int *pnk = NULL, int *pnr = NULL)
	{
		return set_cursor(ppk, ppr, last ? DB_LAST : DB_PREV, pnk, pnr);
	}

	int
	close_cursor ()
	{
		if (m_dbc == NULL)
			return 0;
		return m_dbc->close();
	}

	int
	update (K &kdata, T &rdata, int flag = DB_CURRENT, int nr = sizeof(T), int nk = sizeof(K))
	{
		if (m_dbc == NULL)
		{
			cursor(NULL, &m_dbc, 0);
			next(NULL, NULL, true);
		}

		Dbt key ((void*) &kdata, nk);
		Dbt data ((void*) &rdata, nr);
		return m_dbc->put (&key, &data, flag);
	}

	int
	sync ()
	{
		return sync(0);
	}

protected:
	Dbc   * m_dbc;
	const char * m_file;
};

#endif // __CDB_H__
