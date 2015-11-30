#include <stdio.h>
#include "codegen.h"
#include "register.h"
#include <string.h>

int label_counter = 0;

const char* arg_regs[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

char func_end[1024];

char* codegen_label()
{
	char *lbl = (char *)malloc(sizeof(*lbl) * 128);
	sprintf(lbl, "_LABEL_%d", ++label_counter);
	return lbl;
}

char* str_label()
{
	char *lbl = (char *)malloc(sizeof(*lbl) * 128);
	sprintf(lbl, "_STR_LABEL_%d", ++label_counter);
	return lbl;
}

void caller_prepostamble(char *func_name, FILE *file) {
	fprintf(file, "PUSHQ %%r10\n");
	fprintf(file, "PUSHQ %%r11\n");

	if (strcmp(func_name, "") != 0)
		fprintf(file, "CALL %s\n", func_name);

	fprintf(file, "POPQ %%r11\n");
	fprintf(file, "POPQ %%r10\n");
}

char * resolve_print_expr_type(struct type* t) {
				switch(t->kind) {
					case TYPE_BOOLEAN:
						return "print_boolean";
					case TYPE_CHAR:
						return "print_character";
					case TYPE_STRING:
						return "print_string";
					case TYPE_INTEGER:
						return "print_integer";
					case TYPE_FUNCTION:
						return resolve_print_expr_type(t->subtype);
					default:
						return "";
				}
			}

void decl_codegen(struct decl *d, FILE *file)
{
  if (!d) return;

  struct param_list *pl;
  int pl_count = 0;

	//TODO: do not generate code for function declarations

  // global variables
  switch(d->type->kind) {
  case TYPE_BOOLEAN:
  case TYPE_CHAR:
  case TYPE_INTEGER:
    if (d->symbol->kind == SYMBOL_GLOBAL) {
      fprintf(file, ".data\n");
      fprintf(file, "%s:\n", d->name);
      fprintf(file, ".quad %d\n", d->value != 0 ? d->value->literal_value : 0);
    } else {
      if (d->value) { // evalue rhs, put into stack
        expr_codegen(d->value, file);
        //calculate offset of %rbp -- check if this is correct
				fprintf(file, "MOVQ %s, %s\n", register_name(d->value->reg), symbol_code(d->symbol));

        register_free(d->value->reg);
				d->value->reg = -1;
      }
    }
    break;
  case TYPE_STRING:
    if (d->symbol->kind == SYMBOL_GLOBAL) {
      fprintf(file, ".data\n");
      fprintf(file, "%s:\n", d->name);
      fprintf(file, ".string \"%s\"\n", d->value != 0 ? d->value->string_literal : "");
    } else { // local variable
      fprintf(file, ".data\n");
      fprintf(file, "%s:\n", d->name);
      fprintf(file, ".string \"%s\"\n", d->value != 0 ? d->value->string_literal : "");

			fprintf(file, ".text\n");
      //allocate a register
			int reg = register_alloc();
			fprintf(file, "LEA %s, %s\n", d->name, register_name(reg));
      // LEA d->name, %E ; MOV %E, -(%rbp)
			fprintf(file, "MOV %s, %s", register_name(reg), symbol_code(d->symbol));
      //deallocate register
			register_free(reg);
			reg = -1;
    }
    break;

  case TYPE_FUNCTION:
		if (!d->code) break; // if declaration just skip it
    fprintf(file, ".text\n");
		if (d->symbol->kind == SYMBOL_GLOBAL) fprintf(file, ".global %s\n", d->name);
		fprintf(file, "%s:\n", d->name);

    // preamble
    fprintf(file, "PUSHQ %%rbp\n");
    fprintf(file, "MOVQ %%rsp, %%rbp\n");

    // count number of arguments
    pl = d->type->params;
    pl_count = 0;
    while(pl) {
      pl = pl->next;
      pl_count++;
    }

    // push arguments
    if (pl_count >= 1) fprintf(file, "PUSHQ %%rdi\n");
    if (pl_count >= 2) fprintf(file, "PUSHQ %%rsi\n");
    if (pl_count >= 3) fprintf(file, "PUSHQ %%rdx\n");
    if (pl_count >= 4) fprintf(file, "PUSHQ %%rcx\n");
    if (pl_count >= 5) fprintf(file, "PUSHQ %%r8\n");
    if (pl_count == 6) fprintf(file, "PUSHQ %%r9\n");

    // allocate local variables
		fprintf(file, "# function %s has %d local variables\n", d->name, d->type->local_variables);
    fprintf(file, "SUBQ $%d, %%rsp %s\n", d->type->local_variables * 8, "# allocate local variables");

    // save callee saved registers
    fprintf(file, "PUSHQ %%rbx\n");
    fprintf(file, "PUSHQ %%r12\n");
    fprintf(file, "PUSHQ %%r13\n");
    fprintf(file, "PUSHQ %%r14\n");
    fprintf(file, "PUSHQ %%r15\n"); // r10 and r11 are stored by caller

		sprintf(func_end, "_%s_POST", d->name);

		//after we save all registers, wipe out old register info
		register_stepin();
		
    // function body
    stmt_codegen(d->code, file);


		fprintf(file, "%s:\n", func_end); // postamble label

    // pop callee saved registers
    fprintf(file, "POPQ %%r15\n");
    fprintf(file, "POPQ %%r14\n");
    fprintf(file, "POPQ %%r13\n");
    fprintf(file, "POPQ %%r12\n");
    fprintf(file, "POPQ %%rbx\n");

    // postamble restore stack pointer
    fprintf(file, "MOVQ %%rbp, %%rsp\n");
    fprintf(file, "POPQ %%rbp\n");
    fprintf(file, "RET\n");
		fprintf(file, "# function %s end \n\n", d->name);

		register_stepout(); // restore register info

    break;
  case TYPE_ARRAY:
    printf("codegen error: does not support arry\n");
    break;
  case TYPE_VOID:
    break;
  }

	decl_codegen(d->next, file);

}

void stmt_codegen(struct stmt *s, FILE *file)
{

  if (!s) return;

	char *if_label;
	char *else_label;
	char *end_label;

	char *eval_label;
	char *body_label;
	char *next_label;

	struct expr *curr_expr;

  switch(s->kind) {
  case STMT_DECL:
    decl_codegen(s->decl, file);
    break;
  case STMT_EXPR:
		expr_codegen(s->expr, file);
		register_free(s->expr->reg);
		s->expr->reg = -1;
    break;
  case STMT_IF_ELSE:
		expr_codegen(s->expr, file);
		fprintf(file, "CMPQ $1, %s\n", register_name(s->expr->reg));
		register_free(s->expr->reg);
		s->expr->reg = -1;

		if_label = codegen_label();
		else_label = codegen_label();
		end_label = codegen_label();

		fprintf(file, "JE %s %s\n", if_label, "# jump to if body");
		fprintf(file, "%s:\n", else_label);
		stmt_codegen(s->else_body, file);
		fprintf(file, "JMP %s %s\n", end_label, "# end else body");
		fprintf(file, "%s:\n", if_label);
		stmt_codegen(s->body, file);
		fprintf(file, "%s:\n", end_label);

    break;
  case STMT_FOR:
		if (s->init_expr) {
			expr_codegen(s->init_expr, file); // init expression
			register_free(s->init_expr->reg);
			s->init_expr->reg = -1;
		}

		eval_label = codegen_label();
		body_label = codegen_label();
		next_label = codegen_label();
		end_label = codegen_label();

		fprintf(file, "%s:\n", eval_label);

		if (s->expr) {
			expr_codegen(s->expr, file);
			fprintf(file, "CMPQ $1, %s\n", register_name(s->expr->reg));
			register_free(s->expr->reg);
			s->expr->reg = -1;
			fprintf(file, "JE %s\n", body_label);
			fprintf(file, "JMP %s\n", end_label);
			fprintf(file, "%s:\n", body_label);
			stmt_codegen(s->body, file);
			fprintf(file, "\n");
		} else {
			fprintf(file, "JMP %s #infinite loop\n", body_label);
			fprintf(file, "%s:\n", body_label);
			stmt_codegen(s->body, file);
			fprintf(file, "\n");
		}


		if (s->next_expr){
			expr_codegen(s->next_expr, file);
			register_free(s->next_expr->reg);
			s->next_expr->reg = -1;
		}

		fprintf(file, "JMP %s\n", eval_label);
		fprintf(file, "%s:\n",end_label);

    break;
  case STMT_WHILE:
    break;
  case STMT_PRINT:

		curr_expr = s->expr;


		// resolve each parameters in print statement
		while(curr_expr) {

			if (curr_expr->type->kind != TYPE_VOID && curr_expr->type->kind != TYPE_ARRAY) {
				fprintf(file, "#before expr eval\n");
				expr_codegen(curr_expr, file); // evaluate expr
				fprintf(file, "#after expr eval\n");
				// print expr
				if (curr_expr->type->kind == TYPE_STRING){
					fprintf(file, "MOVQ $%s, %s\n", curr_expr->name, arg_regs[0]);
				} else {
					fprintf(file, "MOVQ %s, %s\n", register_name(curr_expr->reg), arg_regs[0]);
				}
				register_free(curr_expr->reg);
	
				caller_prepostamble(resolve_print_expr_type(curr_expr->type), file);
			}


			curr_expr = curr_expr->next; // move to next
		}

    break;
  case STMT_RETURN:
		expr_codegen(s->expr, file);
		fprintf(file, "MOVQ %s, %%rax\n", register_name(s->expr->reg));
		fprintf(file, "JMP %s %s\n", func_end, "# jump to the end of this function");
		register_free(s->expr->reg);
		s->expr->reg = -1;
    break;
  case STMT_BLOCK:
    stmt_codegen(s->body, file);
  }

	stmt_codegen(s->next, file);

}

void expr_codegen(struct expr *e, FILE *file)
{

  if (!e) return;

  char *next_label;
  char *branch_label;
	struct expr *expr_list;
	int argcount = 0;

	char *set_to_false;

  switch(e->kind) {

  case EXPR_ADD: // tested
		expr_codegen(e->left, file);
		expr_codegen(e->right, file);

		if (e->left) {
		fprintf(file, "ADDQ %s, %s\n", register_name(e->left->reg), register_name(e->right->reg));

		register_free(e->left->reg);
		e->left->reg = -1;

		e->reg = register_alloc();
		fprintf(file, "MOVQ %s, %s\n", register_name(e->right->reg), register_name(e->reg));

		register_free(e->right->reg);
		e->right->reg = -1;
		} else {
			e->reg = register_alloc();

			fprintf(file, "MOVQ %s, %s", register_name(e->right->reg), register_name(e->reg));

			register_free(e->right->reg);
			e->right->reg = -1;
		}

		break;
  case EXPR_SUB: // tested
		expr_codegen(e->left, file);
		expr_codegen(e->right, file);

		if (e->left) {
	
			fprintf(file, "SUBQ %s, %s\n", register_name(e->right->reg), register_name(e->left->reg));
	
			register_free(e->right->reg);
			e->right->reg = -1;
	
			e->reg = register_alloc();
	
			fprintf(file, "MOVQ %s, %s\n", register_name(e->left->reg), register_name(e->reg));
	
			register_free(e->left->reg);
			e->left->reg = -1;
		} else {
			e->reg = register_alloc();

			fprintf(file, "MOVQ %s, %s\n", register_name(e->right->reg), register_name(e->reg));

			register_free(e->right->reg);
			e->right->reg = -1;

			fprintf(file, "NEG %s %s\n", register_name(e->reg), "# change sign");
		}

		break;
  case EXPR_MUL: // tested
		expr_codegen(e->left, file);
		expr_codegen(e->right, file);
		fprintf(file, "MOV %s, %%rax\n", register_name(e->left->reg));
		register_free(e->left->reg);
		e->left->reg = -1;
		fprintf(file, "IMUL %s\n", register_name(e->right->reg));
		register_free(e->right->reg);
		e->right->reg = -1;
		e->reg = register_alloc();
		fprintf(file, "MOVQ %%rax, %s\n", register_name(e->reg));
		break;
  case EXPR_DIV: // tested
		expr_codegen(e->left, file);
		expr_codegen(e->right, file);

		fprintf(file, "MOV %s, %%rax\n", register_name(e->left->reg));

		register_free(e->left->reg);
		e->left->reg = -1;

		fprintf(file, "CQO\n");

		fprintf(file, "IDIVQ %s\n", register_name(e->right->reg));
		register_free(e->right->reg);
		e->right->reg = -1;
		e->reg = register_alloc();
		fprintf(file, "MOVQ %%rax, %s\n", register_name(e->reg));
		break;

  case EXPR_IDENT: // tested
    e->reg = register_alloc();
    fprintf(file, "MOVQ %s, %s\n", symbol_code(e->symbol), register_name(e->reg));
    break;

  // boolean/integer/char are stored in literal_value
  case EXPR_CHAR:
  case EXPR_BOOLEAN:
  case EXPR_INTEGER:
    e->reg = register_alloc();
    fprintf(file, "MOVQ $%d, %s %s\n", e->literal_value, register_name(e->reg), "# declear instant char/boolean/integer");
    break;

  case EXPR_STRING:

		if (e->name == 0){
			fprintf(file, ".data\n");
			e->name = str_label();
			fprintf(file, "%s:\n", e->name);
			fprintf(file, ".string %s\n", e->string_literal);
			fprintf(file, ".text\n");
		}
		break;

  case EXPR_CALL:
		// call function
		
		// push all parameters into registers
		
		expr_list = e->expr_list;
		argcount = 0;

		while(expr_list && argcount < 6) {
			expr_codegen(expr_list, file);
			if (expr_list->kind == EXPR_STRING) {
				fprintf(file, "MOVQ %s, %s %s %d\n", expr_list->name, arg_regs[argcount++], "#push arg", argcount);
			} else {
				fprintf(file, "MOVQ %s, %s %s %d\n", register_name(expr_list->reg), arg_regs[argcount++], "#push arg", argcount);
			}
			register_free(expr_list->reg);
			expr_list = expr_list->next;
		}

		fprintf(file, "PUSHQ %%r10\n");
		fprintf(file, "PUSHQ %%r11\n");

		fprintf(file, "CALL %s\n", e->left->name);

		fprintf(file, "POPQ %%r11\n");
		fprintf(file, "POPQ %%r10\n");

		e->reg = register_alloc();
		fprintf(file, "MOVQ %%rax, %s\n", register_name(e->reg));

		break;

	//TODO: store result in e->reg
  case EXPR_INCR:
    expr_codegen(e->right, file);

    //e->reg = register_alloc();
		//fprintf(file, "MOVQ %s, %s %s\n", register_name(e->right->reg), register_name(e->reg), "# move value to expr before ++");

    fprintf(file, "INCQ %s %s\n", register_name(e->right->reg), "# increment");

		if (e->right->kind == EXPR_IDENT) { // update value to the variable data
			fprintf(file, "MOVQ %s, %s %s\n", register_name(e->right->reg), symbol_code(e->right->symbol), "# update value of variable");
		}

    register_free(e->right->reg);
		e->right->reg = -1;

    break;

	//TODO: store result in e->reg
  case EXPR_DECR:
    expr_codegen(e->right, file);

    //e->reg = register_alloc();
		//fprintf(file, "MOVQ %s, %s %s\n", register_name(e->right->reg), register_name(e->reg), "# move value to expr before --");

    fprintf(file, "DECQ %s %s\n", register_name(e->right->reg), "# increment");

		if (e->right->kind == EXPR_IDENT) { // update value to the variable data
			fprintf(file, "MOVQ %s, %s %s\n", register_name(e->right->reg), symbol_code(e->right->symbol), "# update value of variable");
		}

    register_free(e->right->reg);
		e->right->reg = -1;
    break;

  case EXPR_NOT:
    expr_codegen(e->right,file);
    fprintf(file, "MOVQ %s, %%rax %s\n", register_name(e->right->reg), "# move for logical not operation");
    register_free(e->right->reg);
		e->right->reg = -1;
    e->reg = register_alloc();

		set_to_false = codegen_label();
		next_label = codegen_label();

    fprintf(file, "CMPQ $1, %%rax %s\n", "# compare if is true");
		fprintf(file, "JE %s\n", set_to_false);

		fprintf(file, "MOVQ $1, %%rax\n");
		fprintf(file, "JMP %s\n", next_label);

		fprintf(file, "%s:\n", set_to_false);
		fprintf(file, "MOVQ $0, %%rax\n");

		fprintf(file, "%s:\n", next_label);
    fprintf(file, "MOVQ %%rax, %s %s\n", register_name(e->reg), "# move back to expr");
    break;

  case EXPR_EXP:
		// generate two operators
		expr_codegen(e->left, file);
		expr_codegen(e->right, file);

		fprintf(file, "MOVQ %s, %s\n", register_name(e->left->reg), arg_regs[0]);
		fprintf(file, "MOVQ %s, %s\n", register_name(e->right->reg), arg_regs[1]);

		register_free(e->left->reg);
		e->left->reg = -1;
		register_free(e->right->reg);
		e->right->reg = -1;

		fprintf(file, "PUSHQ %%r10\n");
		fprintf(file, "PUSHQ %%r11\n");

		fprintf(file, "CALL integer_power\n");

		fprintf(file, "POPQ %%r11\n");
		fprintf(file, "POPQ %%r10\n");

		e->reg = register_alloc();
		fprintf(file, "MOVQ %%rax, %s\n", register_name(e->reg));

    break;
  case EXPR_MOD:
    expr_codegen(e->left, file);
    expr_codegen(e->right, file);

    fprintf(file, "MOVQ %s, %%rax %s\n", register_name(e->left->reg), "# move dividend");
    register_free(e->left->reg);
		e->left->reg = -1;

		fprintf(file, "CQO\n");

    fprintf(file, "IDIVQ %s %s\n", register_name(e->right->reg), "# divide divisor");
    register_free(e->right->reg);
		e->right->reg = -1;

    e->reg = register_alloc();
    fprintf(file, "MOVQ %%rdx, %s %s\n", register_name(e->reg), "# move mod result back to expr");
    break;

  case EXPR_EQ:
  case EXPR_NEQ:
  case EXPR_LT:
  case EXPR_LEQ:
  case EXPR_GT:
  case EXPR_GEQ:

    expr_codegen(e->left, file);
    expr_codegen(e->right, file);

		
    fprintf(file, "CMPQ %s, %s %s\n", register_name(e->right->reg), register_name(e->left->reg), "# compare equal");

    register_free(e->left->reg);
		e->left->reg = -1;
    register_free(e->right->reg);
		e->right->reg = -1;

    next_label = codegen_label();
    branch_label = codegen_label();

    e->reg = register_alloc();

    if (e->kind == EXPR_EQ) {
      fprintf(file, "JE %s %s\n", branch_label, "# satisfy ==");
    } else if (e->kind == EXPR_NEQ) {
      fprintf(file, "JNE %s %s\n", branch_label, "# satisfy !=");
    } else if (e->kind == EXPR_LT) {
      fprintf(file, "JL %s %s\n", branch_label, "# satisfy <");
    } else if (e->kind == EXPR_LEQ) {
      fprintf(file, "JLE %s %s\n", branch_label, "# satisfy <=");
    } else if (e->kind == EXPR_GT) {
      fprintf(file, "JG %s %s\n", branch_label, "# satisfy >");
    } else if (e->kind == EXPR_GEQ) {
      fprintf(file, "JGE %s %s\n", branch_label, "# satisfy >=");
    }
    fprintf(file, "MOVQ $0, %s %s\n", register_name(e->reg), "# comparison result set to false");
    fprintf(file, "JMP %s %s\n", next_label, "# end comparison");
    fprintf(file, "%s:\n", branch_label);
    fprintf(file, "MOVQ $1, %s %s\n", register_name(e->reg), "# comparsion result set to true");
    fprintf(file, "%s:\n", next_label);

    break;

  case EXPR_AND:
  case EXPR_OR:

    expr_codegen(e->left, file);
    expr_codegen(e->right, file);

    fprintf(file, "%s %s, %s %s\n", (e->kind == EXPR_AND ? "ANDQ" : "ORQ"), register_name(e->left->reg), register_name(e->right->reg), "# logic AND");

    register_free(e->left->reg);
		e->left->reg = -1;

    e->reg = register_alloc();
    fprintf(file, "MOVQ %s, %s %s\n", register_name(e->right->reg), register_name(e->reg), "# move result back to expr");

    register_free(e->right->reg);
		e->right->reg = -1;
    break;


  case EXPR_ASSIGN: // left part should always be an variable, so do not allocate register for it

    expr_codegen(e->left, file);
    expr_codegen(e->right, file);

    // move to register or move to stack
    fprintf(file, "MOVQ %s, %s %s\n", register_name(e->right->reg), symbol_code(e->left->symbol), "# assign result to variable");

    register_free(e->right->reg);
		e->right->reg = -1;
		register_free(e->left->reg);
		e->left->reg = -1;
    break;

  case EXPR_PARENT:
    expr_codegen(e->right, file);
    fprintf(file, "MOVQ %s, %%rax %s\n", register_name(e->right->reg), "# move to tmp");
    register_free(e->right->reg);
		e->right->reg = -1;

    e->reg = register_alloc();
    fprintf(file, "MOVQ %%rax, %s %s\n", register_name(e->reg), "# move back to expr");
    break;

  case EXPR_CURLY:
		printf("codegen error: does not support array\n");
    // check how expr_list are structurred
    break;

  case EXPR_SUBSCRIPT:
    printf("codegen error: does support array subscript\n");
    break;

  }

}

char *symbol_code(struct symbol *s) {
	char *name = (char *)malloc(sizeof(*name) * 1025);
	if (s->kind == SYMBOL_GLOBAL) { // return name
		sprintf(name, "%s", s->name);
		return name;
	} else { // return offset 
		sprintf(name, "-%d(%%rbp)", (s->which + (s->kind == SYMBOL_LOCAL ? 0 : 1)) * 8);
		return name;
	}
}
