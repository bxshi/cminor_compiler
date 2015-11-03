#include "stmt.h"

struct stmt *stmt_create(stmt_kind_t kind, struct decl *d, struct expr *init_expr, struct expr *e, struct expr *next_expr, struct stmt *body, struct stmt *else_body) {
	struct stmt *s = (struct stmt*)malloc(sizeof(*s));
	s->kind = kind;
	s->decl = d;
	s->init_expr = init_expr;
	s->expr = e;
	s->next_expr = next_expr;
	s->body = body;
	s->else_body = else_body;
	s->nline = 1;
	s->eline = 1;
	return s;
}

void stmt_print(struct stmt *s, int indent) {
	if (!s) return;

	switch(s->kind) {
		case STMT_DECL:
			decl_print(s->decl, indent); // decl has its own ;\n. And we do not pass indent into it since we already solved it by stmt
			break;
		case STMT_EXPR:
			if(indent > 0) printf("%*c", indent * 2, ' ');
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_IF_ELSE:

			if (indent > 0 && s->nline) printf("%*c", indent * 2, ' '); // starting indent
			printf("if (");
			expr_print(s->expr);
			printf(")");

			// if statement is not a block
			if (s->body && s->body->kind != STMT_BLOCK) { // if (expr) stmt
				// start a new line and print the statement
				printf("\n");
				stmt_print(s->body, indent + 1); // will start a new line

				if (s->else_body) { // if has else statement
					printf("%*c", indent * 2, ' ');
					printf("else");
					if (s->else_body->kind == STMT_IF_ELSE || s->else_body->kind == STMT_BLOCK) {
						printf(" ");
						s->else_body->nline = 0;
						stmt_print(s->else_body, indent);
					} else {
						printf("\n");
						stmt_print(s->else_body, indent + 1);
					}
				}
			} else { // statement is a block
				printf(" ");
				s->body->nline = 0; // do not start a new line
				if (s->else_body) s->body->eline = 0;
				stmt_print(s->body, indent); // new code block's brace has the same indent as prev level

				if (s->else_body) { // if has else statement
					printf(" else");
					if (s->else_body->kind == STMT_IF_ELSE || s->else_body->kind == STMT_BLOCK) {
						printf(" ");
						s->else_body->nline = 0;
						stmt_print(s->else_body, indent);
					} else {
						printf("\n");
						stmt_print(s->else_body, indent + 1);
					}
				}
			}
			break;
/*
			if (s->body && s->body->kind != STMT_BLOCK) stmt_print(s->body, 0);
			else stmt_print(s->body, -((indent > 0 ? indent : -indent)));
			if (s->else_body) {
				// prev if statement is a block
				if (s->body->kind == STMT_BLOCK) { 
					printf(" else ");
				} else {
					if(indent != 0) printf("%*c", (indent > 0 ? indent : -indent) * 2, ' ');
					printf("else ");
				}
				if (s->else_body->kind == STMT_BLOCK) stmt_print(s->else_body, -((indent > 0 ? indent : -indent)));
				else stmt_print(s->else_body, 0);
			}
			printf("\n");
			break;
			*/
		case STMT_FOR:
			if(indent > 0) printf("%*c", indent * 2, ' ');
			printf("for (");
			expr_print(s->init_expr);
			printf(";");
			expr_print(s->expr);
			printf(";");
			expr_print(s->next_expr);
			printf(")\n");
			if (s->body && s->body->kind == STMT_BLOCK)
				stmt_print(s->body, indent);
			else stmt_print(s->body, indent + 1);
			break;
		case STMT_WHILE:
		  // TODO: reserve key word
			break;
		case STMT_PRINT:
			if(indent > 0) printf("%*c", indent * 2, ' ');
			printf("print ");
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_RETURN:
			if(indent > 0) printf("%*c", indent * 2, ' ');
			printf("return");
			if (s->expr) printf(" ");
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_BLOCK:
			if (!s->nline) { // do not start a new line
				printf("{\n");
				stmt_print(s->body, indent + 1);
			} else {
				if(indent > 0) printf("%*c", indent * 2, ' ');
				printf("{\n");
				stmt_print(s->body, indent + 1);
			}

			if (indent > 0) printf("%*c", (indent) * 2, ' ');
			printf("}");
			if (s->eline) printf("\n");
	}

	stmt_print(s->next, indent);

}
