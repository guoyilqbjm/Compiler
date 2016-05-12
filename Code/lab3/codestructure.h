#ifndef CODE_H_
#define CODE_H_
#include <stdio.h>
#include <stdlib.h>
struct Operand{
	enum {TEMP, VARIABLE, CONSTANT, ADDRESS} kind;
	union{
		int temp_no;
		char *var_name;
		int value;
	} data;
};
typedef struct Operand Operand;
typedef struct Operand* OperandPoint;

struct InterCode{
//	enum { LABEL, FUNCTION, ASSIGN, ADD, SUB, MUL, DIV, GOTO, DEC, ARG, CALL, PARAM, READ, WRITE} kind;
	enum { ONEOP, BINOP, THREEOP } kind;
	union{
		struct{ OperandPoint right, left; int opkind;} oneop;
		struct{ OperandPoint result, op1, op2; int opkind;} binop;
	} data;
};

typedef struct InterCode InterCode;
struct InterCodes{
	struct InterCode code;
	struct InterCodes* last;
	struct InterCodes* next;
};
typedef struct InterCodes InterCodes;
extern struct InterCodes* code_root;
extern struct InterCodes* code_tail;

void insert(InterCodes* data);

void printInterCodes(char *filename);

#endif
