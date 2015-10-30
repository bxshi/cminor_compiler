#ifndef EXPR_H
#define EXPR_H
#define NULL 0

#include "symbol.h"

typedef enum {
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_IDENT,
	EXPR_BOOLEAN,
	EXPR_INTEGER,
	EXPR_CHAR,
	EXPR_STRING,
	EXPR_CALL,
	EXPR_INCR,
	EXPR_DECR,
	EXPR_NOT,
	EXPR_EXP,
	EXPR_MOD,
	EXPR_EQ,
	EXPR_NEQ,
	EXPR_LT,
	EXPR_LEQ,
	EXPR_GT,
	EXPR_GEQ,
	EXPR_AND,
	EXPR_OR,
	EXPR_ASSIGN
	/* many more types to add here */
} expr_t;

struct expr {
	/* used by all expr types */
	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by leaf expr types */
	const char *name;
	struct symbol *symbol;
	int literal_value;
	const char * string_literal;
};

struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right );

struct expr * expr_create_name( const char *n );
struct expr * expr_create_boolean_literal( int c );
struct expr * expr_create_integer_literal( int c );
struct expr * expr_create_character_literal( int c );
struct expr * expr_create_string_literal( const char *str );

void expr_print( struct expr *e );

#endif
