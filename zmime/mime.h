#ifndef _MIME_H_
#define _MIME_H_

struct mime_header;

/* mime_header routines */
const char *mime_getvalue(struct mime_header *, const char *);
const char *mime_getvaluec(struct mime_header *, const char *, int *);
/* general mime parser routines */
struct mime_header *mime_parseheader(char *, int, char **, const char *);
void mime_headerdelete(struct mime_header *);

#endif /* _MIME_H_ */
