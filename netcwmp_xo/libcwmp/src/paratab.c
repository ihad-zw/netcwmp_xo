/*
 * Implementation of the parameter table type.
 *
 * Author : Stephen Smalley, <sds@epoch.ncsc.mil>
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include "paratab.h"

static unsigned int parahash(struct hashtab *h, const void *key)
{
	const char *p, *keyp;
	unsigned int size;
	unsigned int val;

	val = 0;
	keyp = key;
	size = strlen(keyp);
	for (p = keyp; (p - keyp) < size; p++)
		val = (val << 4 | (val >> (8*sizeof(unsigned int)-4))) ^ (*p);
	return val & (h->size - 1);
}

static int paracmp(struct hashtab *h, const void *key1, const void *key2)
{
	const char *keyp1, *keyp2;

	keyp1 = key1;
	keyp2 = key2;
	return strcmp(keyp1, keyp2);
}


int paratab_init(struct paratab *s, unsigned int size)
{
	s->table = hashtab_create(parahash, paracmp, size);
	if (!s->table)
		return -1;
	s->nprim = 0;
	return 0;
}

