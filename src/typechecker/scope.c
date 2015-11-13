#include "scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol.h"

struct scope *root = 0;
struct scope *curr = 0;
int scope_level = 0;

void init() {
	root = (struct scope *)malloc(sizeof(*root));
	root->h = hash_table_create(0,0);
	root->prev = 0;
	root->next = 0;
	curr = root;
}

void scope_enter() {
	scope_level++;
	struct scope *ptr = (struct scope *)malloc(sizeof(*ptr));

	ptr->prev = curr;
	ptr->next = 0;
	curr->next = ptr;

	curr = ptr;
	curr->h = hash_table_create(0,0);
}

void scope_exit() {
	scope_level--;
	if (root == 0) return;

	curr = curr->prev;

}

void scope_bind(const char *name, struct symbol *s) {
	hash_table_insert(curr->h, name, s);
}

struct symbol *scope_lookup(const char *name) {
	struct scope *ptr = curr;
	while(ptr != 0) {
		struct symbol *res = (struct symbol *)hash_table_lookup(ptr->h, name);
		if (res != 0) return res;
		ptr = ptr->prev;
	}

	return 0;
}

struct symbol *scope_local_lookup(const char *name) {
	return (struct symbol *)hash_table_lookup(curr->h, name);
}

void decl_resolve(struct decl *d) {
	if (!d) return;

	struct symbol *sym = symbol_create(scope_level == 0 ? SYMBOL_GLOBAL : SYMBOL_LOCAL,
																		 d->type, d->name);
	if (scope_local_lookup(d->name) != 0) {
		printf("resolve error: %s is already defined\n", d->name);
	} else {
		scope_bind(d->name, sym);
	}


	expr_resolve(d->value);


	if (d->code) {
		scope_enter();
		param_list_resolve(d->type->params, 0);
		stmt_resolve_func(d->code);
		scope_exit();
	}

	decl_resolve(d->next); // ?
}

void expr_resolve(struct expr *e) {
	if (!e) return;


	expr_resolve(e->left);
	expr_resolve(e->right);
	if (e->kind == EXPR_IDENT) {
		struct symbol *sym = scope_lookup(e->name);
		if (!sym) {
			printf("resolve error: %s is not defined\n", e->name);
		} else {
			printf("%s resolves to ", e->name);
			symbol_print(sym);
		}
	}

	if (e->kind == EXPR_CALL) 
		expr_resolve(e->expr_list);

	expr_resolve(e->next);

}

void stmt_resolve_func(struct stmt *s) {
	if (s->kind == STMT_BLOCK) {
		stmt_resolve(s->body);
	} else {
		stmt_resolve(s);
	}
}

void stmt_resolve(struct stmt *s) {
	if (!s) return;
	
	switch(s->kind) {
		case STMT_DECL:
			decl_resolve(s->decl);
			break;
		case STMT_EXPR:
			expr_resolve(s->expr);
			break;
		case STMT_IF_ELSE:
			expr_resolve(s->expr);
			stmt_resolve(s->body);
			stmt_resolve(s->else_body);
			break;
		case STMT_FOR:
			expr_resolve(s->init_expr);
			expr_resolve(s->expr);
			expr_resolve(s->next_expr);
			stmt_resolve(s->body);
			break;
		case STMT_WHILE:
			break;
		case STMT_PRINT:
			expr_resolve(s->expr);
			break;
		case STMT_RETURN:
			expr_resolve(s->expr);
			break;
		case STMT_BLOCK:
			scope_enter();
			stmt_resolve(s->body);
			scope_exit();
			break;
	}

	stmt_resolve(s->next);

}

void param_list_resolve(struct param_list *p, int idx) {
	if (!p) return;
	struct symbol *sym = scope_local_lookup(p->name);
	if (sym != 0 && sym->kind == SYMBOL_PARAM) {
		printf("resolve error: %s is already defined\n", p->name);
	} else {
		sym = symbol_create_param(idx, p->type, p->name);
		expr_resolve(p->expr);
		p->symbol = sym;
		scope_bind(p->name, sym);
	}

	param_list_resolve(p->next, idx + 1);
}
