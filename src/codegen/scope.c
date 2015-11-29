#include "scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol.h"

struct scope *root = 0;
struct scope *curr = 0;
int scope_level = 0;

int resolve_error = 0;
int resolve_print = 1;

int typecheck_error = 0;

int returned = 0;

int resolve_result() {
	return resolve_error;
}

int typecheck_result() {
	return typecheck_error;
}

void init(int print_resolve) {
	root = (struct scope *)malloc(sizeof(*root));
	root->h = hash_table_create(0,0);
	root->prev = 0;
	root->next = 0;
	root->type_handler = 0;
	curr = root;
	resolve_print = print_resolve;
}

void scope_enter() {
	scope_level++;
	struct scope *ptr = (struct scope *)malloc(sizeof(*ptr));

	ptr->prev = curr;
	ptr->next = 0;
	ptr->type_handler = 0;
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
	if (curr->type_handler)
		curr->type_handler->local_variables += 1;
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
		resolve_error++;
	} else {
		scope_bind(d->name, sym);
	}
	d->symbol = sym;
	sym->which = hash_table_size(curr->h);


	expr_resolve(d->value);

	// if decl is a function and have code 
	/*
	if (d->type->kind == TYPE_FUNCTION && d->code) {
		if (d->type->subtype->kind != TYPE_VOID) {
			printf("type error: function %s returns void, should return ", d->name);
			type_print(d->type->subtype);
			printf("\n");
			typecheck_error++;
		}
	}
	*/
	

	if (d->code) {
		scope_enter();
		param_list_resolve(d->type->params, 0);
		int old_rtn = returned;
		returned = 0;
		stmt_resolve(d->code);
		if (returned == 0 && d->type->subtype->kind != TYPE_VOID) {
			printf("type error: function %s returns void, should return ", d->name);
			type_print(d->type->subtype);
			printf("\n");
			typecheck_error++;
		}
		returned = old_rtn;
		scope_exit();
	} else { // array
		scope_enter();
		param_list_resolve(d->type->params, 0);
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
			resolve_error++;
		} else {
			if (resolve_print) {
				printf("%s resolves to ", e->name);
				symbol_print(sym);
			}
		}
		e->symbol = sym;
	}

	if (e->kind == EXPR_CALL) {
		struct symbol *sym = scope_lookup(e->left->name);
		if (!sym) {
			printf("resolve error: %s is not defined\n", e->name);
			resolve_error++;
		} else {
			if (resolve_print) {
				printf("%s resolves to ", e->left->name);
				symbol_print(sym);
			}
		}
		e->symbol = sym;
		expr_resolve(e->expr_list);
	}



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
			returned++;
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
	if (p->type == 0) { // array
		expr_resolve(p->expr);
		p->symbol = symbol_create_param(idx, type_create(TYPE_INTEGER, 0, 0), 0);
	} else {
		struct symbol *sym = scope_local_lookup(p->name);
		if (sym != 0 && sym->kind == SYMBOL_PARAM) {
			printf("resolve error: %s is already defined\n", p->name);
			resolve_error++;
		} else {
			sym = symbol_create_param(idx, p->type, p->name);
			expr_resolve(p->expr);
			p->symbol = sym;
			scope_bind(p->name, sym);
		}
	}
	param_list_resolve(p->next, idx + 1);
}

struct type * type_copy(struct type *t) {
	if (!t) return 0;

	struct type *tcopy = (struct type *)malloc(sizeof(*tcopy));

	tcopy->kind = t->kind;
	tcopy->params = t->params;
	tcopy->subtype = t->subtype;

	return tcopy;
}

int type_compare(struct type *a, struct type *b) {
	if (a == b) {
		return 1;
	}
	if (a->kind == b->kind) {
		if (type_compare(a->subtype, b->subtype)) {
			return 1;
		}
	}
	return 0;
}

void type_delete(struct type *t) {
}

