#include "decl.h"

struct decl * decl_create(char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next) {
	struct decl *d = new struct decl;
	d->name = name;
	d->type = t;
	d->value = v;
	d->code = c;
	d->next = next;
	return d;
};
