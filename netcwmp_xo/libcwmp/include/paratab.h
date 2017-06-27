/*
 * A parameter table (paratab) maintains associations between symbol
 * strings and datum values.  The type of the datum values
 * is arbitrary.  The symbol table type is implemented
 * using the hash table type (hashtab).
 *
 * Author : Stephen Smalley, <sds@epoch.ncsc.mil>
 */
#ifndef __PARATAB_H__
#define __PARATAB_H__


#include "hashtab.h"


#define PARATAB_DEFAULT_SIZE	2048

struct paratab {
	struct hashtab *table;	/* hash table (keyed on a string) */
	unsigned int nprim;		/* number of primary names in table */
};

int paratab_init(struct paratab *s, unsigned int size);

#endif	/* _PARATAB_H_ */