const char* expr_print_operator(struct expr *e) {
	if (!e) return "";
	switch(e->kind) {
		case EXPR_ADD: return "+";
		case EXPR_SUB: return "-";
		case EXPR_MUL: return "*";
		case EXPR_DIV: return "/";
		case EXPR_MOD: return "%";
		case EXPR_EXP: return "^";
		case EXPR_INCR: return "++";
		case EXPR_DECR: return "--";
		case EXPR_NOT: return "!";
		case EXPR_EQ: return "==";
		case EXPR_NEQ: return "!=";
		case EXPR_LT: return ">";
		case EXPR_LEQ: return "<=";
		case EXPR_GT: return ">";
		case EXPR_GEQ: return ">=";
		case EXPR_AND: return "&&";
		case EXPR_OR: return "||";
		case EXPR_ASSIGN: return "=";
		case EXPR_SUBSCRIPT: return "[]";
		default: return "";
	}
}

struct type * expr_typecheck(struct expr *e) {
	//TODO: do name resolve first
	if (!e) return type_create(TYPE_VOID, 0, 0);

	struct type *lhs = expr_typecheck(0);
	struct type *rhs = expr_typecheck(0);

	struct expr *tmpe = 0;
	struct param_list *tmppl = 0;
	struct type *tmpt;
	int paramcount = 0;

