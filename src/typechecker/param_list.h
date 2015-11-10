
#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include "type.h"
#include "expr.h"
#include <stdio.h>
#include <stdlib.h>

struct expr;

struct param_list {
	char *name;
	struct type *type;
	struct expr *expr;
	struct symbol *symbol;
	struct param_list *next;
};

struct param_list * param_list_create( char *name, struct type *type, struct expr *expr, struct param_list *next );
void param_list_print( struct param_list *a );

#endif
