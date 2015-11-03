#include "param_list.h"

struct param_list *param_list_create(char *name, struct type *type, struct expr *expr, struct param_list *next) {
	struct param_list *pl = (struct param_list*)malloc(sizeof(*pl));

	pl->name = name;
	pl->type = type;
	pl->expr = expr;
	pl->next = next;
	return pl;
}

void param_list_print(struct param_list *a) {
	if (!a) return;

	if (a->name) {
		printf("%s : ", a->name);
	}

	type_print(a->type);

	if (a->expr) {
		expr_print(a->expr);
	}

	if (a->next) {
		printf(", ");
		param_list_print(a->next);
	}
	
}
