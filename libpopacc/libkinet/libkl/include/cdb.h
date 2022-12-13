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
		if (m_dbc)
			m_dbc->close();
		close(0);
	}

	int
	insert (K * pk, T *pr, int flag = 0, int nr = sizeof(T), int nk = sizeof(K))
	{
		Dbt key ((void*) pk, nk);
		Dbt data ((void*) pr, nr);
		return put (NULL, &key, &data, flag);
	}

	int
	remove (K * pk, int nk = sizeof(K))
	{
		Dbt key ((void*) pk, nk);
		return del (NULL, &key, 0);
	}

	int
	select (K * pk, T **ppr, int *pnr = NULL, int nk = sizeof(K))
	{
		Dbt key ((void*) pk, nk);
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
	find (K * pk, T **ppr, int *pnr = NULL, int nk = sizeof(K))
	{
		Dbt key ((void*) pk, nk);
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
	next (K **pk, T **pr, bool begin = false,
		int *pnk = NULL, int *pnr = NULL)
	{
		Dbt key;
		Dbt data;

		if (m_dbc == NULL)
			cursor(NULL, &m_dbc, 0);

		int ret  = m_dbc->get(&key, &data, begin ? DB_FIRST : DB_NEXT);
		if (ret != 0)
			return ret;

		if (pk) *pk = (K*) key.get_data();
		if (pnk) *pnk = key.get_size();
		if (pr) *pr = (T*) data.get_data();
		if (pnr) *pnr = data.get_size();
		return 0;
	}

	int
	update (K * pk, T *pr, int flag = DB_CURRENT, int nr = sizeof(T), int nk = sizeof(K))
	{
		if (m_dbc == NULL)
		{
			cursor(NULL, &m_dbc, 0);
			next(NULL, NULL, true);
		}

		Dbt key ((void*) pk, nk);
		Dbt data ((void*) pr, nr);
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
