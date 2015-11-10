#include "decl.h"

#include <stdio.h>
#include <stdlib.h>

struct decl * decl_create(char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next) {
	struct decl *d = (struct decl*)malloc(sizeof(*d));
	d->name = name;
	d->type = t;
	d->value = v;
	d->code = c;
	d->next = next;
	d->func_declaration = 1;
	return d;
};

struct decl * decl_create_func(char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next, int func_decl) {
	struct decl *d = (struct decl*)malloc(sizeof(*d));
	d->name = name;
	d->type = t;
	d->value = v;
	d->code = c;
	d->next = next;
	d->func_declaration = func_decl;
	return d;
};

void decl_print(struct decl *d, int indent) {

	if(indent > 0) printf("%*c", indent * 2, ' '); // indent
	printf("%s : ", d->name); // variable name

	if (d->type) {
		type_print(d->type);
	}

	if (d->value) {
		printf(" = ");
		if (d->type->kind == TYPE_ARRAY) printf("{");
		expr_print(d->value);
		if (d->type->kind == TYPE_ARRAY) {
			if(indent > 0) printf("%*c", indent * 2, ' '); // indent
			printf("}");
		}
	}

	if (d->type->kind == TYPE_FUNCTION && !d->func_declaration) {
		printf(" = {\n");
		stmt_print(d->code, indent + 1);
		if(indent > 0) printf("%*c", indent * 2, ' '); // indent
		printf("}\n");
	} else {
		printf(";\n");
	}

  if (d->next) decl_print(d->next, indent);	

}
