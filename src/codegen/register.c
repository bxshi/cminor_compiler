#include "register.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int default_registers[16] = {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};

struct register_elem {
	int registers[16];
	struct register_elem *prev;
	struct register_elem *next;
};

struct register_elem *registers = 0;

void init_registers() {
	if (!registers) {
		registers = (struct register_elem*)malloc(sizeof(*registers));
		memcpy(registers->registers, default_registers, sizeof(int) * 16);
	}
}

void register_stepin() {
	struct register_elem *elem = (struct register_elem*)malloc(sizeof(*elem));
	memcpy(elem->registers, default_registers, sizeof(int) * 16);
	registers->next = elem;
	elem->prev = registers;
	registers = elem;
}

void register_stepout() {
	if (registers->prev)
		registers = registers->prev;
}

int register_alloc() {
	int i = 0;
	for(i = 0; i < 16; i++) {
		if (!registers->registers[i]) {
			registers->registers[i] = 1;
			return i;
		}
	}
	printf("codegen error: no more registers!\n");
	return -1;
}

// do not free reserved registers
void register_free(int r) {
	if (r == 1 || (r >= 10 && r <= 15)) {
		registers->registers[r] = 0;
	}
}

const char *register_name(int r) {
	switch(r) {
		case 0:
			return "%rax";
		case 1:
			return "%rbx";
		case 2:
			return "%rcx";
		case 3:
			return "%rdx";
		case 4:
			return "%rsi";
		case 5:
			return "%rdi";
		case 6:
			return "%rbp";
		case 7:
			return "%rsp";
		case 8:
			return "%r8";
		case 9:
			return "%r9";
		case 10:
			return "%r10";
		case 11:
			return "%r11";
		case 12:
			return "%r12";
		case 13:
			return "%r13";
		case 14:
			return "%r14";
		case 15:
			return "%r15";
		default:
			return "UNDEFINED_REGISTER";
	}
}
