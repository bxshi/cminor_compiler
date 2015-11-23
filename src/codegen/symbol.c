#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>

struct symbol * symbol_create(symbol_t kind, struct type *type, char *name) {
	struct symbol *sym = malloc(sizeof(*sym));
	sym->kind = kind;
	sym->which = -1;
	sym->type = type;
	sym->name = name;

	return sym;

}

struct symbol * symbol_create_param(int idx, struct type *type, char *name) {
	struct symbol *sym = malloc(sizeof(*sym));
	sym->kind = SYMBOL_PARAM;
	sym->which = idx;
	sym->type = type;
	sym->name = name;

	return sym;
}

void symbol_print(struct symbol *sym) {
	switch(sym->kind) {
		case SYMBOL_LOCAL:
			printf("local %s\n", sym->name);
			break;
		case SYMBOL_PARAM:
			printf("param %d\n", sym->which);
			break;
		case SYMBOL_GLOBAL:
			printf("global %s\n", sym->name);
	}

}
