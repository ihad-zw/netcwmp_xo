#ifndef _GETOPT_H_
#define _GETOPT_H_ 1



#if !defined(HAVE_GETOPT_H)

#ifdef __cplusplus
extern          "C" {
#endif

int   getopt(int, char *const *, const char *);
char *optarg;
int   optind, opterr, optopt, optreset;

#ifdef __cplusplus
}
#endif

#endif /* HAVE_GETOPT_H */

#endif
