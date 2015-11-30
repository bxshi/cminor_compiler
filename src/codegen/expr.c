#include "expr.h"
#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct expr * expr_create(expr_t kind, struct expr *left, struct expr *right) {
	struct expr *e = (struct expr*)malloc(sizeof(*e));
	e->kind = kind;
	e->left = left;
	e->right = right;
	e->symbol = 0;
	e->next = 0;
	e->reg = -1;
	e->type = 0;
	return e;
}

struct expr * expr_create_parent(struct expr * subexpr) {
	struct expr *e = expr_create(EXPR_PARENT, nullptr, subexpr);
	return e;
}

struct expr * expr_create_curly(struct expr * subexpr) {
	struct expr *e = expr_create(EXPR_CURLY, nullptr, subexpr);
	return e;
}

struct expr * expr_create_function_call(const char *ident, struct expr *expr_list) {
	struct expr *e = expr_create(EXPR_CALL, expr_create_name(ident), nullptr);
	e->expr_list = expr_list;
	return e;
}

struct expr * expr_create_name (const char *n) {
	struct expr *e = expr_create(EXPR_IDENT, nullptr, nullptr);
	e->name = n;
	return e;
}

struct expr * expr_create_boolean_literal(int c) {
	struct expr *e = expr_create(EXPR_BOOLEAN, nullptr, nullptr);
	e->literal_value = c;
	return e;
}

struct expr * expr_create_integer_literal(int c) {
	struct expr *e = expr_create(EXPR_INTEGER, nullptr, nullptr);
	e->literal_value = c;
	return e;
}

struct expr * expr_create_character_literal(char *c) {
	struct expr *e = expr_create(EXPR_CHAR, nullptr, nullptr);

	if (strlen(c) > 3) {
		switch(c[2]) {
			case 'a': e->literal_value = '\a'; break;
			case 'b': e->literal_value = '\b'; break;
			case 'f': e->literal_value = '\f'; break;
			case 'n': e->literal_value = '\n'; break;
			case 'r': e->literal_value = '\r'; break;
			case 't': e->literal_value = '\t'; break;
			case 'v': e->literal_value = '\v'; break;
			case '\\': e->literal_value = '\\'; break;
			case '\'': e->literal_value = '\''; break;
			case '\"': e->literal_value = '\"'; break;
			case '?': e->literal_value = '\?'; break;
			default: e->literal_value = c[2]; break;
		}
	} else {
		e->literal_value = c[1];
	}

	return e;
}

struct expr * expr_create_string_literal(const char *str) {
	struct expr *e = expr_create(EXPR_STRING, nullptr, nullptr);
	e->string_literal = str;
	return e;
}

struct expr * expr_create_subscript(const char *ident, struct expr *expr) {
	struct expr *e = expr_create(EXPR_SUBSCRIPT, expr_create_name(ident), expr);
	return e;
}

void expr_print(struct expr *e) {
	if (!e) return;

	switch(e->kind) {
		case EXPR_ADD:
			expr_print(e->left);
			printf("+");
			expr_print(e->right);
			break;
		case EXPR_SUB:
			expr_print(e->left);
			if (e->left) printf("-");
			else printf("-");
			expr_print(e->right);
			break;
		case EXPR_MUL:
			expr_print(e->left);
			printf("*");
			expr_print(e->right);
			break;
		case EXPR_DIV:
			expr_print(e->left);
			printf("/");
			expr_print(e->right);
			break;
		case EXPR_IDENT:
			printf("%s",e->name);
			break;
		case EXPR_BOOLEAN:
			printf("%s", e->literal_value == 1 ? "true" : "false");
			break;
		case EXPR_INTEGER:
			printf("%d", e->literal_value);
			break;
		case EXPR_CHAR:
			printf("'%c'", e->literal_value);
			break;
		case EXPR_STRING:
			printf("%s", (e->string_literal));
			break;
		case EXPR_CALL:
			expr_print(e->left);
			printf("(");
			expr_print(e->expr_list);
			printf(")");
			break;
		case EXPR_INCR:
			expr_print(e->right);
			printf("++");
			break;
		case EXPR_DECR:
			expr_print(e->right);
			printf("--");
			break;
		case EXPR_NOT:
			printf("!");
			expr_print(e->right);
			break;
		case EXPR_EXP:
			expr_print(e->left);
			printf("^");
			expr_print(e->right);
			break;
		case EXPR_MOD:
			expr_print(e->left);
			printf("%%");
			expr_print(e->right);
			break;
		case EXPR_EQ:
			expr_print(e->left);
			printf("==");
			expr_print(e->right);
			break;
		case EXPR_NEQ:
			expr_print(e->left);
			printf("!=");
			expr_print(e->right);
			break;
		case EXPR_LT:
			expr_print(e->left);
			printf("<");
			expr_print(e->right);
			break;
		case EXPR_LEQ:
			expr_print(e->left);
			printf("<=");
			expr_print(e->right);
			break;
		case EXPR_GT:
			expr_print(e->left);
			printf(">");
			expr_print(e->right);
			break;
		case EXPR_GEQ:
			expr_print(e->left);
			printf(">=");
			expr_print(e->right);
			break;
		case EXPR_AND:
			expr_print(e->left);
			printf("&&");
			expr_print(e->right);
			break;
		case EXPR_OR:
			expr_print(e->left);
			printf("||");
			expr_print(e->right);
			break;
		case EXPR_ASSIGN:
			expr_print(e->left);
			printf(" = ");
			expr_print(e->right);
			break;
		case EXPR_PARENT:
			printf("(");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_CURLY:
			printf("{");
			expr_print(e->right);
			printf("}");
			break;
		case EXPR_SUBSCRIPT:
			expr_print(e->left);
			printf("[");
			expr_print(e->right);
			printf("]");
			break;
		default:
			printf("undefined expr!\n");
	}
	if (e->next) printf(", ");
	expr_print(e->next);
}
