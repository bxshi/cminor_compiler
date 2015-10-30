#include "expr.h"

struct expr * expr_create(expr_t kind, struct expr *left, struct expr *right) {
	struct expr *e = (struct expr*)malloc(sizeof(*e));
	e->kind = kind;
	e->left = left;
	e->right = right;
	return e;
}

struct expr * expr_create_function_call(const char *ident, struct stmt *expr_list) {
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

struct expr * expr_create_character_literal(int c) {
	struct expr *e = expr_create(EXPR_CHAR, nullptr, nullptr);
	e->literal_value = c;
	return e;
}

struct expr * expr_create_string_literal(const char *str) {
	struct expr *e = expr_create(EXPR_STRING, nullptr, nullptr);
	e->string_literal = str;
	return e;
}
