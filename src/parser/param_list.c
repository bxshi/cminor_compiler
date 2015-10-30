#include "param_list.h"

struct param_list *param_list_create(char *name, struct type *type, struct expr *expr, struct param_list *next) {
	struct param_list *pl = (struct param_list*)malloc(sizeof(*pl));

	pl->name = name;
	pl->type = type;
	pl->next = next;
	return pl;
}