#ifndef _MISC_H_
#define _MISC_H_

struct attrib;

struct attrib *attrib_create(void);
/* make a copy of the strings and store the copy */
void attrib_add(struct attrib *, const char *, const char *);
/* char * must be allocated with malloc, we will free it when destroyed */
void attrib_addnodup(struct attrib *, char *, char *);
const char *attrib_get(struct attrib *, const char *, int *);
void attrib_free(struct attrib *);

char *bd_makenorm(const char *, const char *);
char *bd_maketail(const char *, const char *);

struct attrib *mime_getattrib(const char *ct);

#endif /* _MISC_H_ */
