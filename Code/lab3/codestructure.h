#ifndef CODE_H_
#define CODE_H_
#include <stdio.h>
#include <stdlib.h>
struct Operand{
	enum {TEMP, VARIABLE,CONSTANT, ADDRESS} kind;
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
	enum { PARAM, FUNCTIONLABEL, LABEL, GOTO, RETURNFUNCTION, ARG, ONEOP, BINOP, CALL, DEC, IFSTMT} kind;//CALL 存储在funcall，DEC存储在decstmt IFSTMT存储在ifstmt;
	union{
		char symbol_name[20];
		struct{ OperandPoint left; char *fun_name;} funcall;
		struct{ OperandPoint left; int size;} decstmt;
		struct{ OperandPoint right, left; int opkind;} oneop;
		struct{ OperandPoint result, op1, op2; int opkind;} binop;
		struct{ OperandPoint left,right;char relop[3];char label[5];} ifstmt;
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


InterCodes* mergeInterCodes(InterCodes* a,InterCodes *b);


void printInterCodes(char *filename);


#endif
