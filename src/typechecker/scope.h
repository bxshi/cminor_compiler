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


void init(int print_resolve);
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

int resolve_result();

struct type * type_copy(struct type *t);
int type_compare(struct type *a, struct type *b);
void type_delete(struct type *t);

struct type * expr_typecheck(struct expr *e);
void stmt_typecheck(struct stmt *s, struct type *rtn_type, const char *func_name);
void decl_typecheck(struct decl *d);

int typecheck_result();
