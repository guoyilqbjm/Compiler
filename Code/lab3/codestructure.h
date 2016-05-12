#ifndef CODE_H_
#define CODE_H_
#include <stdio.h>
#include <stdlib.h>
struct Operand{
	enum {VARIABLE, CONSTANT, ADDRESS} kind;
	union{
		int var_no;
		int value;
	} data;
};
typedef struct Operand Operand;
typedef struct Operand* OperandPoint;

struct InterCode{
//	enum { LABEL, FUNCTION, ASSIGN, ADD, SUB, MUL, DIV, GOTO, DEC, ARG, CALL, PARAM, READ, WRITE} kind;
	enum { LABEL } kind;
	union{
		struct{ OperandPoint right, left;} assign;
		struct{ OperandPoint result, op1, op2;} binop;
		char *defname;
		OperandPoint oneop;
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
