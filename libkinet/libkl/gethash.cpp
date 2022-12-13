#include <fcntl.h>
#include <strings.h>
#include <db.h>

const int MAX_DATA = 1024;

char *
gethash (const char * u, const char * dbname)
{
	DBT key, data;
	static char p [MAX_DATA+1];
	p[0] = 0;
	data.size = 0;

	DB * db = dbopen(dbname, O_RDONLY, 0644, DB_HASH, (void*)0);

	if(db == (void *) 0)
		return NULL;

	key.data = (char *)u;
	key.size = strlen(u);

	int ret = db->get(db, &key, &data, 0);

	if (ret == 0)
	{
		int size = data.size < MAX_DATA ? data.size : MAX_DATA;
		strncpy(p, (char*)data.data, size);
		p[size] = 0;
	}
	db->close(db);

	return (ret < 0) ? NULL : p;
}