	switch(e->kind) {
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV:
		case EXPR_MOD:
		case EXPR_EXP:
			lhs = expr_typecheck(e->left);
			rhs = expr_typecheck(e->right);

			//TODO: check if functions, try with its return type
			
			if (lhs->kind == TYPE_FUNCTION) lhs = lhs->subtype;
			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;
			
			if (lhs->kind != TYPE_INTEGER || rhs->kind != TYPE_INTEGER) {
				printf("type error: can not perform ");
				type_print(lhs);
				printf(" %s ", expr_print_operator(e));
				type_print(rhs);
				printf("\n");
				typecheck_error++;
			}


			e->type = type_create(TYPE_INTEGER, 0, 0);
			return e->type;

		case EXPR_LT:
		case EXPR_LEQ:
		case EXPR_GT:
		case EXPR_GEQ:

			lhs = expr_typecheck(e->left);
			rhs = expr_typecheck(e->right);

			if (lhs->kind == TYPE_FUNCTION) lhs = lhs->subtype;
			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;

			if (lhs->kind != TYPE_INTEGER || rhs->kind != TYPE_INTEGER) {
				printf("type error: can not perform ");
				type_print(lhs);
				printf(" %s ", expr_print_operator(e));
				type_print(rhs);
				printf("\n");
				typecheck_error++;
			}

			e->type = type_create(TYPE_BOOLEAN, 0, 0);
			return e->type;

		case EXPR_INCR:
		case EXPR_DECR:

			rhs = expr_typecheck(e->right);

			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;

			if (rhs->kind != TYPE_INTEGER) {
				printf("type error: can not perform ");
				type_print(rhs);
				printf("%s, need to be an integer.\n", expr_print_operator(e));
				typecheck_error++;
			}

			e->type = type_create(TYPE_INTEGER, 0, 0);
			return e->type;

		// ident and literals
		case EXPR_IDENT:
			printf("assign ident %s symbol type to expr\n", e->name);
			e->type = e->symbol->type;
			return e->type;
		case EXPR_BOOLEAN:
			e->type = type_create(TYPE_BOOLEAN, 0, 0);
			return e->type;
		case EXPR_INTEGER:
			e->type = type_create(TYPE_INTEGER, 0, 0);
			return e->type;
		case EXPR_CHAR:
			e->type = type_create(TYPE_CHAR, 0, 0);
			return e->type;
		case EXPR_STRING:
			e->type = type_create(TYPE_STRING, 0, 0);
			return e->type;
		case EXPR_CALL:
			tmpe = e->expr_list;
			tmppl = e->symbol->type->params;
			while(tmpe != 0 && tmppl != 0) {
				lhs = expr_typecheck(tmpe);

				if (!type_compare(lhs, tmppl->type)) {
					printf("type error: %s requires ", e->left->name);
					type_print(tmppl->type);
					printf(" as %dth param, but get ", paramcount + 1);
					type_print(lhs);
					printf("\n");
					typecheck_error++;
				}

				tmpe->type = lhs;

				tmpe = tmpe->next;
				tmppl = tmppl->next;
				paramcount++;
			}

			if ((void *)tmpe != (void *)tmppl) {
				printf("type error: params of %s does not match its prototype\n", e->left->name);
				typecheck_error++;
			}

			e->type = e->symbol->type;
			return e->type;

		case EXPR_NOT:
			rhs = expr_typecheck(e->right);
			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;
			if (rhs->kind != TYPE_BOOLEAN) {
				printf("type error: can not perform ");
				type_print(lhs);
				printf(" %s ", expr_print_operator(e));
				type_print(rhs);
				printf("\n");
				typecheck_error++;
			}

			e->type = type_create(TYPE_BOOLEAN, 0, 0);
			return e->type;

		case EXPR_EQ: // anything but function and array
		case EXPR_NEQ:

			lhs = expr_typecheck(e->left);
			rhs = expr_typecheck(e->right);
			if (lhs->kind == TYPE_FUNCTION) lhs = lhs->subtype;
			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;

			if (!type_compare(lhs, rhs) || lhs->kind == TYPE_FUNCTION || rhs->kind == TYPE_FUNCTION || lhs->kind == TYPE_ARRAY || rhs->kind == TYPE_ARRAY) {
				printf("type error: can not perform ");
				type_print(lhs);
				printf(" %s ", expr_print_operator(e));
				type_print(rhs);
				printf("\n");
				typecheck_error++;
			}

			e->type = type_create(TYPE_BOOLEAN, 0, 0);
			return e-type;


		case EXPR_AND: // only on boolean
		case EXPR_OR:

			lhs = expr_typecheck(e->left);
			rhs = expr_typecheck(e->right);

			if (lhs->kind == TYPE_FUNCTION) lhs= lhs->subtype;
			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;

			if (lhs->kind != TYPE_BOOLEAN) {
				printf("type error: can not do binary operation %s on non-boolean type ", expr_print_operator(e));
				type_print(lhs);
				printf("\n");
				typecheck_error++;
			}

			if (rhs->kind != TYPE_BOOLEAN) {
				printf("type error: can not do binary operation %s on non-boolean type ", expr_print_operator(e));
				type_print(lhs);
				printf("\n");
				typecheck_error++;
			}


			e->type = type_create(TYPE_BOOLEAN, 0, 0);
			return e->type;

		case EXPR_ASSIGN: // anything but function

			lhs = expr_typecheck(e->left);
			rhs = expr_typecheck(e->right);

			if (rhs->kind == TYPE_FUNCTION) rhs = rhs->subtype;

			if (!type_compare(lhs, rhs) || lhs->kind == TYPE_FUNCTION) {
				printf("type error: can not assign ");
				type_print(rhs);
				printf(" to ");
				type_print(lhs);
				printf("\n");
				typecheck_error++;
			}


			e->type = lhs;

			return lhs;

		case EXPR_PARENT:
			if (e->right) {
				e->type = expr_typecheck(e->right);
				return e->type;
			}
			e->type = type_create(TYPE_VOID, 0, 0);
			return e->type;

		case EXPR_CURLY:
			tmpe = e->right;
			tmpt = 0;
			while(tmpe != 0) {


				if (tmpt == 0) {
					tmpt = expr_typecheck(tmpe);
				} else {
					if (!type_compare(tmpt, expr_typecheck(tmpe))) {
						printf("type error : type inconsistent in array elements\n");
						tmpt = expr_typecheck(tmpe);
						typecheck_error++;
					}
				}

				tmpe = tmpe->next;

			}

			e->type = type_create(TYPE_ARRAY, 0, tmpt);
			return e->type;

		case EXPR_SUBSCRIPT:
			rhs = expr_typecheck(e->right);
			if (rhs->kind != TYPE_INTEGER) {
				printf("type error: subscript should be integer but actual is ");
				type_print(rhs);
				printf("\n");
				typecheck_error++;
			}

			e->type = type_create(TYPE_INTEGER, 0, 0);
			return e->type;
		
	}

