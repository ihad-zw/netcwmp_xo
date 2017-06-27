#ifndef __URL_H__
#define __URL_H__

extern char * urlencode(char const *s, int len, int *new_length);
extern int urldecode(char *str, int len);

#endif //__URL_H__
