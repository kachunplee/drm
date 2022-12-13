#ifndef _MISC_PRIV_H_
#define _MISC_PRIV_H_

struct attrib {
	int a_n;
	int a_alloc;
	char **a_name;
	char **a_value;
};

#include <mimemisc.h>

#endif /* _MISC_PRIV_H_ */
