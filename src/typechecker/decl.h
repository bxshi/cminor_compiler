
#ifndef DECL_H
#define DECL_H

#include "type.h"
#include "stmt.h"
#include "expr.h"
#include <stdio.h>
#include <stdlib.h>

struct decl {
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
	int func_declaration; // does not have a function body
};

struct decl * decl_create_func(char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next, int func_decl);
struct decl * decl_create( char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next );
void decl_print( struct decl *d, int indent );

#endif


