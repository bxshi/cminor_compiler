#include <stdio.h>

#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "type.h"
#include "symbol.h"
#include "register.h"

void decl_codegen(struct decl *d, FILE *file);

void stmt_codegen(struct stmt *s, FILE *file);

void expr_codegen(struct expr *e, FILE *file);

char *symbol_code(struct symbol *s);