	expr_typecheck(e->next);
}

void stmt_typecheck(struct stmt *s, struct type *rtn_type, const char *func_name) {
	if (!s) return;
	struct type *t;
	switch(s->kind) {
		case STMT_DECL:
			decl_typecheck(s->decl);
			break;
		case STMT_EXPR:
			expr_typecheck(s->expr);
			break;
		case STMT_IF_ELSE:
			t = expr_typecheck(s->expr);
			if (t->kind != TYPE_BOOLEAN) {
				printf("type error: if condition is ");
				type_print(t);
				printf(" not boolean\n");
				typecheck_error++;
			}
			stmt_typecheck(s->body, rtn_type, func_name);
			stmt_typecheck(s->else_body, rtn_type, func_name);
			break;
		case STMT_FOR:
			expr_typecheck(s->init_expr);
			if (expr_typecheck(s->expr)->kind != TYPE_BOOLEAN) {
				printf("type error: condition of for-loop is ");
				type_print(expr_typecheck(s->expr));
				printf(" not boolean\n");
				typecheck_error++;
			}
			expr_typecheck(s->next_expr);
			break;
		case STMT_WHILE:
			break;
		case STMT_PRINT: // should all exprs after print has non void return?
			expr_typecheck(s->expr);
			break;
		case STMT_RETURN: // should match function prototype
			t = expr_typecheck(s->expr);
			if (!type_compare(t, rtn_type)) {
				printf("type error: function %s returns ", func_name);
				type_print(t);
				printf(", should return ");
				type_print(rtn_type);
				printf("\n");
				typecheck_error++;
			}
			break;
		case STMT_BLOCK: 
			stmt_typecheck(s->body, rtn_type, func_name);
			break;
	}
	stmt_typecheck(s->next, rtn_type, func_name);
}

int expr_constant(struct expr *e) {
	if (!e) return 1;
	switch(e->kind) {
		case EXPR_BOOLEAN:
		case EXPR_INTEGER:
		case EXPR_CHAR:
		case EXPR_STRING:
			return 1;
		default: return 0;
	}
}

// only be called during declaration
// check parameter declarations of functions and the param of array
void param_list_typecheck(struct param_list *pl, int isGlobal, int isParam) {
	if (!pl) return;

	struct type *type_exp = expr_typecheck(pl->expr);
	if (pl->type == 0) pl->type = type_create(TYPE_INTEGER, 0, 0);
	if (!isParam) { // this is not part of paramlist of a function
		if (!type_compare(pl->type, type_exp)) {
			printf("type error: requires param with type ");
			type_print(pl->type);
			printf(", actual is ");
			type_print(type_exp);
			printf("\n");
			typecheck_error++;
		}
	}

	if (isGlobal && !expr_constant(pl->expr)) {
		printf("type error: only constant values can be assigned to array's param\n");
		typecheck_error++;
	}

	param_list_typecheck(pl->next, isGlobal, isParam);
	
}

void decl_typecheck(struct decl *d) {
	if (!d) return;

	if (!d->symbol) printf("decl has no symbol\n");
	if (!d->symbol->type) printf("decl has no symbol type\n");

	// how to determine this?
	if (d->symbol->kind == SYMBOL_GLOBAL && !expr_constant(d->value)) {
		printf("type error: only constant values can be assigned to global value %s\n", d->symbol->name);
		typecheck_error++;
	}

	param_list_typecheck(d->type->params, d->symbol->kind == SYMBOL_GLOBAL, d->type->kind == TYPE_FUNCTION);

	struct type *t = expr_typecheck(d->value);
	if (d->type->kind != TYPE_FUNCTION && !type_compare(d->type, t) && t->kind != TYPE_VOID) {
		printf("type error: can not assign ");
		type_print(expr_typecheck(d->value));
		printf(" to newly decleared varialbe %s (", d->symbol->name);
		type_print(d->type);
		printf(")\n");
		typecheck_error++;
	}

	if (d->code) stmt_typecheck(d->code, d->type->subtype, d->name);

	decl_typecheck(d->next);

}
