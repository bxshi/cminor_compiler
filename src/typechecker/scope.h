#include "hash_table.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "type.h"

struct scope {
	struct hash_table *h; // symbol table
	struct scope *prev;
	struct scope *next; // next element
} scope_t;


void init();
void scope_enter(); // add a new scope into scope list
void scope_exit(); // delete a scope
void scope_bind(const char *name, struct symbol *s); // bind a symbol with a name at a scope
struct symbol *scope_lookup(const char *name); // find symbol in entire scope
struct symbol *scope_local_lookup(const char *name); // find symbol only in local scope

void decl_resolve(struct decl *d);
void expr_resolve(struct expr *e);
void stmt_resolve(struct stmt *s);
void param_list_resolve(struct param_list *p, int idx);
void stmt_resolve_func(struct stmt *s);
