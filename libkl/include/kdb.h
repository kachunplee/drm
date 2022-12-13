#ifndef __KDB_H__
#define __KDB_H__

#include <fcntl.h>
#include <db.h>
#include <limits.h>

class KDB
{
public:
	KDB (const char * dbf, int flags = 0, int mode = 0644, DBTYPE type = DB_HASH)
	{
		if (flags & O_RDWR)
		{
			flags |= O_EXLOCK;
		}
		else
		{
			flags |= (O_SHLOCK|O_RDONLY);
		}

		m_pdb = dbopen(dbf, flags, mode, type, NULL);
	}

	bool
	IsOpen () { return m_pdb != NULL; }

	int
	Getfd ()  { return m_pdb->fd(m_pdb); }

	int
	Get (string & key, string & data, int flags = 0)
	{
		DBT keyt, datat;
		keyt.data = (void*)key.c_str();
		keyt.size = key.length();

		int ret = m_pdb->get(m_pdb, &keyt, &datat, flags);
		if (ret == 0)
		{
			data.copy((char *)datat.data, datat.size);
		}

		return ret;
	}

	int
	Add (const char * key, int keylen,
		const char * data, int datalen = 0, int flags = 0)
	{
		DBT keyt, datat;
		keyt.data = (void*)key;
		keyt.size = keylen?keylen:strlen(key);
		datat.data = (void*)data;
		datat.size = datalen?datalen:strlen(data);
		return m_pdb->put(m_pdb, &keyt, &datat, flags);
	}

	int
	Add (string & key, const char * data, int datalen = 0, int flags = 0)
	{
		DBT keyt, datat;
		keyt.data = (void*)key.c_str();
		keyt.size = key.length();
		datat.data = (void*)data;
		datat.size = datalen?datalen:strlen(data);
		return m_pdb->put(m_pdb, &keyt, &datat, flags);
	}

	int
	Add (string & key, string & data, u_int flags = 0)
	{
		DBT keyt, datat;
		keyt.data = (void*)key.c_str();
		keyt.size = key.length();
		datat.data = (void*)data.c_str();
		datat.size = data.length();
		return m_pdb->put(m_pdb, &keyt, &datat, flags);
	}

	bool
	AddStg (string & key, unsigned long n)
	{
		char buf[20];
		sprintf("%lu", buf);
		return Add(key, buf);
	}

	int
	Delete (const char * key, int keylen = 0, u_int flags = 0)
	{
		DBT keyt;
		keyt.data = (void*)key;
		keyt.size = keylen?keylen:strlen(key);
		return m_pdb->del(m_pdb, &keyt, flags);
	}

	int
	Delete (string & key)
	{
		return Delete(key.c_str(), key.length());
	}

	~KDB ()
	{
		if (m_pdb)
			m_pdb->close(m_pdb);
	}

protected:
	DB	*m_pdb;
};

#endif // __KDB_H__
